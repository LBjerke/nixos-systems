/***********************************************************************************************
Holt
------
VCV Rack module based on Holt by Chris Johnson from Airwindows <www.airwindows.com>

Ported and designed by Jens Robert Janke 

Changes/Additions:
- mono
- CV inputs for frequency, resonance and poles
- no output control
- no dry/wet
- polyphonic
- added output saturation

See ./LICENSE.md for all licenses
************************************************************************************************/

/* 
    Note: for the sake of porting variety, this one encapsulates the entire audio plugin as its own entity
    Advantages: cleaner module logic, way easier and less messy handling of polyphony
    Drawbacks: possibly sliiightly less speedy
*/

#include "plugin.hpp"

/* Engine (the audio plugin code, single channel)
======================================================================================== */
struct HoltEngine {

    double previousSampleA;
    double previousTrendA;
    double previousSampleB;
    double previousTrendB;
    double previousSampleC;
    double previousTrendC;
    double previousSampleD;
    double previousTrendD;

    double alpha;
    double beta;
    float lastFrequencyParam;
    float lastResonanceParam;

    HoltEngine()
    {
        previousSampleA = 0.0;
        previousTrendA = 0.0;
        previousSampleB = 0.0;
        previousTrendB = 0.0;
        previousSampleC = 0.0;
        previousTrendC = 0.0;
        previousSampleD = 0.0;
        previousTrendD = 0.0;

        alpha = 0.0;
        beta = 0.0;
        lastFrequencyParam = 0.0f;
        lastResonanceParam = 0.0f;
    }

    double process(double inputSample, float frequencyParam = 1.0, float resonanceParam = 0.0, float polesParam = 1.0, float outputParam = 1.0, float drywetParam = 1.0)
    {
        if ((frequencyParam != lastFrequencyParam) || (resonanceParam != lastResonanceParam)) {
            alpha = pow(frequencyParam, 4) + 0.00001;
            if (alpha > 1.0) {
                alpha = 1.0;
            }

            beta = (alpha * pow(resonanceParam, 2)) + 0.00001;
            alpha += ((1.0 - beta) * pow(frequencyParam, 3)); //correct for droop in frequency
            if (alpha > 1.0) {
                alpha = 1.0;
            }

            lastFrequencyParam = frequencyParam;
            lastResonanceParam = resonanceParam;
        }

        double trend;
        double forecast; //defining these here because we're copying the routine four times

        //four-stage wet/dry control using progressive stages that bypass when not engaged
        double aWet = 1.0;
        double bWet = 1.0;
        double cWet = 1.0;
        double dWet = polesParam * 4.0;

        if (dWet < 1.0) {
            aWet = dWet;
            bWet = 0.0;
            cWet = 0.0;
            dWet = 0.0;
        } else if (dWet < 2.0) {
            bWet = dWet - 1.0;
            cWet = 0.0;
            dWet = 0.0;
        } else if (dWet < 3.0) {
            cWet = dWet - 2.0;
            dWet = 0.0;
        } else {
            dWet -= 3.0;
        }
        //this is one way to make a little set of dry/wet stages that are successively added to the
        //output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        //beyond that point: this is a way to progressively add a 'black box' sound processing
        //which lets you fall through to simpler processing at lower settings.

        double gain = outputParam;
        double wet = drywetParam;

        double drySample = inputSample;

        if (aWet > 0.0) {
            trend = (beta * (inputSample - previousSampleA) + ((0.999 - beta) * previousTrendA));
            forecast = previousSampleA + previousTrendA;
            inputSample = (alpha * inputSample) + ((0.999 - alpha) * forecast);
            previousSampleA = inputSample;
            previousTrendA = trend;
            inputSample = (inputSample * aWet) + (drySample * (1.0 - aWet));
        }

        if (bWet > 0.0) {
            trend = (beta * (inputSample - previousSampleB) + ((0.999 - beta) * previousTrendB));
            forecast = previousSampleB + previousTrendB;
            inputSample = (alpha * inputSample) + ((0.999 - alpha) * forecast);
            previousSampleB = inputSample;
            previousTrendB = trend;
            inputSample = (inputSample * bWet) + (previousSampleA * (1.0 - bWet));
        }

        if (cWet > 0.0) {
            trend = (beta * (inputSample - previousSampleC) + ((0.999 - beta) * previousTrendC));
            forecast = previousSampleC + previousTrendC;
            inputSample = (alpha * inputSample) + ((0.999 - alpha) * forecast);
            previousSampleC = inputSample;
            previousTrendC = trend;
            inputSample = (inputSample * cWet) + (previousSampleB * (1.0 - cWet));
        }

        if (dWet > 0.0) {
            trend = (beta * (inputSample - previousSampleD) + ((0.999 - beta) * previousTrendD));
            forecast = previousSampleD + previousTrendD;
            inputSample = (alpha * inputSample) + ((0.999 - alpha) * forecast);
            previousSampleD = inputSample;
            previousTrendD = trend;
            inputSample = (inputSample * dWet) + (previousSampleC * (1.0 - dWet));
        }

        if (gain < 1.0) {
            inputSample *= gain;
        }

        //clip to 1.2533141373155 to reach maximum output
        if (inputSample > 1.2533141373155)
            inputSample = 1.2533141373155;
        if (inputSample < -1.2533141373155)
            inputSample = -1.2533141373155;
        inputSample = sin(inputSample * fabs(inputSample)) / ((inputSample == 0.0) ? 1 : fabs(inputSample));

        if (wet < 1.0) {
            inputSample = (inputSample * wet) + (drySample * (1.0 - wet));
        }

        return inputSample;
    }
};

