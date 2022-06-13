#include "21kHz.hpp"

using simd::float_4;

struct D_Inf : Module {
	enum ParamIds {
		OCTAVE_PARAM,
		COARSE_PARAM,
		HALF_SHARP_PARAM,
		INVERT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INVERT_INPUT,
		TRANSPOSE_INPUT,
		A_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		A_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	bool invert = true;
	bool transpose = true;

	dsp::SchmittTrigger invertTrigger;
	dsp::SchmittTrigger transposeTrigger;

	D_Inf() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OCTAVE_PARAM, -4, 4, 0, "Octave");
		configParam(COARSE_PARAM, -7, 7, 0, "Coarse");
		configParam(HALF_SHARP_PARAM, 0, 1, 0, "Half Sharp");
		configParam(INVERT_PARAM, 0, 1, 0, "Invert");
		configInput(INVERT_INPUT, "Invert Control Voltage");
		configInput(TRANSPOSE_INPUT, "Transpose");
		configInput(A_INPUT, "Main");
		configOutput(A_OUTPUT, "Main");
	}
	void process(const ProcessArgs &args) override;

};


void newState(bool &state, bool inactive, bool triggered) {
	if (inactive) {
		state = true;
	}
	else {
		if (triggered) {
			state = !state;
		}
	}
}


void D_Inf::process(const ProcessArgs &args) {
	if (params[INVERT_PARAM].getValue() == 0) {
		invert = false;
	}
	else {
		newState(invert, !inputs[INVERT_INPUT].isConnected(), invertTrigger.process(inputs[INVERT_INPUT].getVoltage()));
	}
	newState(transpose, !inputs[TRANSPOSE_INPUT].isConnected(), transposeTrigger.process(inputs[TRANSPOSE_INPUT].getVoltage()));
	float xpose_coeff = params[OCTAVE_PARAM].getValue() + 0.083333 * params[COARSE_PARAM].getValue() + 0.041667 * params[HALF_SHARP_PARAM].getValue();

	int channels = inputs[A_INPUT].getChannels();
	for (int c = 0; c < channels; c+=4) {
		float_4 output = inputs[A_INPUT].getPolyVoltageSimd<float_4>(c);
		if (invert) {
			output *= -1.0f;
		}
		if (transpose) {
			output += xpose_coeff;
		}
		outputs[A_OUTPUT].setVoltageSimd(output, c);
	}
	outputs[A_OUTPUT].setChannels(channels);
}


struct D_InfWidget : ModuleWidget {
	D_InfWidget(D_Inf *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/D_Inf.svg")));

		addChild(createWidget<kHzScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<kHzScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<kHzKnobSmallSnap>(Vec(14, 40), module, D_Inf::OCTAVE_PARAM));
		addParam(createParam<kHzKnobSmallSnap>(Vec(14, 96), module, D_Inf::COARSE_PARAM));

		addParam(createParam<kHzButton>(Vec(10, 150), module, D_Inf::HALF_SHARP_PARAM));
		addParam(createParam<kHzButton>(Vec(36, 150), module, D_Inf::INVERT_PARAM));

		addInput(createInput<kHzPort>(Vec(17, 192), module, D_Inf::INVERT_INPUT));
		addInput(createInput<kHzPort>(Vec(17, 234), module, D_Inf::TRANSPOSE_INPUT));
		addInput(createInput<kHzPort>(Vec(17, 276), module, D_Inf::A_INPUT));
		addOutput(createOutput<kHzPort>(Vec(17, 318), module, D_Inf::A_OUTPUT));
	}
};

Model *modelD_Inf = createModel<D_Inf, D_InfWidget>("kHzD_Inf");
