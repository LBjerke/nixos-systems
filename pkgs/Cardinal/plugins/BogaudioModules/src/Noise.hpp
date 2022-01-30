#pragma once

#include "bogaudio.hpp"
#include "dsp/noise.hpp"
#include "poly_channels.hpp"

using namespace bogaudio::dsp;

extern Model* modelNoise;

namespace bogaudio {

struct Noise : PolyChannelsModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		ABS_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		WHITE_OUTPUT,
		PINK_OUTPUT,
		RED_OUTPUT,
		GAUSS_OUTPUT,
		ABS_OUTPUT,
		BLUE_OUTPUT,
		NUM_OUTPUTS
	};

	BlueNoiseGenerator _blue;
	WhiteNoiseGenerator _white;
	PinkNoiseGenerator _pink;
	RedNoiseGenerator _red;
	GaussianNoiseGenerator _gauss;

	Noise() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(ABS_INPUT, "Absolute value");

		configOutput(WHITE_OUTPUT, "White noise");
		configOutput(PINK_OUTPUT, "Pink noise");
		configOutput(RED_OUTPUT, "Red noise");
		configOutput(GAUSS_OUTPUT, "Gaussian noise");
		configOutput(ABS_OUTPUT, "Absolute value");
		configOutput(BLUE_OUTPUT, "Blue noise");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
