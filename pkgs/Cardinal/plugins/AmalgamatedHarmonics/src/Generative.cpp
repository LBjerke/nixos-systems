#include "dsp/noise.hpp"

#include "AH.hpp"
#include "AHCommon.hpp"
#include "VCO.hpp"

using namespace ah;

struct Generative : core::AHModule {

	enum ParamIds {
		FREQ_PARAM,
		WAVE_PARAM,
		FM_PARAM,
		AM_PARAM,
		NOISE_PARAM,
		CLOCK_PARAM,
		PROB_PARAM,
		DELAYL_PARAM,
		DELAYS_PARAM,
		GATEL_PARAM,
		GATES_PARAM,
		SLOPE_PARAM,
		SPEED_PARAM,
		ATTN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		WAVE_INPUT,
		FM_INPUT,
		AM_INPUT,
		NOISE_INPUT,
		SAMPLE_INPUT,
		CLOCK_INPUT,
		PROB_INPUT,
		HOLD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LFO_OUTPUT,
		MIXED_OUTPUT,
		NOISE_OUTPUT,
		OUT_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(GATE_LIGHT,2),
		NUM_LIGHTS
	};

	Generative() : core::AHModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) { 

		// LFO section
		configParam(FREQ_PARAM, -8.0f, 10.0f, 1.0f, "Frequency");

		struct WaveParamQuantity : engine::ParamQuantity {
			std::string getDisplayValueString() override {
				float v = getValue();
				float r = math::eucMod(v, 1.0);
				int f = (int)std::floor(v);

				std::string dial;
				std::string left;
				std::string right;

				if (math::chop(v) == 0.0 || v == 4.0f) {
					return "Sine " + ParamQuantity::getDisplayValueString();
				} else if (f == 1 && math::chop(r) == 0.0) {
					return "Triangle " + ParamQuantity::getDisplayValueString();
				} else if (f == 2 && math::chop(r) == 0.0) {
					return "Saw " + ParamQuantity::getDisplayValueString();
				} else if (f == 3 && math::chop(r) == 0.0) {
					return "Square " + ParamQuantity::getDisplayValueString();
				}

				if (r < 0.3333f) {
					dial = "|..";	
				} else if (r > 0.6666f) {
					dial = "..|";
				} else {
					dial = ".|.";
				}

				switch(f) {
					case 0:
						left = "Sine";
						right = "Triangle ";
						break;
					case 1:
						left = "Triangle";
						right = "Saw ";
						break;
					case 2:
						left = "Saw";
						right = "Square ";
						break;
					case 3:
						left = "Square";
						right = "Sine ";
						break;
				}

				return left + dial + right + ParamQuantity::getDisplayValueString();

			}
		};

		configParam<WaveParamQuantity>(WAVE_PARAM, 0.0f, 4.0f, 1.5f, "Waveform");
		paramQuantities[WAVE_PARAM]->description = "Continuous: Sine - Triangle - Saw - Square - Sine";

		configParam(FM_PARAM, 0.0f, 1.0f, 0.5f, "Frequency Modulation CV");

		configParam(AM_PARAM, 0.0f, 1.0f, 0.5f, "Amplitude Modulation Mix");
		paramQuantities[AM_PARAM]->description = "Mix between the FM modulated LFO and the voltage supplied in AM input";

		configParam(NOISE_PARAM, 0.0f, 1.0f, 0.5f, "Noise Mix");
		paramQuantities[NOISE_PARAM]->description = "Mix between the FM-AM modulated LFO and the internal noise source";

		configParam(CLOCK_PARAM, -2.0, 6.0, 1.0, "Clock tempo", " bpm", 2.f, 60.f);

		configParam(PROB_PARAM, 0.0, 1.0, 1.0, "Clock-tick probability", "%", 0.0f, 100.0f);

		configParam(DELAYL_PARAM, 1.0f, 2.0f, 1.0f, "Delay length", "ms", 2.0f, 500.0f, -1000.0f);

		configParam(GATEL_PARAM, 1.0f, 2.0f, 1.0f, "Gate length", "ms", 2.0f, 500.0f, -1000.0f);

		configParam(DELAYS_PARAM, 1.0f, 2.0f, 1.0f, "Delay length spread", "ms", 2.0f, 500.0f, -1000.0f);
		paramQuantities[DELAYS_PARAM]->description = "Magnitude of random time applied to delay length";

		configParam(GATES_PARAM, 1.0f, 2.0f, 1.0f, "Gate length spread", "ms", 2.0f, 500.0f, -1000.0f);
		paramQuantities[GATES_PARAM]->description = "Magnitude of random time applied to gate length";

		configParam(SLOPE_PARAM, 0.0, 1.0, 0.0, "Slope");
		paramQuantities[SLOPE_PARAM]->description = "Linear to exponential slope";

		configParam(SPEED_PARAM, 0.0, 1.0, 0.0, "Inertia", "%", 0.0f, 100.0f);
		paramQuantities[SPEED_PARAM]->description = "Resistance of the signal to change";

		configParam(ATTN_PARAM, 0.0, 1.0, 1.0, "Level", "%", 0.0f, 100.0f);

	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// quantise
		json_t *quantiseJ = json_boolean(quantise);
		json_object_set_new(rootJ, "quantise", quantiseJ);

		// offset
		json_t *offsetJ = json_boolean(offset);
		json_object_set_new(rootJ, "offset", offsetJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// quantise
		json_t *quantiseJ = json_object_get(rootJ, "quantise");
		if (quantiseJ) quantise = json_boolean_value(quantiseJ);

		// offset
		json_t *offsetJ = json_object_get(rootJ, "offset");
		if (offsetJ) offset = json_boolean_value(offsetJ);
	}

	rack::dsp::SchmittTrigger sampleTrigger;
	rack::dsp::SchmittTrigger holdTrigger;
	rack::dsp::SchmittTrigger clockTrigger;
	bogaudio::dsp::PinkNoiseGenerator pink;
	LowFrequencyOscillator oscillator;
	LowFrequencyOscillator clock;
	digital::AHPulseGenerator delayPhase;
	digital::AHPulseGenerator gatePhase;

	float target = 0.0f;
	float current = 0.0f;
	bool quantise = false;
	bool offset = false;
	bool delayState = false;
	bool gateState = false;

	// minimum and maximum slopes in volts per second
	const float slewMin = 0.1f;
	const float slewMax = 10000.0f;
	const float slewRatio = slewMin / slewMax;

	// Amount of extra slew per voltage difference
	const float shapeScale = 1.0f / 10.0;

	float delayTime;
	float gateTime;

};

