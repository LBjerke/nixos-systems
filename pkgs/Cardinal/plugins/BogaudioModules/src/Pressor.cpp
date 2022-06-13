
#include "Pressor.hpp"

#define THRESHOLD_RANGE "threshold_range"

void Pressor::Engine::sampleRateChange() {
	detectorRMS.setSampleRate(APP->engine->getSampleRate());
}

float Pressor::ThresholdParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	v *= 30.0f;
	v -= 24.0f;
	v *= dynamic_cast<Pressor*>(module)->_thresholdRange;
	return v;
}

void Pressor::ThresholdParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}
	Pressor* m = dynamic_cast<Pressor*>(module);
	v /= m->_thresholdRange;
	v = clamp(v, -24.0f, 6.0f);
	v += 24.0f;
	v /= 30.0f;
	setValue(v);
}

void Pressor::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

json_t* Pressor::saveToJson(json_t* root) {
	json_object_set_new(root, THRESHOLD_RANGE, json_real(_thresholdRange));
	return root;
}

void Pressor::loadFromJson(json_t* root) {
	json_t* tr = json_object_get(root, THRESHOLD_RANGE);
	if (tr) {
		_thresholdRange = std::max(0.0f, (float)json_real_value(tr));
	}
}

bool Pressor::active() {
	return (
		outputs[LEFT_OUTPUT].isConnected() ||
		outputs[RIGHT_OUTPUT].isConnected() ||
		outputs[ENVELOPE_OUTPUT].isConnected() ||
		outputs[LEFT_INPUT].isConnected() ||
		outputs[RIGHT_INPUT].isConnected() ||
		outputs[SIDECHAIN_INPUT].isConnected()
	);
}

int Pressor::channels() {
	return inputs[LEFT_INPUT].getChannels();
}

void Pressor::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->sampleRateChange();
}

void Pressor::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void Pressor::modulate() {
	_compressorMode = params[MODE_PARAM].getValue() > 0.5f;
	_rmsDetector = params[DECTECTOR_MODE_PARAM].getValue() > 0.5f;
	_softKnee = params[KNEE_PARAM].getValue() > 0.5f;
}

