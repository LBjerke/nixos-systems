#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelNsgt;

namespace bogaudio {

struct Nsgt : BGModule {
	enum ParamsIds {
		THRESHOLD_PARAM,
		RATIO_PARAM,
		KNEE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEFT_INPUT,
		RIGHT_INPUT,
		THRESHOLD_INPUT,
		RATIO_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		float thresholdDb = 0.0f;
		float ratio = 0.0f;
		float ratioKnob = -1.0f;
		float lastEnv = 0.0f;

		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter releaseSL;
		RootMeanSquare detector;
		NoiseGate noiseGate;
		Amplifier amplifier;
		Saturator saturator;

		void sampleRateChange();
	};

	static constexpr float defaultAttackMs = 150.0f;
	static constexpr float maxAttackMs = 500.0f;
	static constexpr float defaultReleaseMs = 600.0f;
	static constexpr float maxReleaseMs = 2000.0f;

	Engine* _engines[maxChannels] {};
	bool _softKnee = true;
	float _attackMs = defaultAttackMs;
	float _releaseMs = defaultReleaseMs;
	float _thresholdRange = 1.0f;

	struct ThresholdParamQuantity : ParamQuantity {
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};

	Nsgt() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<ThresholdParamQuantity>(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB");
		configParam<DynamicsRatioParamQuantity>(RATIO_PARAM, 0.0f, 1.0f, 0.55159f, "Ratio");
		configSwitch(KNEE_PARAM, 0.0f, 1.0f, 1.0f, "Knee", {"Hard", "Soft"});
		configBypass(LEFT_INPUT, LEFT_OUTPUT);
		configBypass(RIGHT_INPUT, RIGHT_OUTPUT);

		configInput(LEFT_INPUT, "Left signal");
		configInput(RIGHT_INPUT, "Right signal");
		configInput(THRESHOLD_INPUT, "Threshold CV");
		configInput(RATIO_INPUT, "Ratio CV");

		configOutput(LEFT_OUTPUT, "Left signal");
		configOutput(RIGHT_OUTPUT, "Right signal");
	}

	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
