
#include "AD.hpp"

#define INVERT "invert"

void AD::Engine::reset() {
	trigger.reset();
	eocPulseGen.process(10.0);
	envelope.reset();
	on = false;
}

void AD::Engine::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	envelope.setSampleRate(sr);
	attackSL.setParams(sr / (float)modulationSteps);
	decaySL.setParams(sr / (float)modulationSteps);
}

void AD::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void AD::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

json_t* AD::saveToJson(json_t* root) {
	json_object_set_new(root, INVERT, json_real(_invert));
	return root;
}

void AD::loadFromJson(json_t* root) {
	json_t* i = json_object_get(root, INVERT);
	if (i) {
		_invert = json_real_value(i);
	}
}

bool AD::active() {
	return inputs[TRIGGER_INPUT].isConnected() || outputs[ENV_OUTPUT].isConnected() || outputs[EOC_OUTPUT].isConnected();
}

int AD::channels() {
	return inputs[TRIGGER_INPUT].getChannels();
}

void AD::addChannel(int c) {
	_engines[c] = new Engine(_modulationSteps);
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
}

void AD::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void AD::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float attack = powf(params[ATTACK_PARAM].getValue(), 2.0f);
	if (inputs[ATTACK_INPUT].isConnected()) {
		attack *= clamp(inputs[ATTACK_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.envelope.setAttack(e.attackSL.next(attack * 10.f));

	float decay = powf(params[DECAY_PARAM].getValue(), 2.0f);
	if (inputs[DECAY_INPUT].isConnected()) {
		decay *= clamp(inputs[DECAY_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.envelope.setDecay(e.decaySL.next(decay * 10.f));

	e.envelope.setLinearShape(_linearMode);

	_retriggerMode = params[RETRIGGER_PARAM].getValue() > 0.5f;
	_loopMode = params[LOOP_PARAM].getValue() > 0.5f;
	_linearMode = params[LINEAR_PARAM].getValue() > 0.5f;
}

void AD::processAlways(const ProcessArgs& args) {
	_attackLightSum = _decayLightSum = 0;
}

void AD::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	bool start = e.trigger.process(inputs[TRIGGER_INPUT].getVoltage(c));
	if (!e.on && (
		start ||
		(_retriggerMode && e.trigger.isHigh()) ||
		(_loopMode && e.envelope.isStage(ADSR::STOPPED_STAGE))
	)) {
		e.on = true;
	} else if (e.on && start && _retriggerMode) {
		if (_loopMode) {
			e.envelope.reset();
		}
		else {
			e.envelope.retrigger();
		}
	}
	e.envelope.setGate(e.on);
	outputs[ENV_OUTPUT].setChannels(_channels);
	outputs[ENV_OUTPUT].setVoltage(e.envelope.next() * 10.0f * _invert, c);
	if (e.on && e.envelope.isStage(ADSR::SUSTAIN_STAGE)) {
		e.envelope.reset();
		e.on = false;
		e.eocPulseGen.trigger(0.001f);
	}
	outputs[EOC_OUTPUT].setChannels(_channels);
	outputs[EOC_OUTPUT].setVoltage(e.eocPulseGen.process(APP->engine->getSampleTime()) ? 5.0f : 0.0f, c);

	_attackLightSum += e.envelope.isStage(ADSR::ATTACK_STAGE);
	_decayLightSum += e.envelope.isStage(ADSR::DECAY_STAGE);
}

void AD::postProcessAlways(const ProcessArgs& args) {
	lights[ATTACK_LIGHT].value = _attackLightSum * _inverseChannels;
	lights[DECAY_LIGHT].value = _decayLightSum * _inverseChannels;
}

struct ADWidget : BGModuleWidget {
	static constexpr int hp = 3;

	ADWidget(AD* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "AD");
		createScrews();

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(8.0, 33.0);
		auto decayParamPosition = Vec(8.0, 90.0);
		auto retriggerParamPosition = Vec(12.0, 131.5);
		auto loopParamPosition = Vec(33.0, 131.5);
		auto linearParamPosition = Vec(26.0, 146.0);

		auto triggerInputPosition = Vec(10.5, 163.5);
		auto attackInputPosition = Vec(10.5, 198.5);
		auto decayInputPosition = Vec(10.5, 233.5);

		auto envOutputPosition = Vec(10.5, 271.5);
		auto eocOutputPosition = Vec(10.5, 306.5);

		auto attackLightPosition = Vec(20.8, 65.0);
		auto decayLightPosition = Vec(20.8, 122.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(attackParamPosition, module, AD::ATTACK_PARAM));
		addParam(createParam<Knob29>(decayParamPosition, module, AD::DECAY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(loopParamPosition, module, AD::LOOP_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearParamPosition, module, AD::LINEAR_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(retriggerParamPosition, module, AD::RETRIGGER_PARAM));

		addInput(createInput<Port24>(triggerInputPosition, module, AD::TRIGGER_INPUT));
		addInput(createInput<Port24>(attackInputPosition, module, AD::ATTACK_INPUT));
		addInput(createInput<Port24>(decayInputPosition, module, AD::DECAY_INPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, AD::ENV_OUTPUT));
		addOutput(createOutput<Port24>(eocOutputPosition, module, AD::EOC_OUTPUT));

		addChild(createLight<BGTinyLight<GreenLight>>(attackLightPosition, module, AD::ATTACK_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(decayLightPosition, module, AD::DECAY_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<AD*>(module);
		assert(m);
		menu->addChild(new OptionMenuItem(
			"Invert output",
			[m]() { return m->_invert == -1.0f; },
			[m]() {
				if (m->_invert < 0.0f) {
					m->_invert = 1.0f;
				}
				else {
					m->_invert = -1.0f;
				}
			}
		));
	}
};

Model* modelAD = bogaudio::createModel<AD, ADWidget>("Bogaudio-AD", "AD", "Attack/decay envelope generator", "Envelope generator", "Polyphonic");