void Generative::process(const ProcessArgs &args) {

	AHModule::step();

	oscillator.setPitch(params[FREQ_PARAM].getValue() + params[FM_PARAM].getValue() * inputs[FM_INPUT].getVoltage());
	oscillator.offset = offset;
	oscillator.step(args.sampleTime);

	clock.setPitch(clamp(params[CLOCK_PARAM].getValue() + inputs[CLOCK_INPUT].getVoltage(), -2.0f, 6.0f));
	clock.step(args.sampleTime);

	float wavem = fabs(fmodf(params[WAVE_PARAM].getValue() + inputs[WAVE_INPUT].getVoltage(), 4.0f));

	float interp = 0.0f;
	bool toss = false;

	if (wavem < 1.0f) {
		interp = crossfade(oscillator.sin(), oscillator.tri(), wavem) * 5.0f; 
	} else if (wavem < 2.0f) {
		interp = crossfade(oscillator.tri(), oscillator.saw(), wavem - 1.0f) * 5.0f;
	} else if (wavem < 3.0f) {
		interp = crossfade(oscillator.saw(), oscillator.sqr(), wavem - 2.0f) * 5.0f;
	} else {
		interp = crossfade(oscillator.sqr(), oscillator.sin(), wavem - 3.0f) * 5.0f;
	}

	// Capture (pink) noise
	float noise = clamp(pink.next() * 7.5f, -5.0f, 5.0f); // -5V to 5V
	float range = params[ATTN_PARAM].getValue();

	// Shift the noise floor
	if (offset) {
		noise += 5.0f;
	}

	float noiseLevel = clamp(params[NOISE_PARAM].getValue() + inputs[NOISE_INPUT].getVoltage(), 0.0f, 1.0f);

	// Mixed the input AM signal or noise
	if (inputs[AM_INPUT].isConnected()) {
		interp = crossfade(interp, inputs[AM_INPUT].getVoltage(), params[AM_PARAM].getValue()) * range;
	} else {
		interp *= range;
	}

	// Mix noise
	float mixedSignal = (noise * noiseLevel + interp * (1.0f - noiseLevel));

	// Process gate
	bool sampleActive = inputs[SAMPLE_INPUT].isConnected();
	float sample = inputs[SAMPLE_INPUT].getVoltage();
	bool hold = inputs[HOLD_INPUT].getVoltage() > 0.000001f;

	bool isClocked = false;

	if (!sampleActive) {
		if (clockTrigger.process(clock.sqr())) {
			isClocked = true;
		}
	} else {
		if (sampleTrigger.process(sample)) {
			isClocked = true;
		}
	}

	// If we have no input or we have been a trigger on the sample input
	if (isClocked) {

		// If we are not in a delay or gate state process the tick, otherwise eat it
		if (!delayPhase.ishigh() && !gatePhase.ishigh()) {

			// Check against prob control
			float threshold = clamp(params[PROB_PARAM].getValue() + inputs[PROB_INPUT].getVoltage() / 10.f, 0.0f, 1.0f);
			toss = (random::uniform() < threshold);

			// Tick is valid
			if (toss) {

				// Determine delay time
				float dlyLen = log2(params[DELAYL_PARAM].getValue());
				float dlySpr = log2(params[DELAYS_PARAM].getValue());

				double rndD = clamp(random::normal(), -2.0f, 2.0f);
				delayTime = clamp(dlyLen + dlySpr * rndD, 0.0f, 100.0f);
				
				// Trigger the respective delay pulse generator
				delayState = true;
				delayPhase.trigger(delayTime);
			}
		}
	}

	// In delay state and finished waiting
	if (delayState && !delayPhase.process(args.sampleTime)) {

		// set the target voltage
		target = mixedSignal;

		// Determine gate time
		float gateLen = log2(params[GATEL_PARAM].getValue());
		float gateSpr = log2(params[GATES_PARAM].getValue());

		double rndG = clamp(random::normal(), -2.0f, 2.0f);
		gateTime = clamp(gateLen + gateSpr * rndG, digital::TRIGGER, 100.0f);

		// Open the gate and set flags
		gatePhase.trigger(gateTime);
		gateState = true;
		delayState = false;
	}

	// If not held slew voltages
	if (!hold) {

		// Curve calc
		float shape = params[SLOPE_PARAM].getValue();
		float speed = params[SPEED_PARAM].getValue();	
		float slew = slewMax * powf(slewRatio, speed);

		// Rise
		if (target > current) {
			current += slew * crossfade(1.0f, shapeScale * (target - current), shape) * args.sampleTime;
			if (current > target) // Trap overshoot
				current = target;
		}
		// Fall
		else if (target < current) {
			current -= slew * crossfade(1.0f, shapeScale * (current - target), shape) * args.sampleTime;
			if (current < target) // Trap overshoot
				current = target;
		}
	}

	// If the gate is open, set output to high
	if (gatePhase.process(args.sampleTime)) {
		outputs[GATE_OUTPUT].setVoltage(10.0f);

		lights[GATE_LIGHT].setSmoothBrightness(1.0f, args.sampleTime);
		lights[GATE_LIGHT + 1].setSmoothBrightness(0.0f, args.sampleTime);

	} else {
		outputs[GATE_OUTPUT].setVoltage(0.0f);
		gateState = false;

		if (delayState) {
			lights[GATE_LIGHT].setSmoothBrightness(0.0f, args.sampleTime);
			lights[GATE_LIGHT + 1].setSmoothBrightness(1.0f, args.sampleTime);
		} else {
			lights[GATE_LIGHT].setSmoothBrightness(0.0f, args.sampleTime);
			lights[GATE_LIGHT + 1].setSmoothBrightness(0.0f, args.sampleTime);
		}

	}

	if (quantise) {
		outputs[OUT_OUTPUT].setVoltage(music::getPitchFromVolts(current, music::Notes::NOTE_C, music::Scales::SCALE_CHROMATIC));
	} else {
		outputs[OUT_OUTPUT].setVoltage(current);
	}
	
	outputs[NOISE_OUTPUT].setVoltage(noise * 2.0);
	outputs[LFO_OUTPUT].setVoltage(interp);
	outputs[MIXED_OUTPUT].setVoltage(mixedSignal);

}

