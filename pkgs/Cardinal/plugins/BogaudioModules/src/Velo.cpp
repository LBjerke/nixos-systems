
#include "Velo.hpp"

#define LEVEL_SCALES_CV "level_scales_cv"

bool Velo::LevelParamQuantity::isLinear() {
	return dynamic_cast<Velo*>(module)->isLinear();
}

void Velo::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	for (int c = 0; c < maxChannels; ++c) {
		_levelSL[c].setParams(sampleRate, 5.0f, 1.0f);
		_velocitySL[c].setParams(sampleRate, 5.0f, 1.0f);
	}
}

json_t* Velo::saveToJson(json_t* root) {
	json_object_set_new(root, LEVEL_SCALES_CV, json_boolean(_levelScalesCV));
	return root;
}

void Velo::loadFromJson(json_t* root) {
	json_t* s = json_object_get(root, LEVEL_SCALES_CV);
	if (s) {
		_levelScalesCV = json_boolean_value(s);
	}
}

bool Velo::active() {
	return inputs[IN_INPUT].isConnected() && outputs[OUT_OUTPUT].isConnected();
}

void Velo::modulate() {
	_velocityDb = clamp(params[VELOCITY_PARAM].getValue(), 0.0f, 1.0f);
	_velocityDb *= _velocityDb;
	_velocityDb *= Amplifier::minDecibels;
	assert(_velocityDb <= 0.0f);
}

void Velo::processAll(const ProcessArgs& args) {
	int n = inputs[IN_INPUT].getChannels();
	outputs[OUT_OUTPUT].setChannels(n);
	for (int c = 0; c < n; ++c) {
		float level = clamp(params[LEVEL_PARAM].getValue(), 0.0f, 1.0f);
		if (inputs[LEVEL_INPUT].isConnected()) {
			level *= clamp(inputs[LEVEL_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		if (inputs[CV_INPUT].isConnected()) {
			float cv = clamp(inputs[CV_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
			cv *= clamp(params[LEVEL_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
			if (_levelScalesCV) {
				level += level * cv;
			}
			else {
				level += cv;
			}
		}
		level = clamp(level, 0.0f, 2.0f);
		level = _levelSL[c].next(level);

		float velocity = 1.0f;
		if (inputs[VELOCITY_INPUT].isConnected()) {
			velocity = clamp(inputs[VELOCITY_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		velocity = _velocitySL[c].next(velocity);
		_velocity[c].setLevel((1.0f - velocity) * _velocityDb);

		float out = inputs[IN_INPUT].getVoltage(c);
		if (isLinear()) {
			out *= level;
		}
		else {
			level = 1.0f - level;
			level *= Amplifier::minDecibels;
			level = std::min(level, 12.0f);
			_amplifier[c].setLevel(level);
			out = _amplifier[c].next(out);
		}
		out = _velocity[c].next(out);
		out = _saturator[c].next(out);
		outputs[OUT_OUTPUT].setVoltage(out, c);
	}
}

struct VeloWidget : BGModuleWidget {
	static constexpr int hp = 3;

	VeloWidget(Velo* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Velo");
		createScrews();

		// generated by svg_widgets.rb
		auto levelParamPosition = Vec(9.5, 34.5);
		auto levelAttenuatorParamPosition = Vec(14.5, 75.0);
		auto velocityParamPosition = Vec(14.5, 115.0);
		auto linearParamPosition = Vec(25.5, 143.0);

		auto levelInputPosition = Vec(10.5, 160.0);
		auto cvInputPosition = Vec(10.5, 195.0);
		auto velocityInputPosition = Vec(10.5, 230.0);
		auto inInputPosition = Vec(10.5, 265.0);

		auto outOutputPosition = Vec(10.5, 303.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(levelParamPosition, module, Velo::LEVEL_PARAM));
		addParam(createParam<Knob16>(levelAttenuatorParamPosition, module, Velo::LEVEL_ATTENUATOR_PARAM));
		addParam(createParam<Knob16>(velocityParamPosition, module, Velo::VELOCITY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearParamPosition, module, Velo::LINEAR_PARAM));

		addInput(createInput<Port24>(levelInputPosition, module, Velo::LEVEL_INPUT));
		addInput(createInput<Port24>(cvInputPosition, module, Velo::CV_INPUT));
		addInput(createInput<Port24>(velocityInputPosition, module, Velo::VELOCITY_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Velo::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Velo::OUT_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<Velo*>(module);
		assert(m);
		menu->addChild(new BoolOptionMenuItem("Level knob/CV scales bipolar CV", [m]() { return &m->_levelScalesCV; }));
	}
};

Model* modelVelo = createModel<Velo, VeloWidget>("Bogaudio-Velo", "VELO", "Triple-CV VCA, for tremolo, MIDI velocity, etc", "VCA", "Polyphonic");
