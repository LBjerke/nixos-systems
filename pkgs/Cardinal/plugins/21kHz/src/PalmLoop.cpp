#include "21kHz.hpp"
#include "dsp/math.hpp"
#include <array>

using std::array;

struct PalmLoop : Module {
	enum ParamIds {
		OCT_PARAM,
		COARSE_PARAM,
		FINE_PARAM,
		EXP_FM_PARAM,
		LIN_FM_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		RESET_INPUT,
		V_OCT_INPUT,
		EXP_FM_INPUT,
		LIN_FM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SAW_OUTPUT,
		SQR_OUTPUT,
		TRI_OUTPUT,
		SIN_OUTPUT,
		SUB_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float phase = 0.0f;
	float oldPhase = 0.0f;
	float square = 1.0f;
	int discont = 0;
	int oldDiscont = 0;

	array<float, 4> sawBuffer;
	array<float, 4> sqrBuffer;
	array<float, 4> triBuffer;

	float log2sampleFreq = 15.4284f;

	dsp::SchmittTrigger resetTrigger;

	PalmLoop() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OCT_PARAM, 4, 12, 8, "Octave", "", 0.f, 1.f, -4.f);
		configParam(COARSE_PARAM, -7, 7, 0, "Coarse Offset", " step", 0.f, 0.5f, 0.f);
		configParam(FINE_PARAM, -0.083333, 0.083333, 0.0, "Fine Offset", " step", 0.f, 6.f, 0.f);
		configParam(EXP_FM_PARAM, -1.0, 1.0, 0.0, "Exp. FM");
		configParam(LIN_FM_PARAM, -11.7, 11.7, 0.0, "Lin. FM");
		configInput(EXP_FM_INPUT, "Exponential FM");
		configInput(V_OCT_INPUT, "Master Pitch");
		configInput(LIN_FM_INPUT, "Linear FM");
		configInput(RESET_INPUT, "Reset");
		configOutput(SAW_OUTPUT, "Sawtooth Wave");
		configOutput(SIN_OUTPUT, "Sin Wave");
		configOutput(SQR_OUTPUT, "Square Wave");
		configOutput(TRI_OUTPUT, "Triangle Wave");
		configOutput(SUB_OUTPUT, "Sub Sine Wave");
	}
	void process(const ProcessArgs &args) override;
	void onSampleRateChange() override;

};


void PalmLoop::onSampleRateChange() {
	log2sampleFreq = log2f(1.0f / APP->engine->getSampleTime()) - 0.00009f;
}


// quick explanation: the whole thing is driven by a naive sawtooth, which writes to a four-sample buffer for each
// (non-sine) waveform. the waves are calculated such that their discontinuities (or in the case of triangle, derivative
// discontinuities) only occur each time the phasor exceeds a [0, 1) range. when we calculate the outputs, we look to see
// if a discontinuity occurred in the previous sample. if one did, we calculate the polyblep or polyblamp and add it to
// each sample in the buffer. the output is the oldest buffer sample, which gets overwritten in the following step.