void Pressor::modulateChannel(int c) {
	Engine& e = *_engines[c];

	e.thresholdDb = params[THRESHOLD_PARAM].getValue();
	if (inputs[THRESHOLD_INPUT].isConnected()) {
		e.thresholdDb *= clamp(inputs[THRESHOLD_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.thresholdDb *= 30.0f;
	e.thresholdDb -= 24.0f;
	e.thresholdDb *= _thresholdRange;

	float ratio = params[RATIO_PARAM].getValue();
	if (inputs[RATIO_INPUT].isConnected()) {
		ratio *= clamp(inputs[RATIO_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	if (e.ratioKnob != ratio) {
		e.ratioKnob = ratio;
		ratio = powf(ratio, 1.5f);
		ratio = 1.0f - ratio;
		ratio *= M_PI;
		ratio *= 0.25f;
		ratio = tanf(ratio);
		ratio = 1.0f / ratio;
		e.ratio = ratio;
	}

	float sampleRate = APP->engine->getSampleRate();
	float attack = params[ATTACK_PARAM].getValue();
	if (inputs[ATTACK_INPUT].isConnected()) {
		attack *= clamp(inputs[ATTACK_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	attack *= attack;
	e.attackSL.setParams(sampleRate, attack * 500.0f);

	float release = params[RELEASE_PARAM].getValue();
	if (inputs[RELEASE_INPUT].isConnected()) {
		release *= clamp(inputs[RELEASE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	release *= release;
	e.releaseSL.setParams(sampleRate, release * 2000.0f);

	float inGain = params[INPUT_GAIN_PARAM].getValue();
	if (inputs[INPUT_GAIN_INPUT].isConnected()) {
		inGain = clamp(inGain + inputs[INPUT_GAIN_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	inGain *= 12.0f;
	if (e.inGain != inGain) {
		e.inGain = inGain;
		e.inLevel = decibelsToAmplitude(e.inGain);
	}

	float outGain = params[OUTPUT_GAIN_PARAM].getValue();
	if (inputs[OUTPUT_GAIN_INPUT].isConnected()) {
		outGain = clamp(outGain + inputs[OUTPUT_GAIN_INPUT].getPolyVoltage(c) / 5.0f, 0.0f, 1.0f);
	}
	outGain *= 24.0f;
	if (e.outGain != outGain) {
		e.outGain = outGain;
		e.outLevel = decibelsToAmplitude(e.outGain);
	}

	e.detectorMix.setParams(params[DETECTOR_MIX_PARAM].getValue(), 0.0f, true);
}

void Pressor::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	float leftInput = inputs[LEFT_INPUT].getPolyVoltage(c) * e.inLevel;
	float rightInput = inputs[RIGHT_INPUT].getPolyVoltage(c) * e.inLevel;
	float env = leftInput + rightInput;
	if (inputs[SIDECHAIN_INPUT].isConnected()) {
		env = e.detectorMix.next(env, inputs[SIDECHAIN_INPUT].getPolyVoltage(c));
	}
	if (_rmsDetector) {
		env = e.detectorRMS.next(env);
	}
	else {
		env = fabsf(env);
	}
	if (env > e.lastEnv) {
		env = e.attackSL.next(env, e.lastEnv);
	}
	else {
		env = e.releaseSL.next(env, e.lastEnv);
	}
	e.lastEnv = env;

	float detectorDb = amplitudeToDecibels(env / 5.0f);
	float compressionDb = 0.0f;
	if (_compressorMode) {
		compressionDb = e.compressor.compressionDb(detectorDb, e.thresholdDb, e.ratio, _softKnee);
	}
	else {
		compressionDb = e.noiseGate.compressionDb(detectorDb, e.thresholdDb, e.ratio, _softKnee);
	}
	e.amplifier.setLevel(-compressionDb);
	if (c == 0) {
		_compressionDb = compressionDb;
		outputs[ENVELOPE_OUTPUT].setChannels(_channels);
		outputs[LEFT_OUTPUT].setChannels(_channels);
		outputs[RIGHT_OUTPUT].setChannels(_channels);
	}
	outputs[ENVELOPE_OUTPUT].setVoltage(env, c);
	if (outputs[LEFT_OUTPUT].isConnected()) {
		outputs[LEFT_OUTPUT].setVoltage(e.saturator.next(e.amplifier.next(leftInput) * e.outLevel), c);
	}
	if (outputs[RIGHT_OUTPUT].isConnected()) {
		outputs[RIGHT_OUTPUT].setVoltage(e.saturator.next(e.amplifier.next(rightInput) * e.outLevel), c);
	}
}

struct PressorWidget : BGModuleWidget {
	struct CompressionDisplay : LightEmittingWidget<OpaqueWidget> {
		struct Level {
			float db;
			NVGcolor color;
			Level(float db, const NVGcolor& color) : db(db), color(color) {}
		};

		const NVGcolor bgColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
		Pressor* _module;
		std::vector<Level> _levels;

		CompressionDisplay(Pressor* module) : _module(module) {
			auto color = nvgRGBA(0xff, 0xaa, 0x00, 0xff);
			_levels.push_back(Level(30.0f, color));
			for (int i = 1; i <= 15; ++i) {
				float db = 30.0f - i*2.0f;
				_levels.push_back(Level(db, color)); // decibelsToColor(db - 15.0f)));
			}
		}

		bool isLit() override {
			return _module && !_module->isBypassed();
		}

		void draw(const DrawArgs& args) override {
			nvgSave(args.vg);
			for (int i = 0; i < 80; i += 5) {
				drawBox(args, i);
				nvgFillColor(args.vg, bgColor);
				nvgFill(args.vg);
			}
			nvgRestore(args.vg);
		}

		void drawLit(const DrawArgs& args) override {
			float compressionDb = 0.0f;
			if (_module && !_module->isBypassed()) {
				compressionDb = _module->_compressionDb;
			}

			nvgSave(args.vg);
			for (int i = 0; i < 80; i += 5) {
				const Level& l = _levels.at(i / 5);
				if (compressionDb > l.db) {
					drawBox(args, i);
					nvgFillColor(args.vg, l.color);
					nvgFill(args.vg);
				}
			}
			nvgRestore(args.vg);
		}

		void drawBox(const DrawArgs& args, int offset) {
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 3, offset + 1, 5, 4);
		}
	};

	static constexpr int hp = 15;

	PressorWidget(Pressor* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Pressor");
		createScrews();

		{
			auto display = new CompressionDisplay(module);
			display->box.pos = Vec(208.0, 30.0 - 0.5);
			display->box.size = Vec(18, 180);
			addChild(display);
		}

		// generated by svg_widgets.rb
		auto thresholdParamPosition = Vec(36.0, 53.0);
		auto ratioParamPosition = Vec(125.0, 53.0);
		auto attackParamPosition = Vec(42.0, 141.0);
		auto releaseParamPosition = Vec(131.0, 141.0);
		auto inputGainParamPosition = Vec(28.0, 213.0);
		auto outputGainParamPosition = Vec(89.0, 213.0);
		auto detectorMixParamPosition = Vec(150.0, 213.0);
		auto modeParamPosition = Vec(198.5, 129.5);
		auto dectectorModeParamPosition = Vec(198.5, 178.5);
		auto kneeParamPosition = Vec(198.5, 227.5);

		auto leftInputPosition = Vec(16.0, 274.0);
		auto sidechainInputPosition = Vec(50.0, 274.0);
		auto thresholdInputPosition = Vec(84.0, 274.0);
		auto ratioInputPosition = Vec(118.0, 274.0);
		auto rightInputPosition = Vec(16.0, 318.0);
		auto attackInputPosition = Vec(50.0, 318.0);
		auto releaseInputPosition = Vec(84.0, 318.0);
		auto inputGainInputPosition = Vec(118.0, 318.0);
		auto outputGainInputPosition = Vec(152.0, 318.0);

		auto envelopeOutputPosition = Vec(152.0, 274.0);
		auto leftOutputPosition = Vec(186.0, 274.0);
		auto rightOutputPosition = Vec(186.0, 318.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(thresholdParamPosition, module, Pressor::THRESHOLD_PARAM));
		addParam(createParam<Knob38>(ratioParamPosition, module, Pressor::RATIO_PARAM));
		addParam(createParam<Knob26>(attackParamPosition, module, Pressor::ATTACK_PARAM));
		addParam(createParam<Knob26>(releaseParamPosition, module, Pressor::RELEASE_PARAM));
		addParam(createParam<Knob26>(outputGainParamPosition, module, Pressor::OUTPUT_GAIN_PARAM));
		addParam(createParam<Knob26>(inputGainParamPosition, module, Pressor::INPUT_GAIN_PARAM));
		addParam(createParam<Knob26>(detectorMixParamPosition, module, Pressor::DETECTOR_MIX_PARAM));
		addParam(createParam<SliderSwitch2State14>(modeParamPosition, module, Pressor::MODE_PARAM));
		addParam(createParam<SliderSwitch2State14>(dectectorModeParamPosition, module, Pressor::DECTECTOR_MODE_PARAM));
		addParam(createParam<SliderSwitch2State14>(kneeParamPosition, module, Pressor::KNEE_PARAM));

		addInput(createInput<Port24>(leftInputPosition, module, Pressor::LEFT_INPUT));
		addInput(createInput<Port24>(sidechainInputPosition, module, Pressor::SIDECHAIN_INPUT));
		addInput(createInput<Port24>(thresholdInputPosition, module, Pressor::THRESHOLD_INPUT));
		addInput(createInput<Port24>(ratioInputPosition, module, Pressor::RATIO_INPUT));
		addInput(createInput<Port24>(rightInputPosition, module, Pressor::RIGHT_INPUT));
		addInput(createInput<Port24>(attackInputPosition, module, Pressor::ATTACK_INPUT));
		addInput(createInput<Port24>(releaseInputPosition, module, Pressor::RELEASE_INPUT));
		addInput(createInput<Port24>(inputGainInputPosition, module, Pressor::INPUT_GAIN_INPUT));
		addInput(createInput<Port24>(outputGainInputPosition, module, Pressor::OUTPUT_GAIN_INPUT));

		addOutput(createOutput<Port24>(envelopeOutputPosition, module, Pressor::ENVELOPE_OUTPUT));
		addOutput(createOutput<Port24>(leftOutputPosition, module, Pressor::LEFT_OUTPUT));
		addOutput(createOutput<Port24>(rightOutputPosition, module, Pressor::RIGHT_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<Pressor*>(module);
		assert(m);

		OptionsMenuItem* tr = new OptionsMenuItem("Threshold range");
		tr->addItem(OptionMenuItem("1x (-24dB to 6dB)", [m]() { return m->_thresholdRange == 1.0f; }, [m]() { m->_thresholdRange = 1.0f; }));
		tr->addItem(OptionMenuItem("2x (-48dB to 12dB)", [m]() { return m->_thresholdRange == 2.0f; }, [m]() { m->_thresholdRange = 2.0f; }));
		OptionsMenuItem::addToMenu(tr, menu);
	}
};

Model* modelPressor = bogaudio::createModel<Pressor, PressorWidget>("Bogaudio-Pressor", "PRESSOR", "Stereo compressor and noise gate", "Compressor", "Dynamics", "Polyphonic");
