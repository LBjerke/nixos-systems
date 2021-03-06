
#include "PEQ6.hpp"

void PEQ6::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->setSampleRate(sr);
	}
}

bool PEQ6::active() {
	return
		outputs[OUT_OUTPUT].isConnected() ||
		outputs[OUT1_OUTPUT].isConnected() ||
		outputs[OUT2_OUTPUT].isConnected() ||
		outputs[OUT3_OUTPUT].isConnected() ||
		outputs[OUT4_OUTPUT].isConnected() ||
		outputs[OUT5_OUTPUT].isConnected() ||
		outputs[OUT6_OUTPUT].isConnected() ||
		expanderConnected();
}

int PEQ6::channels() {
	return inputs[IN_INPUT].getChannels();
}

void PEQ6::addChannel(int c) {
	const int n = 6;
	_engines[c] = new PEQEngine(n);
	for (int i = 0; i < n; ++i) {
		_engines[c]->configChannel(
			i,
			c,
			params[LEVEL1_PARAM + i*3],
			params[FREQUENCY1_PARAM + i*3],
			params[FREQUENCY_CV1_PARAM + i*3],
			&params[FREQUENCY_CV_PARAM],
			params[BANDWIDTH_PARAM],
			inputs[LEVEL1_INPUT + i*2],
			inputs[FREQUENCY_CV1_INPUT + i*2],
			inputs[FREQUENCY_CV_INPUT],
			&inputs[BANDWIDTH_INPUT]
		);
	}
	_engines[c]->setSampleRate(APP->engine->getSampleRate());
}

void PEQ6::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ6::modulate() {
	_fullFrequencyMode = params[FMOD_PARAM].getValue() > 0.5f;

	auto lowMode = params[LP_PARAM].getValue() > 0.5f ? MultimodeFilter::LOWPASS_MODE : MultimodeFilter::BANDPASS_MODE;
	auto highMode = params[HP_PARAM].getValue() > 0.5f ? MultimodeFilter::HIGHPASS_MODE : MultimodeFilter::BANDPASS_MODE;
	for (int c = 0; c < _channels; ++c) {
		PEQEngine& e = *_engines[c];
		e.setLowFilterMode(lowMode);
		e.setHighFilterMode(highMode);
		e.setFrequencyMode(_fullFrequencyMode);
		e.modulate();
	}
}

void PEQ6::processAlways(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);
	for (int i = 0; i < 6; ++i) {
		outputs[OUT1_OUTPUT + i].setChannels(_channels);
	}
	std::fill(_rmsSums, _rmsSums + 6, 0.0f);

	_expanderMessage = NULL;
	if (expanderConnected()) {
		_expanderMessage = toExpander();
	}
}

