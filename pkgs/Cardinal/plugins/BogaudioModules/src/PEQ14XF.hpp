#pragma once

#include "PEQ14_shared.hpp"

namespace bogaudio {

struct PEQ14XF : ExpanderModule<PEQ14ExpanderMessage, ExpandableModule<PEQ14ExpanderMessage, PEQXFBase>> {
	enum ParamsIds {
		DAMP_PARAM,
		GAIN_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		DAMP_INPUT,
		GAIN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		EF1_OUTPUT,
		EF2_OUTPUT,
		EF3_OUTPUT,
		EF4_OUTPUT,
		EF5_OUTPUT,
		EF6_OUTPUT,
		EF7_OUTPUT,
		EF8_OUTPUT,
		EF9_OUTPUT,
		EF10_OUTPUT,
		EF11_OUTPUT,
		EF12_OUTPUT,
		EF13_OUTPUT,
		EF14_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		EnvelopeFollower efs[14];
		float response = -1.0f;
		Amplifier gain;
	};

	Engine* _engines[maxChannels] {};
	Saturator _saturator;
	PEQ14ExpanderMessage* _baseMessage = NULL;

	PEQ14XF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(DAMP_PARAM, 0.0f, 1.0f, 0.3f, "Envelope follower smoothing", "%", 0.0f, 100.0f);
		configParam<EFGainParamQuantity>(GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Envelope follower gain", " dB");

		configInput(DAMP_INPUT, "Damp CV");
		configInput(GAIN_INPUT, "Gain CV");

		configOutput(EF1_OUTPUT, "Envelope 1");
		configOutput(EF2_OUTPUT, "Envelope 2");
		configOutput(EF3_OUTPUT, "Envelope 3");
		configOutput(EF4_OUTPUT, "Envelope 4");
		configOutput(EF5_OUTPUT, "Envelope 5");
		configOutput(EF6_OUTPUT, "Envelope 6");
		configOutput(EF7_OUTPUT, "Envelope 7");
		configOutput(EF8_OUTPUT, "Envelope 8");
		configOutput(EF9_OUTPUT, "Envelope 9");
		configOutput(EF10_OUTPUT, "Envelope 10");
		configOutput(EF11_OUTPUT, "Envelope 11");
		configOutput(EF12_OUTPUT, "Envelope 12");
		configOutput(EF13_OUTPUT, "Envelope 13");
		configOutput(EF14_OUTPUT, "Envelope 14");

		setBaseModelPredicate([](Model* m) { return m == modelPEQ14 || m == modelPEQ14XF || m == modelPEQ14XR  || m == modelPEQ14XV; });
		setExpanderModelPredicate([](Model* m) { return m == modelPEQ14XF || m == modelPEQ14XR || m == modelPEQ14XV; });
	}

	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
