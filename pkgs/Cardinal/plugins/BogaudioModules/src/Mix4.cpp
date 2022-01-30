
#include "Mix4.hpp"

#define POLY_OFFSET "poly_channel_offset"

void Mix4::onRandomize(const RandomizeEvent& e) {
	Module::onRandomize(e);
	for (int i = 0; i < 4; ++i) {
		getParamQuantity(MUTE1_PARAM + 3*i)->setValue(random::uniform() > 0.5f);
	}
}

json_t* Mix4::saveToJson(json_t* root) {
	root = DimmableMixerModule::saveToJson(root);
	json_object_set_new(root, POLY_OFFSET, json_integer(_polyChannelOffset));
	return root;
}

void Mix4::loadFromJson(json_t* root) {
	DimmableMixerModule::loadFromJson(root);
	json_t* o = json_object_get(root, POLY_OFFSET);
	if (o) {
		_polyChannelOffset = json_integer_value(o);
	}
}

void Mix4::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < 4; ++i) {
		_channels[i]->setSampleRate(sr);
		_panSLs[i].setParams(sr, MIXER_PAN_SLEW_MS, 2.0f);
	}
	_slewLimiter.setParams(sr, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
	_levelCVSL.setParams(sr, MixerChannel::levelSlewTimeMS, 1.0f);
	_rms.setSampleRate(sr);
}

