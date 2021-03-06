/***********************************************************************************************
Distance
--------
VCV Rack module based on Distance by Chris Johnson from Airwindows <www.airwindows.com>

Ported and designed by Jens Robert Janke 

Changes/Additions:
- mono
- CV inputs for distance and dry/wet
- polyphonic

See ./LICENSE.md for all licenses
************************************************************************************************/

#include "plugin.hpp"

struct Distance : Module {
    enum ParamIds {
        DISTANCE_PARAM,
        DRYWET_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        DISTANCE_CV_INPUT,
        DRYWET_CV_INPUT,
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
    float distanceParam;
    float drywetParam;

    // state variables (as arrays in order to handle up to 16 polyphonic channels)
    double lastclamp[16];
    double clasp[16];
    double change[16];
    double thirdresult[16];
    double prevresult[16];
    double last[16];
    double fpNShape[16];

    // other variables, which do not need to be updated every cycle
    double overallscale;
    double softslew;
    double filtercorrect;
    double thirdfilter;
    double levelcorrect;
    double wet;
    double dry;
    float lastDistanceParam;
    float lastDrywetParam;

    Distance()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(DISTANCE_PARAM, 0.f, 1.f, 0.f, "Distance");
        configParam(DRYWET_PARAM, 0.f, 1.f, 1.f, "Dry/Wet");

        configInput(DISTANCE_CV_INPUT, "Distance CV");
        configInput(DRYWET_CV_INPUT, "Dry/wet CV");
        configInput(IN_INPUT, "Signal");
        configOutput(OUT_OUTPUT, "Signal");

        configBypass(IN_INPUT, OUT_OUTPUT);

        onReset();
    }

    void onReset() override
    {
        onSampleRateChange();

        for (int i = 0; i < 16; i++) {
            thirdresult[i] = prevresult[i] = lastclamp[i] = clasp[i] = change[i] = last[i] = 0.0;
            fpNShape[i] = 0.0;
        }

        softslew = 0.0;
        filtercorrect = 0.0;
        thirdfilter = 0.0;
        levelcorrect = 0.0;
        wet = 0.0;
        dry = 0.0;
        lastDistanceParam = 0.0;
        lastDrywetParam = 0.0;
    }

    void onSampleRateChange() override
    {
        float sampleRate = APP->engine->getSampleRate();

        overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= sampleRate;
    }

    void process(const ProcessArgs& args) override
    {
        if (outputs[OUT_OUTPUT].isConnected()) {

            distanceParam = params[DISTANCE_PARAM].getValue();
            distanceParam += inputs[DISTANCE_CV_INPUT].getVoltage() / 5;
            distanceParam = clamp(distanceParam, 0.01f, 0.99f);

            drywetParam = params[DRYWET_PARAM].getValue();
            drywetParam += inputs[DRYWET_CV_INPUT].getVoltage() / 5;
            drywetParam = clamp(drywetParam, 0.01f, 0.99f);

            // update only if distanceParam has changed
            if (distanceParam != lastDistanceParam) {
                softslew = (pow(distanceParam * 2.0, 3.0) * 12.0) + 0.6;
                softslew *= overallscale;
                filtercorrect = softslew / 2.0;
                thirdfilter = softslew / 3.0;
                levelcorrect = 1.0 + (softslew / 6.0);

                lastDistanceParam = distanceParam;
            }

            // update only if distanceParam has changed
            if (drywetParam != lastDrywetParam) {
                wet = drywetParam;
                dry = 1.0 - wet;

                lastDrywetParam = drywetParam;
            }

            double postfilter;
            double bridgerectifier;
            double inputSample;
            double drySample;

            // number of polyphonic channels
            int numChannels = std::max(1, inputs[IN_INPUT].getChannels());

            // for each poly channel
            for (int i = 0; i < numChannels; i++) {

                // input
                inputSample = inputs[IN_INPUT].getPolyVoltage(i);

                // pad gain
                inputSample *= gainCut;

                drySample = inputSample;

                inputSample *= softslew;
                lastclamp[i] = clasp[i];
                clasp[i] = inputSample - last[i];
                postfilter = change[i] = fabs(clasp[i] - lastclamp[i]);
                postfilter += filtercorrect;
                if (change[i] > 1.5707963267949)
                    change[i] = 1.5707963267949;
                bridgerectifier = (1.0 - sin(change[i]));
                if (bridgerectifier < 0.0)
                    bridgerectifier = 0.0;
                inputSample = last[i] + (clasp[i] * bridgerectifier);
                last[i] = inputSample;
                inputSample /= softslew;
                inputSample += (thirdresult[i] * thirdfilter);
                inputSample /= (thirdfilter + 1.0);
                inputSample += (prevresult[i] * postfilter);
                inputSample /= (postfilter + 1.0);
                //do an IIR like thing to further squish superdistant stuff
                thirdresult[i] = prevresult[i];
                prevresult[i] = inputSample;
                inputSample *= levelcorrect;

                if (wet < 1.0) {
                    inputSample = (drySample * dry) + (inputSample * wet);
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

struct DistanceWidget : ModuleWidget {
    DistanceWidget(Distance* module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/distance_dark.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH * 1.5, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH * 1.5, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RwKnobMediumDark>(Vec(30.0, 65.0), module, Distance::DISTANCE_PARAM));
        addParam(createParamCentered<RwKnobMediumDark>(Vec(30.0, 125.0), module, Distance::DRYWET_PARAM));

        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 205.0), module, Distance::DISTANCE_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 245.0), module, Distance::DRYWET_CV_INPUT));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(30.0, 285.0), module, Distance::IN_INPUT));

        addOutput(createOutputCentered<RwPJ301MPort>(Vec(30.0, 325.0), module, Distance::OUT_OUTPUT));
    }
};

Model* modelDistance = createModel<Distance, DistanceWidget>("distance");
