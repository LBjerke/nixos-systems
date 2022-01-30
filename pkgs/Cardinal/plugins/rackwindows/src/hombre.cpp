/***********************************************************************************************
Hombre
------
VCV Rack module based on Hombre by Chris Johnson from Airwindows <www.airwindows.com>

Ported and designed by Jens Robert Janke 

Changes/Additions:
- mono
- CV inputs for voicing and intensity
- polyphonic

See ./LICENSE.md for all licenses
************************************************************************************************/

#include "plugin.hpp"

struct Hombre : Module {
    enum ParamIds {
        VOICING_PARAM,
        INTENSITY_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        VOICING_CV_INPUT,
        INTENSITY_CV_INPUT,
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

    // control parameters
    float voicingParam;
    float intensityParam;

    // state variables (as arrays in order to handle up to 16 polyphonic channels)
    double p[16][4001];
    double slide[16];
    int gcount[16];
    double fpNShape[16];

    // other variables, which do not need to be updated every cycle
    double overallscale;
    double target;
    int widthA;
    int widthB;
    double wet;
    double dry;

    Hombre()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(VOICING_PARAM, 0.f, 1.f, 0.5f, "Voicing");
        configParam(INTENSITY_PARAM, 0.f, 1.f, 0.5f, "Intensity");

        configInput(VOICING_CV_INPUT, "Voicing CV");
        configInput(INTENSITY_CV_INPUT, "Intensity CV");
        configInput(IN_INPUT, "Signal");
        configOutput(OUT_OUTPUT, "Signal");

        configBypass(IN_INPUT, OUT_OUTPUT);

        onReset();
    }

    void onReset() override
    {
        onSampleRateChange();

        for (int i = 0; i < 16; i++) {
            for (int count = 0; count < 4000; count++) {
                p[i][count] = 0.0;
            }
            gcount[i] = 0;
            slide[i] = 0.5;
            fpNShape[i] = 0.0;
        }
    }

    void onSampleRateChange() override
    {
        float sampleRate = APP->engine->getSampleRate();

        overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= sampleRate;

        widthA = (int)(1.0 * overallscale);
        widthB = (int)(7.0 * overallscale); //max 364 at 44.1, 792 at 96K
    }

    void process(const ProcessArgs& args) override
    {
        if (outputs[OUT_OUTPUT].isConnected()) {

            voicingParam = params[VOICING_PARAM].getValue();
            voicingParam += inputs[VOICING_CV_INPUT].getVoltage() / 5;
            voicingParam = clamp(voicingParam, 0.01f, 0.99f);

            intensityParam = params[INTENSITY_PARAM].getValue();
            intensityParam += inputs[INTENSITY_CV_INPUT].getVoltage() / 5;
            intensityParam = clamp(intensityParam, 0.01f, 0.99f);

            target = voicingParam;
            wet = intensityParam;
            dry = 1.0 - wet;

            double offsetA;
            double offsetB;
            double total;
            int count;
            double inputSample;
            double drySample;

            // input
            int numChannels = std::max(1, inputs[IN_INPUT].getChannels());

            // for each poly channel
            for (int i = 0; i < numChannels; i++) {

                // input
                inputSample = inputs[IN_INPUT].getPolyVoltage(i);

                // pad gain
                inputSample *= gainCut;

                drySample = inputSample;

                slide[i] = (slide[i] * 0.9997) + (target * 0.0003);

                //adjust for sample rate
                offsetA = ((pow(slide[i], 2)) * 77) + 3.2;
                offsetB = (3.85 * offsetA) + 41;
                offsetA *= overallscale;
                offsetB *= overallscale;

                if (gcount[i] < 1 || gcount[i] > 2000) {
                    gcount[i] = 2000;
                }
                count = gcount[i];

                //double buffer
                p[i][count + 2000] = p[i][count] = inputSample;

                count = (int)(gcount[i] + floor(offsetA));

                total = p[i][count] * 0.391; //less as value moves away from .0
                total += p[i][count + widthA]; //we can assume always using this in one way or another?
                total += p[i][count + widthA + widthA] * 0.391; //greater as value moves away from .0

                inputSample += ((total * 0.274));

                count = (int)(gcount[i] + floor(offsetB));

                total = p[i][count] * 0.918; //less as value moves away from .0
                total += p[i][count + widthB]; //we can assume always using this in one way or another?
                total += p[i][count + widthB + widthB] * 0.918; //greater as value moves away from .0

                inputSample -= ((total * 0.629));

                inputSample /= 4;

                //still scrolling through the samples, remember
                gcount[i]--;

                if (wet != 1.0) {
                    inputSample = (inputSample * wet) + (drySample * dry);
                }

                // bring gain back up
                inputSample *= gainBoost;

                // output
                outputs[OUT_OUTPUT].setChannels(numChannels);
                outputs[OUT_OUTPUT].setVoltage(inputSample, i);
            }
        }
    }
};

struct HombreWidget : ModuleWidget {
    HombreWidget(Hombre* module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/hombre_dark.svg")));

        // screws
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH * 1.5, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH * 1.5, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // knobs
        addParam(createParamCentered<RwKnobMediumDark>(Vec(30.0, 65.0), module, Hombre::VOICING_PARAM));
        addParam(createParamCentered<RwKnobMediumDark>(Vec(30.0, 125.0), module, Hombre::INTENSITY_PARAM));

        // inputs
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 205.0), module, Hombre::VOICING_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 245.0), module, Hombre::INTENSITY_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 285.0), module, Hombre::IN_INPUT));

        // outputs
        addOutput(createOutputCentered<RwPJ301MPort>(Vec(30.0, 325.0), module, Hombre::OUT_OUTPUT));
    }
};

Model* modelHombre = createModel<Hombre, HombreWidget>("hombre");