void Mix4::processAll(const ProcessArgs& args) {
	Mix4ExpanderMessage* toExp = &_dummyExpanderMessage;
	Mix4ExpanderMessage* fromExp = &_dummyExpanderMessage;
	if (expanderConnected()) {
		toExp = toExpander();
		fromExp = fromExpander();
	}

	if (!(
		inputs[IN1_INPUT].isConnected() ||
		inputs[IN2_INPUT].isConnected() ||
		inputs[IN3_INPUT].isConnected() ||
		inputs[IN4_INPUT].isConnected()
	)) {
		if (_wasActive > 0) {
			--_wasActive;
			for (int i = 0; i < 4; ++i) {
				_channels[i]->reset();
				toExp->active[i] = false;
			}
			_rmsLevel = 0.0f;
			outputs[L_OUTPUT].setVoltage(0.0f);
			outputs[R_OUTPUT].setVoltage(0.0f);
		}
		return;
	}
	_wasActive = 2;

	bool solo =
		params[MUTE1_PARAM].getValue() > 1.5f ||
		params[MUTE2_PARAM].getValue() > 1.5f ||
		params[MUTE3_PARAM].getValue() > 1.5f ||
		params[MUTE4_PARAM].getValue() > 1.5f;

	{
		float sample = 0.0f;
		if (_polyChannelOffset >= 0) {
			sample = inputs[IN1_INPUT].getPolyVoltage(_polyChannelOffset);
		} else {
			sample = inputs[IN1_INPUT].getVoltageSum();
		}
		_channels[0]->next(sample, solo, 0, _linearCV);
		toExp->preFader[0] = sample;
		toExp->active[0] = inputs[IN1_INPUT].isConnected();

		for (int i = 1; i < 4; ++i) {
			float sample = 0.0f;
			if (inputs[IN1_INPUT + 3 * i].isConnected()) {
				sample = inputs[IN1_INPUT + 3 * i].getVoltageSum();
				_channels[i]->next(sample, solo, 0, _linearCV);
				_channelActive[i] = true;
			}
			else if (_polyChannelOffset >= 0) {
				sample = inputs[IN1_INPUT].getPolyVoltage(_polyChannelOffset + i);
				_channels[i]->next(sample, solo, 0, _linearCV);
				_channelActive[i] = true;
			}
			else if (_channelActive[i]) {
				_channels[i]->reset();
				_channelActive[i] = false;
			}
			toExp->preFader[i] = sample;
			toExp->active[i] = _channelActive[i];
		}
	}

	float levelCV = 1.0f;
	if (inputs[MIX_CV_INPUT].isConnected()) {
		levelCV = clamp(inputs[MIX_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	float level = Amplifier::minDecibels;
	if (params[MIX_MUTE_PARAM].getValue() < 0.5f) {
		level = params[MIX_PARAM].getValue();
		if (!_linearCV) {
			level *= levelCV;
		}
		level *= MixerChannel::maxDecibels - MixerChannel::minDecibels;
		level += MixerChannel::minDecibels;
		if (params[MIX_DIM_PARAM].getValue() > 0.5f) {
			level = std::max(Amplifier::minDecibels, level - _dimDb);
		}
	}
	_amplifier.setLevel(_slewLimiter.next(level));
	levelCV = _levelCVSL.next(levelCV);

	float outs[4];
	for (int i = 0; i < 4; ++i) {
		toExp->postFader[i] = outs[i] = _channels[i]->out;
	}

	float mono = 0.0f;
	float left = 0.0f;
	float right = 0.0f;
	if (expanderConnected()) {
		mono += fromExp->returnA[0] + fromExp->returnB[0];
		left += fromExp->returnA[0] + fromExp->returnB[0];
		right += fromExp->returnA[1] + fromExp->returnB[1];
		std::copy(fromExp->postEQ, fromExp->postEQ + 4, outs);
	}

	for (int i = 0; i < 4; ++i) {
		mono += outs[i];
	}
	mono = _amplifier.next(mono);
	if (_linearCV) {
		mono *= levelCV;
	}
	mono = _saturator.next(mono);
	_rmsLevel = _rms.next(mono) / 5.0f;

	if (outputs[L_OUTPUT].isConnected() && outputs[R_OUTPUT].isConnected()) {
		for (int i = 0; i < 4; ++i) {
			float pan = clamp(params[PAN1_PARAM + 3 * i].getValue(), -1.0f, 1.0f);
			if (inputs[PAN1_INPUT + 3 * i].isConnected()) {
				pan *= clamp(inputs[PAN1_INPUT + 3 * i].getVoltage() / 5.0f, -1.0f, 1.0f);
			}
			_panners[i].setPan(_panSLs[i].next(pan));
			float l, r;
			_panners[i].next(outs[i], l, r);
			left += l;
			right += r;
		}

		left = _amplifier.next(left);
		if (_linearCV) {
			left *= levelCV;
		}
		left = _saturator.next(left);
		outputs[L_OUTPUT].setVoltage(left);

		right = _amplifier.next(right);
		if (_linearCV) {
			right *= levelCV;
		}
		right = _saturator.next(right);
		outputs[R_OUTPUT].setVoltage(right);
	}
	else {
		outputs[L_OUTPUT].setVoltage(mono);
		outputs[R_OUTPUT].setVoltage(mono);
	}
}

struct Mix4Widget : DimmableMixerWidget {
	static constexpr int hp = 15;

	Mix4Widget(Mix4* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Mix4");
		createScrews();

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(17.5, 32.0);
		auto mute1ParamPosition = Vec(17.5, 185.0);
		auto pan1ParamPosition = Vec(18.5, 223.0);
		auto level2ParamPosition = Vec(60.5, 32.0);
		auto mute2ParamPosition = Vec(60.5, 185.0);
		auto pan2ParamPosition = Vec(61.5, 223.0);
		auto level3ParamPosition = Vec(103.5, 32.0);
		auto mute3ParamPosition = Vec(103.5, 185.0);
		auto pan3ParamPosition = Vec(104.5, 223.0);
		auto level4ParamPosition = Vec(146.5, 32.0);
		auto mute4ParamPosition = Vec(146.5, 185.0);
		auto pan4ParamPosition = Vec(147.5, 223.0);
		auto mixParamPosition = Vec(189.5, 32.0);
		auto mixMuteParamPosition = Vec(189.5, 185.0);
		auto mixDimParamPosition = Vec(189.5, 218.0);

		auto cv1InputPosition = Vec(14.5, 255.0);
		auto pan1InputPosition = Vec(14.5, 290.0);
		auto in1InputPosition = Vec(14.5, 325.0);
		auto cv2InputPosition = Vec(57.5, 255.0);
		auto pan2InputPosition = Vec(57.5, 290.0);
		auto in2InputPosition = Vec(57.5, 325.0);
		auto cv3InputPosition = Vec(100.5, 255.0);
		auto pan3InputPosition = Vec(100.5, 290.0);
		auto in3InputPosition = Vec(100.5, 325.0);
		auto cv4InputPosition = Vec(143.5, 255.0);
		auto pan4InputPosition = Vec(143.5, 290.0);
		auto in4InputPosition = Vec(143.5, 325.0);
		auto mixCvInputPosition = Vec(186.5, 252.0);

		auto lOutputPosition = Vec(186.5, 290.0);
		auto rOutputPosition = Vec(186.5, 325.0);
		// end generated by svg_widgets.rb

		addSlider(level1ParamPosition, module, Mix4::LEVEL1_PARAM, module ? &module->_channels[0]->rms : NULL);
		addParam(createParam<Knob16>(pan1ParamPosition, module, Mix4::PAN1_PARAM));
		addParam(createParam<SoloMuteButton>(mute1ParamPosition, module, Mix4::MUTE1_PARAM));
		addSlider(level2ParamPosition, module, Mix4::LEVEL2_PARAM, module ? &module->_channels[1]->rms : NULL);
		addParam(createParam<Knob16>(pan2ParamPosition, module, Mix4::PAN2_PARAM));
		addParam(createParam<SoloMuteButton>(mute2ParamPosition, module, Mix4::MUTE2_PARAM));
		addSlider(level3ParamPosition, module, Mix4::LEVEL3_PARAM, module ? &module->_channels[2]->rms : NULL);
		addParam(createParam<Knob16>(pan3ParamPosition, module, Mix4::PAN3_PARAM));
		addParam(createParam<SoloMuteButton>(mute3ParamPosition, module, Mix4::MUTE3_PARAM));
		addSlider(level4ParamPosition, module, Mix4::LEVEL4_PARAM, module ? &module->_channels[3]->rms : NULL);
		addParam(createParam<Knob16>(pan4ParamPosition, module, Mix4::PAN4_PARAM));
		addParam(createParam<SoloMuteButton>(mute4ParamPosition, module, Mix4::MUTE4_PARAM));
		addSlider(mixParamPosition, module, Mix4::MIX_PARAM, module ? &module->_rmsLevel : NULL);
		addParam(createParam<MuteButton>(mixMuteParamPosition, module, Mix4::MIX_MUTE_PARAM));
		addParam(createParam<MuteButton>(mixDimParamPosition, module, Mix4::MIX_DIM_PARAM));

		addInput(createInput<Port24>(cv1InputPosition, module, Mix4::CV1_INPUT));
		addInput(createInput<Port24>(pan1InputPosition, module, Mix4::PAN1_INPUT));
		addInput(createInput<Port24>(in1InputPosition, module, Mix4::IN1_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, Mix4::CV2_INPUT));
		addInput(createInput<Port24>(pan2InputPosition, module, Mix4::PAN2_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Mix4::IN2_INPUT));
		addInput(createInput<Port24>(cv3InputPosition, module, Mix4::CV3_INPUT));
		addInput(createInput<Port24>(pan3InputPosition, module, Mix4::PAN3_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Mix4::IN3_INPUT));
		addInput(createInput<Port24>(cv4InputPosition, module, Mix4::CV4_INPUT));
		addInput(createInput<Port24>(pan4InputPosition, module, Mix4::PAN4_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Mix4::IN4_INPUT));
		addInput(createInput<Port24>(mixCvInputPosition, module, Mix4::MIX_CV_INPUT));

		addOutput(createOutput<Port24>(lOutputPosition, module, Mix4::L_OUTPUT));
		addOutput(createOutput<Port24>(rOutputPosition, module, Mix4::R_OUTPUT));
	}

	void addSlider(Vec position, Mix4* module, int id, float* rms) {
		auto slider = createParam<VUSlider151>(position, module, id);
		if (rms) {
			dynamic_cast<VUSlider*>(slider)->setVULevel(rms);
		}
		addParam(slider);
	}

	void contextMenu(Menu* menu) override {
		DimmableMixerWidget::contextMenu(menu);
		auto m = dynamic_cast<Mix4*>(module);
		assert(m);
		OptionsMenuItem* mi = new OptionsMenuItem("Input 1 poly spread");
		mi->addItem(OptionMenuItem("None", [m]() { return m->_polyChannelOffset == -1; }, [m]() { m->_polyChannelOffset = -1; }));
		mi->addItem(OptionMenuItem("Channels 1-4", [m]() { return m->_polyChannelOffset == 0; }, [m]() { m->_polyChannelOffset = 0; }));
		mi->addItem(OptionMenuItem("Channels 5-8", [m]() { return m->_polyChannelOffset == 4; }, [m]() { m->_polyChannelOffset = 4; }));
		mi->addItem(OptionMenuItem("Channels 9-12", [m]() { return m->_polyChannelOffset == 8; }, [m]() { m->_polyChannelOffset = 8; }));
		mi->addItem(OptionMenuItem("Channels 13-16", [m]() { return m->_polyChannelOffset == 12; }, [m]() { m->_polyChannelOffset = 12; }));
		OptionsMenuItem::addToMenu(mi, menu);
	}
};

Model* modelMix4 = bogaudio::createModel<Mix4, Mix4Widget>("Bogaudio-Mix4", "MIX4", "4-channel mixer and panner", "Mixer", "Panning");
