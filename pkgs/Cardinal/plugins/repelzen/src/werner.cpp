#include "repelzen.hpp"

#define NUM_CHANNELS 4

struct Werner : Module {
    enum ParamIds {
	TIME_PARAM,
	DELTA_PARAM,
	NUM_PARAMS
    };
    enum InputIds {
	CV_INPUT,
	NUM_INPUTS = NUM_CHANNELS
    };
    enum OutputIds {
	GATE_OUTPUT,
	NUM_OUTPUTS = NUM_CHANNELS
    };
    enum LightIds {
	NUM_LIGHTS
    };

    Werner() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Werner::TIME_PARAM, 0.0, 1.0, 0.0, "pulse width");
        configParam(Werner::DELTA_PARAM, 0.0, 1.0, 0.0, "sensitivity");
        for (int i = 0; i < NUM_CHANNELS; i++) {
            configInput(CV_INPUT + i, string::f("cv %i", i + 1));
            configOutput(GATE_OUTPUT + i, string::f("gate %i", i + 1));
        }
    }

    void process(const ProcessArgs &args) override;

    dsp::PulseGenerator gatePulse[NUM_CHANNELS];
    float lastValue[NUM_CHANNELS];
    int res = 16;
    float minDelta = 0;
    int frame = 0;
};


void Werner::process(const ProcessArgs &args) {
    //max time is about 100ms at 44kHz
    res = (int)clamp(params[TIME_PARAM].getValue() * 4400.0f, 16.0f, 4400.0f);
    minDelta = params[DELTA_PARAM].getValue() * 2.0f;

    if(++frame > res) {
	for(int i=0; i<NUM_CHANNELS;i++) {
	    float value = inputs[CV_INPUT + i].getVoltage();

	    if(abs(value - lastValue[i]) > minDelta) {
            gatePulse[i].trigger(0.01);
	    }

	    lastValue[i] = value;
	}
	frame = 0;
    }

    for(int i=0; i<NUM_CHANNELS;i++) {
	outputs[GATE_OUTPUT + i].setVoltage(gatePulse[i].process(1.0 / args.sampleRate) ? 10.0 : 0.0);
    }
}

struct WernerWidget : ModuleWidget {
    WernerWidget(Werner *module) {
    	setModule(module);
        box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/reface/retrig_bg.svg")));

        addParam(createParam<ReKnobLGrey>(Vec(9, 40), module, Werner::TIME_PARAM));
        addParam(createParam<ReKnobMGrey>(Vec(15.5, 106), module, Werner::DELTA_PARAM));

        for(int i=0;i<NUM_CHANNELS;i++) {
        	addInput(createInput<ReIOPort>(Vec(3.25, 198.75 + i*42), module, Werner::CV_INPUT + i));
        	addOutput(createOutput<ReIOPort>(Vec(33.25, 198.75 + i*42), module, Werner::GATE_OUTPUT + i));
        }
    }
};

Model *modelWerner = createModel<Werner, WernerWidget>("retrig");