void PalmLoop::process(const ProcessArgs &args) {
	if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
		phase = 0.0f;
	}

	for (int i = 0; i <= 2; ++i) {
		sawBuffer[i] = sawBuffer[i + 1];
		sqrBuffer[i] = sqrBuffer[i + 1];
		triBuffer[i] = triBuffer[i + 1];
	}

	float freq = params[OCT_PARAM].getValue() + 0.031360 + 0.083333 * params[COARSE_PARAM].getValue() + params[FINE_PARAM].getValue() + inputs[V_OCT_INPUT].getVoltage();
	freq += params[EXP_FM_PARAM].getValue() * inputs[EXP_FM_INPUT].getVoltage();
	if (freq >= log2sampleFreq) {
		freq = log2sampleFreq;
	}
	freq = powf(2.0f, freq);
	float incr = 0.0f;
	if (inputs[LIN_FM_INPUT].isConnected()) {
		freq += params[LIN_FM_PARAM].getValue() * params[LIN_FM_PARAM].getValue() * params[LIN_FM_PARAM].getValue() * inputs[LIN_FM_INPUT].getVoltage();
		incr = args.sampleTime * freq;
		if (incr > 1.0f) {
			incr = 1.0f;
		}
		else if (incr < -1.0f) {
			incr = -1.0f;
		}
	}
	else {
		incr = args.sampleTime * freq;
	}

	phase += incr;
	if (phase >= 0.0f && phase < 1.0f) {
		discont = 0;
	}
	else if (phase >= 1.0f) {
		discont = 1;
		--phase;
		square *= -1.0f;
	}
	else {
		discont = -1;
		++phase;
		square *= -1.0f;
	}

	sawBuffer[3] = phase;
	sqrBuffer[3] = square;
	if (square >= 0.0f) {
		triBuffer[3] = phase;
	}
	else {
		triBuffer[3] = 1.0f - phase;
	}

	if (outputs[SAW_OUTPUT].isConnected()) {
		if (oldDiscont == 1) {
			polyblep4(sawBuffer, 1.0f - oldPhase / incr, 1.0f);
		}
		else if (oldDiscont == -1) {
			polyblep4(sawBuffer, 1.0f - (oldPhase - 1.0f) / incr, -1.0f);
		}
		outputs[SAW_OUTPUT].setVoltage(clamp(10.0f * (sawBuffer[0] - 0.5f), -5.0f, 5.0f));
	}
	if (outputs[SQR_OUTPUT].isConnected()) {
		if (discont == 0) {
			if (oldDiscont == 1) {
				polyblep4(sqrBuffer, 1.0f - oldPhase / incr, -2.0f * square);
			}
			else if (oldDiscont == -1) {
				polyblep4(sqrBuffer, 1.0f - (oldPhase - 1.0f) / incr, -2.0f * square);
			}
		}
		else {
			if (oldDiscont == 1) {
				polyblep4(sqrBuffer, 1.0f - oldPhase / incr, 2.0f * square);
			}
			else if (oldDiscont == -1) {
				polyblep4(sqrBuffer, 1.0f - (oldPhase - 1.0f) / incr, 2.0f * square);
			}
		}
		outputs[SQR_OUTPUT].setVoltage(clamp(4.9999f * sqrBuffer[0], -5.0f, 5.0f));
	}
	if (outputs[TRI_OUTPUT].isConnected()) {
		if (discont == 0) {
			if (oldDiscont == 1) {
				polyblamp4(triBuffer, 1.0f - oldPhase / incr, 2.0f * square * incr);
			}
			else if (oldDiscont == -1) {
				polyblamp4(triBuffer, 1.0f - (oldPhase - 1.0f) / incr, 2.0f * square * incr);
			}
		}
		else {
			if (oldDiscont == 1) {
				polyblamp4(triBuffer, 1.0f - oldPhase / incr, -2.0f * square * incr);
			}
			else if (oldDiscont == -1) {
				polyblamp4(triBuffer, 1.0f - (oldPhase - 1.0f) / incr, -2.0f * square * incr);
			}
		}
		outputs[TRI_OUTPUT].setVoltage(clamp(10.0f * (triBuffer[0] - 0.5f), -5.0f, 5.0f));
	}
	if (outputs[SIN_OUTPUT].isConnected()) {
		outputs[SIN_OUTPUT].setVoltage(5.0f * sin_01(phase));
	}
	if (outputs[SUB_OUTPUT].isConnected()) {
		if (square >= 0.0f) {
			outputs[SUB_OUTPUT].setVoltage(5.0f * sin_01(0.5f * phase));
		}
		else {
			outputs[SUB_OUTPUT].setVoltage(5.0f * sin_01(0.5f * (1.0f - phase)));
		}
	}

	oldPhase = phase;
	oldDiscont = discont;
}


struct PalmLoopWidget : ModuleWidget {
	PalmLoopWidget(PalmLoop *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Panels/PalmLoop.svg")));

		addChild(createWidget<kHzScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<kHzScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<kHzScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<kHzScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<kHzKnobSnap>(Vec(36, 40), module, PalmLoop::OCT_PARAM));

		addParam(createParam<kHzKnobSmallSnap>(Vec(16, 112), module, PalmLoop::COARSE_PARAM));
		addParam(createParam<kHzKnobSmall>(Vec(72, 112), module, PalmLoop::FINE_PARAM));

		addParam(createParam<kHzKnobSmall>(Vec(16, 168), module, PalmLoop::EXP_FM_PARAM));
		addParam(createParam<kHzKnobSmall>(Vec(72, 168), module, PalmLoop::LIN_FM_PARAM));

		addInput(createInput<kHzPort>(Vec(10, 234), module, PalmLoop::EXP_FM_INPUT));
		addInput(createInput<kHzPort>(Vec(47, 234), module, PalmLoop::V_OCT_INPUT));
		addInput(createInput<kHzPort>(Vec(84, 234), module, PalmLoop::LIN_FM_INPUT));

		addInput(createInput<kHzPort>(Vec(10, 276), module, PalmLoop::RESET_INPUT));
		addOutput(createOutput<kHzPort>(Vec(47, 276), module, PalmLoop::SAW_OUTPUT));
		addOutput(createOutput<kHzPort>(Vec(84, 276), module, PalmLoop::SIN_OUTPUT));

		addOutput(createOutput<kHzPort>(Vec(10, 318), module, PalmLoop::SQR_OUTPUT));
		addOutput(createOutput<kHzPort>(Vec(47, 318), module, PalmLoop::TRI_OUTPUT));
		addOutput(createOutput<kHzPort>(Vec(84, 318), module, PalmLoop::SUB_OUTPUT));

	}
};

Model *modelPalmLoop = createModel<PalmLoop, PalmLoopWidget>("kHzPalmLoop");