struct GenerativeWidget : ModuleWidget {
	
	std::vector<MenuOption<bool>> quantiseOptions;
	std::vector<MenuOption<bool>> offsetOptions;

	GenerativeWidget(Generative *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Generative.svg")));

		addParam(createParamCentered<gui::AHBigKnobNoSnap>(Vec(19.548, 77.759), module, Generative::FREQ_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(85.419, 107.275), module, Generative::FM_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(118.202, 107.275), module, Generative::AM_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(184.69, 107.275), module, Generative::NOISE_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(51.713, 137.056), module, Generative::WAVE_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(51.713, 213.493), module, Generative::CLOCK_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(133.713, 213.493), module, Generative::DELAYL_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(161.713, 213.493), module, Generative::DELAYS_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(86.134, 243.493), module, Generative::PROB_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(133.713, 271.493), module, Generative::GATEL_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(161.713, 271.493), module, Generative::GATES_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(44.308, 338.04), module, Generative::SLOPE_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(81.485, 338.04), module, Generative::SPEED_PARAM));
		addParam(createParamCentered<gui::AHKnobNoSnap>(Vec(155.839, 338.685), module, Generative::ATTN_PARAM));

		addInput(createInputCentered<gui::AHPort>(Vec(85.419, 70.477), module, Generative::FM_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(118.202, 71.401), module, Generative::AM_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(19.548, 137.131), module, Generative::WAVE_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(184.69, 143.504), module, Generative::NOISE_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(19.548, 213.569), module, Generative::CLOCK_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(86.134, 213.569), module, Generative::PROB_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(19.548, 273.131), module, Generative::SAMPLE_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(118.662, 338.116), module, Generative::HOLD_INPUT));

		addOutput(createOutputCentered<gui::AHPort>(Vec(184.69, 71.231), module, Generative::NOISE_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(150.523, 107.18), module, Generative::LFO_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(218.858, 107.18), module, Generative::MIXED_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(218.858, 243.399), module, Generative::GATE_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(193.016, 337.946), module, Generative::OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenRedLight>>(Vec(197.512, 243.493), module, Generative::GATE_LIGHT));

		quantiseOptions.emplace_back(std::string("Quantised"), true);
		quantiseOptions.emplace_back(std::string("Unquantised"), false);

		offsetOptions.emplace_back(std::string("0V - 10V"), true);
		offsetOptions.emplace_back(std::string("-5V to 5V"), false);

	}

	void appendContextMenu(Menu *menu) override {
		Generative *gen = dynamic_cast<Generative*>(module);
		assert(gen);

		struct GenerativeMenu : MenuItem {
			Generative *module;
			GenerativeWidget *parent;
		};

		struct QuantiseItem : GenerativeMenu {
			bool mode;
			void onAction(const rack::event::Action &e) override {
				module->quantise = mode;
			}
		};

		struct QuantiseMenu : GenerativeMenu {
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

		struct OffsetItem : GenerativeMenu {
			bool offset;
			void onAction(const rack::event::Action &e) override {
				module->offset = offset;
			}
		};

		struct OffsetMenu : GenerativeMenu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->offsetOptions) {
					OffsetItem *item = createMenuItem<OffsetItem>(opt.name, CHECKMARK(module->offset == opt.value));
					item->module = module;
					item->offset = opt.value;
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

		OffsetMenu *offsetItem = createMenuItem<OffsetMenu>("CV Offset");
		offsetItem->module = gen;
		offsetItem->parent = this;
		menu->addChild(offsetItem);

	}
};

Model *modelGenerative = createModel<Generative, GenerativeWidget>("Generative");
