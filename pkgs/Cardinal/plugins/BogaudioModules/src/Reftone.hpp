#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelReftone;

namespace bogaudio {

struct Reftone : BGModule {
	enum ParamsIds {
		PITCH_PARAM,
		OCTAVE_PARAM,
		FINE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		CV_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	int _pitch = 9;
	int _octave = 4;
	float _fine = 0.0f;
	float _frequency = 440.0f;
	float _cv = frequencyToCV(_frequency);
	SineOscillator _sine;

	Reftone() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(PITCH_PARAM, 0.0f, 11.0f, 9.0f, "Pitch");
		paramQuantities[PITCH_PARAM]->snapEnabled = true;
		configParam(OCTAVE_PARAM, 1.0f, 8.0f, 4.0f, "Octave");
		paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
		configParam(FINE_PARAM, -0.99f, 0.99f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);

		configOutput(CV_OUTPUT, "Pitch (1V/octave)");
		configOutput(OUT_OUTPUT, "Sine signal");

		sampleRateChange();
	}

	void sampleRateChange() override {
		_sine.setSampleRate(APP->engine->getSampleRate());
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
