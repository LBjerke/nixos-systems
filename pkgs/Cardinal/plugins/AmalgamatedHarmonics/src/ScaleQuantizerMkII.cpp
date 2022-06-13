#include "AH.hpp"
#include "AHCommon.hpp"

#include <iostream>

using namespace ah;

struct ScaleQuantizer2 : core::AHModule {

	enum ParamIds {
		KEY_PARAM,
		SCALE_PARAM,
		ENUMS(SHIFT_PARAM,8),
		TRANS_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(IN_INPUT,8),
		KEY_INPUT,
		SCALE_INPUT,
		TRANS_INPUT,
		ENUMS(HOLD_INPUT,8),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUT_OUTPUT,8),
		ENUMS(TRIG_OUTPUT,8),
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(KEY_LIGHT,12),
		ENUMS(SCALE_LIGHT,12),
		NUM_LIGHTS
	};

	ScaleQuantizer2() : core::AHModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(KEY_PARAM, 0.0f, 11.0f, 0.0f, "Key"); // 12 notes

		configParam(SCALE_PARAM, 0.0f, 11.0f, 0.0f, "Scale"); // 12 scales

		configParam(TRANS_PARAM, -11.0f, 11.0f, 0.0f, "Global transposition", " semitones"); // 12 notes
		paramQuantities[KEY_PARAM]->description = "Transposition of all outputs post-quantisation";

		for (int i = 0; i < 8; i++) {
			configParam(SHIFT_PARAM + i, -3.0f, 3.0f, 0.0f, "Octave shift", " octaves");
		}

	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// voltscale
		json_t *scaleModeJ = json_integer((int) voltScale);
		json_object_set_new(rootJ, "voltscale", scaleModeJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// voltscale
		json_t *scaleModeJ = json_object_get(rootJ, "voltscale");
		if (scaleModeJ) voltScale = (music::RootScaling)json_integer_value(scaleModeJ);
	}

	void process(const ProcessArgs &args) override;

	music::RootScaling voltScale = music::RootScaling::CIRCLE;

	bool firstStep = true;
	int lastScale = 0;
	int lastRoot = 0;
	float lastTrans = -10000.0f;

	dsp::SchmittTrigger holdTrigger[8][16];
	dsp::PulseGenerator triggerPulse[8][16];

	float holdPitch[8][16] = {};
	float lastPitch[8][16] = {};

	bool holdState[8][16];

	int currScale = 0;
	int currRoot = 0;

};

