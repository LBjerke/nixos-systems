#include "AH.hpp"
#include "AHCommon.hpp"

#include <iostream>
#include <array>

using namespace ah;

struct Quantiser {

	int octave = 0;
	int semitone = 0;
	int cents = 0;

	std::string asString() {
		if (cents == 0) {
			return music::noteNames[semitone] + std::to_string(octave);
		} else if (cents < 0) {
			return music::noteNames[semitone] + std::to_string(octave) + std::to_string(cents);
		} else {
			return music::noteNames[semitone] + std::to_string(octave) + "+" + std::to_string(cents);
		}
	}

	void setNull() {
		octave = 0;
		semitone = 0;
		cents = 0;
	}

	float calculate(float inVolts, bool quantise) {

		float outVolts = 0.0f;

		if (quantise) {

			double octV;
			double octFrac = modf(inVolts, &octV); 
			octave = (int)octV + 4; // 0V is 4th Octave

			// Below 0V. modf returns negative o and frac, so actual octave is one lower
			if (octFrac < 0.0) {
				octave--;
				octFrac = octFrac + 1.0;
			}

			double st;
			double stFrac = modf(octFrac / music::SEMITONE, &st);

			// semitone 0 - 11 
			// stFrac is positive fraction of semitone 0 - 1
			if (stFrac < 0.5) {
				semitone = (int)st;
			} else {
				semitone = (int)st + 1;
				if (semitone == 12) {
					octave++;
					semitone = 0;
				}
			}
			cents = 0;

			outVolts = (octave - 4) + semitone * music::SEMITONE;

		} else {

			double octV;
			double octFrac = modf(inVolts, &octV); 
			octave = (int)octV + 4; // 0V is 4th Octave

			// Below 0V. modf returns negative o and frac, so actual octave is one lower
			if (octFrac < 0.0) {
				octave--;
				octFrac = octFrac + 1.0;
			}

			double st;
			double stFrac = modf(octFrac / music::SEMITONE, &st);

			// semitone 0 - 11 
			// stFrac is positive fraction of semitone 0 - 1
			if (stFrac < 0.5) {
				cents = (int)round(stFrac * 100.0);
				semitone = (int)st;
			} else {
				cents = (int)round((stFrac - 1.0) * 100.0);
				semitone = (int)st + 1;
				if (semitone == 12) {
					octave++;
					semitone = 0;
				}
			}

			outVolts = inVolts;

		}

		return outVolts;

	}

};


struct PolyVolt : core::AHModule {

	enum ParamIds {
		CHAN_PARAM,
		ENUMS(VOLT_PARAM,16),
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	bool quantise = false;
	bool updateUI = false;
	int nChans = 1;
	std::array<Quantiser,16> quantisers;
	std::array<float,16> inVolts;
	std::array<float,16> outVolts;

	PolyVolt() : core::AHModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(CHAN_PARAM, 1.0, 16.0, 16.0, "Output channels");
		for (int i = 0; i < 16; i++) {
			configParam(VOLT_PARAM + i, -10.0, 10.0, 0.0, "Volts");
		}
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// quantise
		json_t *quantiseJ = json_boolean(quantise);
		json_object_set_new(rootJ, "quantise", quantiseJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// quantise
		json_t *quantiseJ = json_object_get(rootJ, "quantise");
		if (quantiseJ) quantise = json_boolean_value(quantiseJ);
	}

	void process(const ProcessArgs &args) override {

		AHModule::step();

		int c = params[CHAN_PARAM].getValue();
		if (c != nChans) {
			nChans = c;
			updateUI = true;
		}

		int i = 0;
		for (; i < nChans; i++) {
			float v = params[VOLT_PARAM + i].getValue();
			if (v != inVolts[i] || updateUI) {
				inVolts[i] = v;
				outVolts[i] = quantisers[i].calculate(v, quantise);
			}
			outputs[POLY_OUTPUT].setVoltage(outVolts[i] , i);
		}
		for (; i < 16; i++) {
			quantisers[i].setNull();
			outVolts[i] = 0.0f;
			outputs[POLY_OUTPUT].setVoltage(0.0f, i);
		}
		outputs[POLY_OUTPUT].setChannels(nChans);
		updateUI = false;
	}
};

struct PolyVoltDisplay : TransparentWidget {

