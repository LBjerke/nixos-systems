#pragma once

#include "bogaudio.hpp"
#include "vco_base.hpp"

extern Model* modelLVCO;

namespace bogaudio {

struct LVCO : VCOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		SLOW_PARAM,
		WAVE_PARAM,
		FM_DEPTH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		FM_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		SINE_LIGHT,
		SAW_LIGHT,
		PULSE_25_LIGHT,
		TRIANGLE_LIGHT,
		SQUARE_LIGHT,
		PULSE_10_LIGHT,
		NUM_LIGHTS
	};

	enum Wave {
		UNINITIALIZED_WAVE,
		SINE_WAVE,
		TRIANGLE_WAVE,
		SAW_WAVE,
		SQUARE_WAVE,
		PULSE_25_WAVE,
		PULSE_10_WAVE
	};

	Wave _wave = UNINITIALIZED_WAVE;
	bool _resetOnWaveChange = false;

	LVCO()
	: VCOBase(
		FREQUENCY_PARAM,
		-1,
		PITCH_INPUT,
		SYNC_INPUT,
		FM_INPUT
	)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<VCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configButton(SLOW_PARAM, "Slow mode");
		configSwitch(WAVE_PARAM, 0.0f, 5.0f, 0.0f, "Waveform", {"Sine", "Triangle", "Saw", "Square", "25% Pulse", "10% Pulse"});
		configParam(FM_DEPTH_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);

		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(FM_INPUT, "Frequency modulation");
		configInput(SYNC_INPUT, "Sync");

		configOutput(OUT_OUTPUT, "Signal");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	void modulateAlways() override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