void ScaleQuantizer2::process(const ProcessArgs &args) {

	AHModule::step();

	lastScale = currScale;
	lastRoot = currRoot;

	if (inputs[KEY_INPUT].isConnected()) {
		float v = inputs[KEY_INPUT].getVoltage();
		if (voltScale == music::RootScaling::CIRCLE) {
			currRoot = music::getKeyFromVolts(v);
		} else {
			int intv;
			music::getPitchFromVolts(v, music::Notes::NOTE_C, music::Scales::SCALE_CHROMATIC, &currRoot, &intv);
		}
	} else {
		currRoot = params[KEY_PARAM].getValue();
	}

	if (inputs[SCALE_INPUT].isConnected()) {
		currScale = music::getScaleFromVolts(inputs[SCALE_INPUT].getVoltage());
	} else {
		currScale = params[SCALE_PARAM].getValue();
	}

	float trans = (inputs[TRANS_INPUT].getVoltage() + params[TRANS_PARAM].getValue()) / 12.0;
	if (trans != 0.0) {
		if (trans != lastTrans) {
			trans = music::getPitchFromVolts(trans, music::Notes::NOTE_C, music::Scales::SCALE_CHROMATIC);
			lastTrans = trans;
		} else {
			trans = lastTrans;
		}
	}

	for (int i = 0; i < 8; i++) {
		float shift			= params[SHIFT_PARAM + i].getValue();
		int nCVChannels		= inputs[IN_INPUT + i].getChannels();
		int nHoldChannels	= inputs[HOLD_INPUT + i].getChannels();
		int nChannels		= std::max(nCVChannels,nHoldChannels);

		outputs[OUT_OUTPUT + i].setChannels(nChannels);
		outputs[TRIG_OUTPUT + i].setChannels(nChannels);

		for (int j = 0; j < nChannels; j++) {

			holdState[i][j] = holdTrigger[i][j].process(inputs[HOLD_INPUT + i].getVoltage(j));

			if (nHoldChannels == 0) {
				holdPitch[i][j] = music::getPitchFromVolts(inputs[IN_INPUT + i].getVoltage(j), currRoot, currScale);
			} else if (nHoldChannels == 1) {
				if (holdState[i][0]) { // Use channel 0 for hold
					holdPitch[i][j] = music::getPitchFromVolts(inputs[IN_INPUT + i].getVoltage(j), currRoot, currScale);
				}
			} else {
				if (nCVChannels == 1) {
					if (holdState[i][j]) {
						holdPitch[i][j] = music::getPitchFromVolts(inputs[IN_INPUT + i].getVoltage(0), currRoot, currScale); // (re)-sample channel 0
					}
				} else {
					if (holdState[i][j]) {
						holdPitch[i][j] = music::getPitchFromVolts(inputs[IN_INPUT + i].getVoltage(j), currRoot, currScale);
					}
				}
			}

			// If the quantised pitch has changed
			if (lastPitch[i][j] != holdPitch[i][j]) {

				// Record the pitch
				lastPitch[i][j] = holdPitch[i][j];

				// Pulse the gate
				triggerPulse[i][j].trigger(digital::TRIGGER);
			} 

			outputs[OUT_OUTPUT + i].setVoltage(holdPitch[i][j] + shift + trans, j);

			if (triggerPulse[i][j].process(args.sampleTime)) {
				outputs[TRIG_OUTPUT + i].setVoltage(10.0f, j);
			} else {
				outputs[TRIG_OUTPUT + i].setVoltage(0.0f, j);
			}

		}

	}

	if (lastScale != currScale || firstStep) {
		for (int i = 0; i < music::Notes::NUM_NOTES; i++) {
			lights[SCALE_LIGHT + i].setBrightness(0.0f);
		}
		lights[SCALE_LIGHT + currScale].setBrightness(10.0f);
	} 

	if (lastRoot != currRoot || firstStep) {
		for (int i = 0; i < music::Notes::NUM_NOTES; i++) {
			lights[KEY_LIGHT + i].setBrightness(0.0f);
		}
		lights[KEY_LIGHT + currRoot].setBrightness(10.0f);
	} 

	firstStep = false;

}

struct ScaleQuantizer2Widget : ModuleWidget {

	std::vector<MenuOption<music::RootScaling>> scalingOptions;