void PEQ6::processChannel(const ProcessArgs& args, int c) {
	PEQEngine& e = *_engines[c];
	float out = e.next(inputs[IN_INPUT].getVoltage(c), _rmsSums);
	float beOut = 0.0f;
	for (int i = 0; i < 6; ++i) {
		if (outputs[OUT1_OUTPUT + i].isConnected()) {
			outputs[OUT1_OUTPUT + i].setVoltage(e.outs[i], c);
		}
		else {
			beOut += e.outs[i];
		}
	}
	if (_bandExclude) {
		outputs[OUT_OUTPUT].setVoltage(beOut, c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(out, c);
	}

	if (_expanderMessage) {
		std::copy(e.outs, e.outs + 6, _expanderMessage->outs[c]);
		std::copy(e.frequencies, e.frequencies + 6, _expanderMessage->frequencies[c]);
		_expanderMessage->bandwidths[c] = e.bandwidth;
	}
}

void PEQ6::postProcessAlways(const ProcessArgs& args) {
	for (int i = 0; i < 6; ++i) {
		_rms[i] = _rmsSums[i] * _inverseChannels;
	}

	bool ffm = params[FMOD_PARAM].getValue() > 0.5f;
	lights[FMOD_RELATIVE_LIGHT].value = !ffm;
	lights[FMOD_FULL_LIGHT].value = ffm;
}

struct PEQ6Widget : BandExcludeModuleWidget {
	static constexpr int hp = 21;

	PEQ6Widget(PEQ6* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PEQ6");
		createScrews();

		// generated by svg_widgets.rb
		auto frequencyCvParamPosition = Vec(14.5, 47.0);
		auto bandwidthParamPosition = Vec(14.5, 129.0);
		auto lpParamPosition = Vec(35.5, 210.0);
		auto hpParamPosition = Vec(35.5, 222.0);
		auto fmodParamPosition = Vec(20.0, 267.0);
		auto level1ParamPosition = Vec(58.5, 28.0);
		auto frequency1ParamPosition = Vec(59.5, 184.0);
		auto frequencyCv1ParamPosition = Vec(59.5, 224.0);
		auto level2ParamPosition = Vec(103.5, 28.0);
		auto frequency2ParamPosition = Vec(104.5, 184.0);
		auto frequencyCv2ParamPosition = Vec(104.5, 224.0);
		auto level3ParamPosition = Vec(148.5, 28.0);
		auto frequency3ParamPosition = Vec(149.5, 184.0);
		auto frequencyCv3ParamPosition = Vec(149.5, 224.0);
		auto level4ParamPosition = Vec(193.5, 28.0);
		auto frequency4ParamPosition = Vec(194.5, 184.0);
		auto frequencyCv4ParamPosition = Vec(194.5, 224.0);
		auto level5ParamPosition = Vec(238.5, 28.0);
		auto frequency5ParamPosition = Vec(239.5, 184.0);
		auto frequencyCv5ParamPosition = Vec(239.5, 224.0);
		auto level6ParamPosition = Vec(283.5, 28.0);
		auto frequency6ParamPosition = Vec(284.5, 183.0);
		auto frequencyCv6ParamPosition = Vec(284.5, 224.0);

		auto frequencyCvInputPosition = Vec(10.5, 79.0);
		auto bandwidthInputPosition = Vec(10.5, 163.0);
		auto inInputPosition = Vec(10.5, 290.0);
		auto level1InputPosition = Vec(55.5, 255.0);
		auto frequencyCv1InputPosition = Vec(55.5, 290.0);
		auto level2InputPosition = Vec(100.5, 255.0);
		auto frequencyCv2InputPosition = Vec(100.5, 290.0);
		auto level3InputPosition = Vec(145.5, 255.0);
		auto frequencyCv3InputPosition = Vec(145.5, 290.0);
		auto level4InputPosition = Vec(190.5, 255.0);
		auto frequencyCv4InputPosition = Vec(190.5, 290.0);
		auto level5InputPosition = Vec(235.5, 255.0);
		auto frequencyCv5InputPosition = Vec(235.5, 290.0);
		auto level6InputPosition = Vec(280.5, 255.0);
		auto frequencyCv6InputPosition = Vec(280.5, 290.0);

		auto outOutputPosition = Vec(10.5, 325.0);
		auto out1OutputPosition = Vec(55.5, 325.0);
		auto out2OutputPosition = Vec(100.5, 325.0);
		auto out3OutputPosition = Vec(145.5, 325.0);
		auto out4OutputPosition = Vec(190.5, 325.0);
		auto out5OutputPosition = Vec(235.5, 325.0);
		auto out6OutputPosition = Vec(280.5, 325.0);

		auto fmodRelativeLightPosition = Vec(16.0, 243.0);
		auto fmodFullLightPosition = Vec(16.0, 256.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(frequencyCvParamPosition, module, PEQ6::FREQUENCY_CV_PARAM));
		addParam(createParam<Knob16>(bandwidthParamPosition, module, PEQ6::BANDWIDTH_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(lpParamPosition, module, PEQ6::LP_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(hpParamPosition, module, PEQ6::HP_PARAM));
		addParam(createParam<StatefulButton9>(fmodParamPosition, module, PEQ6::FMOD_PARAM));
		addSlider(level1ParamPosition, module, PEQ6::LEVEL1_PARAM, module ? &module->_rms[0] : NULL);
		addParam(createParam<Knob16>(frequency1ParamPosition, module, PEQ6::FREQUENCY1_PARAM));
		addParam(createParam<Knob16>(frequencyCv1ParamPosition, module, PEQ6::FREQUENCY_CV1_PARAM));
		addSlider(level2ParamPosition, module, PEQ6::LEVEL2_PARAM, module ? &module->_rms[1] : NULL);
		addParam(createParam<Knob16>(frequency2ParamPosition, module, PEQ6::FREQUENCY2_PARAM));
		addParam(createParam<Knob16>(frequencyCv2ParamPosition, module, PEQ6::FREQUENCY_CV2_PARAM));
		addSlider(level3ParamPosition, module, PEQ6::LEVEL3_PARAM, module ? &module->_rms[2] : NULL);
		addParam(createParam<Knob16>(frequency3ParamPosition, module, PEQ6::FREQUENCY3_PARAM));
		addParam(createParam<Knob16>(frequencyCv3ParamPosition, module, PEQ6::FREQUENCY_CV3_PARAM));
		addSlider(level4ParamPosition, module, PEQ6::LEVEL4_PARAM, module ? &module->_rms[3] : NULL);
		addParam(createParam<Knob16>(frequency4ParamPosition, module, PEQ6::FREQUENCY4_PARAM));
		addParam(createParam<Knob16>(frequencyCv4ParamPosition, module, PEQ6::FREQUENCY_CV4_PARAM));
		addSlider(level5ParamPosition, module, PEQ6::LEVEL5_PARAM, module ? &module->_rms[4] : NULL);
		addParam(createParam<Knob16>(frequency5ParamPosition, module, PEQ6::FREQUENCY5_PARAM));
		addParam(createParam<Knob16>(frequencyCv5ParamPosition, module, PEQ6::FREQUENCY_CV5_PARAM));
		addSlider(level6ParamPosition, module, PEQ6::LEVEL6_PARAM, module ? &module->_rms[5] : NULL);
		addParam(createParam<Knob16>(frequency6ParamPosition, module, PEQ6::FREQUENCY6_PARAM));
		addParam(createParam<Knob16>(frequencyCv6ParamPosition, module, PEQ6::FREQUENCY_CV6_PARAM));

		addInput(createInput<Port24>(frequencyCvInputPosition, module, PEQ6::FREQUENCY_CV_INPUT));
		addInput(createInput<Port24>(bandwidthInputPosition, module, PEQ6::BANDWIDTH_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, PEQ6::IN_INPUT));
		addInput(createInput<Port24>(level1InputPosition, module, PEQ6::LEVEL1_INPUT));
		addInput(createInput<Port24>(frequencyCv1InputPosition, module, PEQ6::FREQUENCY_CV1_INPUT));
		addInput(createInput<Port24>(level2InputPosition, module, PEQ6::LEVEL2_INPUT));
		addInput(createInput<Port24>(frequencyCv2InputPosition, module, PEQ6::FREQUENCY_CV2_INPUT));
		addInput(createInput<Port24>(level3InputPosition, module, PEQ6::LEVEL3_INPUT));
		addInput(createInput<Port24>(frequencyCv3InputPosition, module, PEQ6::FREQUENCY_CV3_INPUT));
		addInput(createInput<Port24>(level4InputPosition, module, PEQ6::LEVEL4_INPUT));
		addInput(createInput<Port24>(frequencyCv4InputPosition, module, PEQ6::FREQUENCY_CV4_INPUT));
		addInput(createInput<Port24>(level5InputPosition, module, PEQ6::LEVEL5_INPUT));
		addInput(createInput<Port24>(frequencyCv5InputPosition, module, PEQ6::FREQUENCY_CV5_INPUT));
		addInput(createInput<Port24>(level6InputPosition, module, PEQ6::LEVEL6_INPUT));
		addInput(createInput<Port24>(frequencyCv6InputPosition, module, PEQ6::FREQUENCY_CV6_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, PEQ6::OUT_OUTPUT));
		addOutput(createOutput<Port24>(out1OutputPosition, module, PEQ6::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, PEQ6::OUT2_OUTPUT));
		addOutput(createOutput<Port24>(out3OutputPosition, module, PEQ6::OUT3_OUTPUT));
		addOutput(createOutput<Port24>(out4OutputPosition, module, PEQ6::OUT4_OUTPUT));
		addOutput(createOutput<Port24>(out5OutputPosition, module, PEQ6::OUT5_OUTPUT));
		addOutput(createOutput<Port24>(out6OutputPosition, module, PEQ6::OUT6_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(fmodFullLightPosition, module, PEQ6::FMOD_FULL_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(fmodRelativeLightPosition, module, PEQ6::FMOD_RELATIVE_LIGHT));
	}

	void addSlider(Vec position, PEQ6* module, int id, float* rms) {
		auto slider = createParam<VUSlider151>(position, module, id);
		if (rms) {
			dynamic_cast<VUSlider*>(slider)->setVULevel(rms);
		}
		addParam(slider);
	}
};

Model* modelPEQ6 = createModel<PEQ6, PEQ6Widget>("Bogaudio-PEQ6", "PEQ6", "6-channel parametric equalizer / filter bank", "Filter", "Polyphonic");