	PolyVolt *module;
    std::string fontPath;
	int refresh = 0;

    PolyVoltDisplay() {
		fontPath = asset::plugin(pluginInstance, "res/RobotoCondensed-Bold.ttf");
    }

	void draw(const DrawArgs &ctx) override {

		std::shared_ptr<Font> font = APP->window->loadFont(fontPath);

		if (font) {		
            nvgGlobalTint(ctx.vg, color::WHITE);
			nvgFontSize(ctx.vg, 16);
			nvgFontFaceId(ctx.vg, font->handle);
			nvgTextLetterSpacing(ctx.vg, -1);

			char text[128];

			int j = 3;
			for (int i = 0; i < 16; i++)  {
				if (i >= module->nChans) {
					nvgFillColor(ctx.vg, nvgRGBA(0x00, 0xFF, 0xFF, 0x6F));
					snprintf(text, sizeof(text), "%02d --", i + 1);
					nvgText(ctx.vg, box.pos.x + 5, box.pos.y + i * 16 + j * 16, text, NULL);		
				} else {
					nvgFillColor(ctx.vg, nvgRGBA(0x00, 0xFF, 0xFF, 0xFF));
					snprintf(text, sizeof(text), "%02d   %f", i + 1, module->inVolts[i]);
					nvgText(ctx.vg, box.pos.x + 5, box.pos.y + i * 16 + j * 16, text, NULL);
					snprintf(text, sizeof(text), "%s", module->quantisers[i].asString().c_str());
					nvgText(ctx.vg, box.pos.x + 110, box.pos.y + i * 16 + j * 16, text, NULL);		
				}
			}
		} 
	}

};


struct PolyVoltWidget : ModuleWidget {

	std::vector<MenuOption<bool>> quantiseOptions;

	PolyVoltWidget(PolyVolt *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PolyVolt.svg")));

		for (int i = 0; i < 8; i++) {
			addParam(createParam<gui::AHKnobNoSnap>(gui::getPosition(gui::PORT, 0, 1 + i, true, true), module, PolyVolt::VOLT_PARAM + i));
		}

		for (int i = 8; i < 16; i++) {
			addParam(createParam<gui::AHKnobNoSnap>(gui::getPosition(gui::PORT, 1, 1 + i - 8, true, true), module, PolyVolt::VOLT_PARAM + i));
		}

		addParam(createParamCentered<gui::AHKnobSnap>(Vec(278.065, 132.653), module, PolyVolt::CHAN_PARAM));

		addOutput(createOutputCentered<gui::AHPort>(Vec(276.38, 315.45), module, PolyVolt::POLY_OUTPUT));

		if (module != NULL) {
			PolyVoltDisplay *displayW = createWidget<PolyVoltDisplay>(Vec(45, 20));
			displayW->box.size = Vec(240, 230);
			displayW->module = module;
			addChild(displayW);
		}

		quantiseOptions.emplace_back(std::string("Quantised"), true);
		quantiseOptions.emplace_back(std::string("Unquantised"), false);

	}

	void appendContextMenu(Menu *menu) override {
		PolyVolt *gen = dynamic_cast<PolyVolt*>(module);
		assert(gen);

		struct PolyVoltMenu : MenuItem {
			PolyVolt *module;
			PolyVoltWidget *parent;
		};

		struct QuantiseItem : PolyVoltMenu {
			bool mode;
			void onAction(const rack::event::Action &e) override {
				module->quantise = mode;
				module->updateUI = true;
			}
		};

		struct QuantiseMenu : PolyVoltMenu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->quantiseOptions) {
					QuantiseItem *item = createMenuItem<QuantiseItem>(opt.name, CHECKMARK(module->quantise == opt.value));
					item->module = module;
					item->mode = opt.value;
					menu->addChild(item);
				}
				return menu;
			}
		};

		menu->addChild(construct<MenuLabel>());

		QuantiseMenu *quantiseItem = createMenuItem<QuantiseMenu>("Quantise");
		quantiseItem->module = gen;
		quantiseItem->parent = this;
		menu->addChild(quantiseItem);

	}

};

Model *modelPolyVolt = createModel<PolyVolt, PolyVoltWidget>("PolyVolt");