	ScaleQuantizer2Widget(ScaleQuantizer2 *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ScaleQuantizerMkII.svg")));

		addParam(createParamCentered<gui::AHKnobSnap>(Vec(16.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 0));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(50.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 1));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(84.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 2));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(118.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 3));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(152.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 4));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(186.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 5));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(220.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 6));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(254.308, 107.756), module, ScaleQuantizer2::SHIFT_PARAM + 7));

		addParam(createParamCentered<gui::AHKnobSnap>(Vec(77.041, 340.23), module, ScaleQuantizer2::KEY_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(172.041, 340.23), module, ScaleQuantizer2::SCALE_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(267.041, 340.23), module, ScaleQuantizer2::TRANS_PARAM));

		addInput(createInputCentered<gui::AHPort>(Vec(16.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 0));
		addInput(createInputCentered<gui::AHPort>(Vec(50.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 1));
		addInput(createInputCentered<gui::AHPort>(Vec(84.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 2));
		addInput(createInputCentered<gui::AHPort>(Vec(118.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 3));
		addInput(createInputCentered<gui::AHPort>(Vec(152.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 4));
		addInput(createInputCentered<gui::AHPort>(Vec(186.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 5));
		addInput(createInputCentered<gui::AHPort>(Vec(220.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 6));
		addInput(createInputCentered<gui::AHPort>(Vec(254.308, 41.04), module, ScaleQuantizer2::IN_INPUT + 7));

		addInput(createInputCentered<gui::AHPort>(Vec(16.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 0));
		addInput(createInputCentered<gui::AHPort>(Vec(50.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 1));
		addInput(createInputCentered<gui::AHPort>(Vec(84.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 2));
		addInput(createInputCentered<gui::AHPort>(Vec(118.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 3));
		addInput(createInputCentered<gui::AHPort>(Vec(152.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 4));
		addInput(createInputCentered<gui::AHPort>(Vec(186.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 5));
		addInput(createInputCentered<gui::AHPort>(Vec(220.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 6));
		addInput(createInputCentered<gui::AHPort>(Vec(254.308, 74.436), module, ScaleQuantizer2::HOLD_INPUT + 7));

		addInput(createInputCentered<gui::AHPort>(Vec(35.635, 339.336), module, ScaleQuantizer2::KEY_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(130.635, 339.336), module, ScaleQuantizer2::SCALE_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(225.635, 339.336), module, ScaleQuantizer2::TRANS_INPUT));

		addOutput(createOutputCentered<gui::AHPort>(Vec(16.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 0));
		addOutput(createOutputCentered<gui::AHPort>(Vec(50.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 1));
		addOutput(createOutputCentered<gui::AHPort>(Vec(84.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 2));
		addOutput(createOutputCentered<gui::AHPort>(Vec(118.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 3));
		addOutput(createOutputCentered<gui::AHPort>(Vec(152.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 4));
		addOutput(createOutputCentered<gui::AHPort>(Vec(186.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 5));
		addOutput(createOutputCentered<gui::AHPort>(Vec(220.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 6));
		addOutput(createOutputCentered<gui::AHPort>(Vec(254.308, 141.076), module, ScaleQuantizer2::OUT_OUTPUT + 7));

		addOutput(createOutputCentered<gui::AHPort>(Vec(16.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 0));
		addOutput(createOutputCentered<gui::AHPort>(Vec(50.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 1));
		addOutput(createOutputCentered<gui::AHPort>(Vec(84.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 2));
		addOutput(createOutputCentered<gui::AHPort>(Vec(118.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 3));
		addOutput(createOutputCentered<gui::AHPort>(Vec(152.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 4));
		addOutput(createOutputCentered<gui::AHPort>(Vec(186.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 5));
		addOutput(createOutputCentered<gui::AHPort>(Vec(220.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 6));
		addOutput(createOutputCentered<gui::AHPort>(Vec(254.308, 174.471), module, ScaleQuantizer2::TRIG_OUTPUT + 7));

		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(21.616, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 0));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(32.024, 214.645), module, ScaleQuantizer2::KEY_LIGHT + 1));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(42.432, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 2));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(52.841, 214.645), module, ScaleQuantizer2::KEY_LIGHT + 3));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(63.249, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 4));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(84.066, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 5));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(94.475, 214.645), module, ScaleQuantizer2::KEY_LIGHT + 6));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(104.883, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 7));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(115.291, 214.645), module, ScaleQuantizer2::KEY_LIGHT + 8));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(125.7, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 9));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(136.108, 214.645), module, ScaleQuantizer2::KEY_LIGHT + 10));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(146.517, 232.673), module, ScaleQuantizer2::KEY_LIGHT + 11));

		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(29.355, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 0));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.834, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 1));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(62.313, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 2));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(78.791, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 3));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(95.27, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 4));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(111.749, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 5));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(128.228, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 6));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(144.706, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 7));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(161.185, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 8));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(177.664, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 9));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(194.143, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 10));
		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(210.621, 282.658), module, ScaleQuantizer2::SCALE_LIGHT + 11));

		scalingOptions.emplace_back(std::string("V/Oct"), music::RootScaling::VOCT);
		scalingOptions.emplace_back(std::string("Fourths and Fifths"), music::RootScaling::CIRCLE);

	}

	void appendContextMenu(Menu *menu) override {

		ScaleQuantizer2 *squant = dynamic_cast<ScaleQuantizer2*>(module);
		assert(squant);

		struct ScaleQuantizer2Menu : MenuItem {
			ScaleQuantizer2 *module;
			ScaleQuantizer2Widget *parent;
		};

		struct ScalingItem : ScaleQuantizer2Menu {
			music::RootScaling voltScale;
			void onAction(const rack::event::Action &e) override {
				module->voltScale = voltScale;
			}
		};

		struct ScalingMenu : ScaleQuantizer2Menu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->scalingOptions) {
					ScalingItem *item = createMenuItem<ScalingItem>(opt.name, CHECKMARK(module->voltScale == opt.value));
					item->module = module;
					item->voltScale = opt.value;
					menu->addChild(item);
				}
				return menu;
			}
		};

		menu->addChild(construct<MenuLabel>());
		ScalingMenu *item = createMenuItem<ScalingMenu>("Root Volt Scaling");
		item->module = squant;
		item->parent = this;
		menu->addChild(item);

	}


};

Model *modelScaleQuantizer2 = createModel<ScaleQuantizer2, ScaleQuantizer2Widget>("ScaleQuantizer2");