/* Mojo (for output saturation)
======================================================================================== */
inline double mojo(double in)
{
    double mojo = pow(fabs(in), 0.25);
    if (mojo > 0.0) {
        in = (sin(in * mojo * M_PI * 0.5) / mojo) * 0.987654321;
        in *= 0.65; // dial back a bit to keep levels roughly the same
    }
    return in;
}

/* Module
======================================================================================== */
struct Holt : Module {
    enum ParamIds {
        FREQUENCY_PARAM,
        RESONANCE_PARAM,
        POLES_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        FREQUENCY_CV_INPUT,
        RESONANCE_CV_INPUT,
        POLES_CV_INPUT,
        IN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        OUT_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // module variables
    const double gainCut = 0.03125;
    const double gainBoost = 32.0;
    HoltEngine holt[16];

    // control parameter
    float frequencyParam;
    float resonanceParam;
    float polesParam;

    // other
    double overallscale;
    double fpNShape;

    Holt()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(FREQUENCY_PARAM, 0.f, 1.f, 1.f, "Frequency");
        configParam(RESONANCE_PARAM, 0.f, 1.f, 0.f, "Resonance");
        configParam(POLES_PARAM, 0.f, 1.f, 1.f, "Poles");

        configInput(FREQUENCY_CV_INPUT, "Frequency CV");
        configInput(RESONANCE_CV_INPUT, "Resonance CV");
        configInput(POLES_CV_INPUT, "Poles CV");
        configInput(IN_INPUT, "Signal");
        configOutput(OUT_OUTPUT, "Signal");

        configBypass(IN_INPUT, OUT_OUTPUT);

        onReset();
    }

    void onSampleRateChange() override
    {
        float sampleRate = APP->engine->getSampleRate();

        overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= sampleRate;
    }

    void onReset() override
    {
        for (int i = 0; i < 16; i++) {
            holt[i] = HoltEngine();
        }

        fpNShape = 0.0;

        onSampleRateChange();
        updateParams();
    }

    void updateParams()
    {
        frequencyParam = params[FREQUENCY_PARAM].getValue();
        frequencyParam += inputs[FREQUENCY_CV_INPUT].getVoltage() / 9;
        frequencyParam = clamp(frequencyParam, 0.01f, 0.99f);

        resonanceParam = params[RESONANCE_PARAM].getValue();
        resonanceParam += inputs[RESONANCE_CV_INPUT].getVoltage() / 9;
        resonanceParam = clamp(resonanceParam, 0.01f, 0.99f);

        polesParam = params[POLES_PARAM].getValue();
        polesParam += inputs[POLES_CV_INPUT].getVoltage() / 10;
        polesParam = clamp(polesParam, 0.01f, 0.99f);
    }

    void process(const ProcessArgs& args) override
    {
        updateParams();

        double in;

        // for each poly channel
        for (int i = 0, numChannels = std::max(1, inputs[IN_INPUT].getChannels()); i < numChannels; ++i) {

            // input
            in = inputs[IN_INPUT].getPolyVoltage(i) * gainCut;

            // holt
            in = holt[i].process(in, frequencyParam, resonanceParam, polesParam);

            // mojo for swallowing excessive resonance
            in = mojo(in);

            // output
            outputs[OUT_OUTPUT].setChannels(numChannels);
            outputs[OUT_OUTPUT].setVoltage(in * gainBoost, i);
        }
    }
};

/* Widget
======================================================================================== */
struct HoltWidget : ModuleWidget {
    HoltWidget(Holt* module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/holt_dark.svg")));

        // screws
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // params
        addParam(createParamCentered<RwKnobMediumDark>(Vec(45.0, 65.0), module, Holt::FREQUENCY_PARAM));
        addParam(createParamCentered<RwKnobMediumDark>(Vec(45.0, 125.0), module, Holt::RESONANCE_PARAM));
        addParam(createParamCentered<RwKnobMediumDark>(Vec(45.0, 185.0), module, Holt::POLES_PARAM));

        // inputs
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 245.0), module, Holt::FREQUENCY_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 245.0), module, Holt::RESONANCE_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(45.0, 285.0), module, Holt::POLES_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 325.0), module, Holt::IN_INPUT));

        // output
        addOutput(createOutputCentered<RwPJ301MPort>(Vec(63.75, 325.0), module, Holt::OUT_OUTPUT));
    }
};

Model* modelHolt = createModel<Holt, HoltWidget>("holt");
