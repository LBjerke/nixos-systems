#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCA;

namespace bogaudio {

struct VCA : BGModule {
	enum ParamsIds {
		LEVEL1_PARAM,
		LEVEL2_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	Amplifier _amplifier1[maxChannels];
	bogaudio::dsp::SlewLimiter _levelSL1[maxChannels];
	Amplifier _amplifier2[maxChannels];
	bogaudio::dsp::SlewLimiter _levelSL2[maxChannels];

	struct LevelParamQuantity : AmplifierParamQuantity {
		bool isLinear() override;
	};

	VCA() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<LevelParamQuantity>(LEVEL1_PARAM, 0.0f, 1.0f, 0.8f, "Level 1");
		configParam<LevelParamQuantity>(LEVEL2_PARAM, 0.0f, 1.0f, 0.8f, "Level 2");
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Disabled", "Enabled"});
		configBypass(IN1_INPUT, OUT1_OUTPUT);
		configBypass(IN2_INPUT, OUT2_OUTPUT);

		configInput(CV1_INPUT, "Level 1 CV");
		configInput(IN1_INPUT, "Signal 1");
		configInput(CV2_INPUT, "Level 2 CV");
		configInput(IN2_INPUT, "Signal 2");

		configOutput(OUT1_OUTPUT, "Signal 1");
		configOutput(OUT2_OUTPUT, "Signal 2");

		sampleRateChange();
	}

	inline bool isLinear() { return params[LINEAR_PARAM].getValue() > 0.5f; }
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
	void channelStep(Input& input, Output& output, Param& knob, Input& cv, Amplifier* amplifier, bogaudio::dsp::SlewLimiter* levelSL, bool linear);
};

} // namespace bogaudio
