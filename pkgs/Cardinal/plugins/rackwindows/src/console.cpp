/***********************************************************************************************
Console
------
VCV Rack module based on Console by Chris Johnson from Airwindows <www.airwindows.com>

Ported and designed by Jens Robert Janke

Changes/Additions:
- console channel and buss combined into an 8-channel stereo summing mixer
- no gain control
- polyphonic
- console types: Console6, PurestConsole

Additional code inspired by the Fundamental Mixer by Andrew Belt.

See ./LICENSE.md for all licenses
************************************************************************************************/

#include "plugin.hpp"

// console types
#define CONSOLE_6 0
#define PUREST_CONSOLE 1

struct Console : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(IN_L_INPUTS, 9),
        ENUMS(IN_R_INPUTS, 9),
        IN_ST_L_INPUT,
        IN_ST_R_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        OUT_L_OUTPUT,
        OUT_R_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(VU_LIGHTS, 9),
        NUM_LIGHTS
    };

    // module variables
    const double gainCut = 0.1;
    const double gainBoost = 10.0;
    int consoleType;
    dsp::VuMeter2 vuMeters[9];
    dsp::ClockDivider lightDivider;
    // float drive;

    // state variables (as arrays in order to handle up to 16 polyphonic channels)
    uint32_t fpd[16];

    Console()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int i = 0; i < 9; i++) {
            configInput(IN_L_INPUTS + i, string::f("Channel %d L", i + 1));
            configInput(IN_R_INPUTS + i, string::f("Channel %d R", i + 1));
        }
        configInput(IN_ST_L_INPUT, "Stereo Channel L");
        configInput(IN_ST_R_INPUT, "Stereo Channel R");

        configOutput(OUT_L_OUTPUT, "Mixed L");
        configOutput(OUT_R_OUTPUT, "Mixed R");

        configBypass(IN_L_INPUTS + 0, OUT_L_OUTPUT);
        configBypass(IN_R_INPUTS + 0, OUT_R_OUTPUT);

        consoleType = loadConsoleType();
        lightDivider.setDivision(512);
        onReset();
    }

    void onReset() override
    {
        for (int i = 0; i < 16; i++) {
            fpd[i] = 17;
        }
    }

    json_t* dataToJson() override
    {
        json_t* rootJ = json_object();

        // consoleType
        json_object_set_new(rootJ, "consoleType", json_integer(consoleType));

        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override
    {
        // consoleType
        json_t* consoleTypeJ = json_object_get(rootJ, "consoleType");
        if (consoleTypeJ)
            consoleType = json_integer_value(consoleTypeJ);
    }

    double encode(double inputSample, int consoleType = 0)
    {
        switch (consoleType) {
        case PUREST_CONSOLE: // PurestConsoleChannel
            inputSample *= 0.25;
            inputSample = sin(inputSample);
            break;
        case CONSOLE_6: // Console6Channel
            inputSample *= 0.2;
            if (inputSample > 1.0)
                inputSample = 1.0;
            else if (inputSample > 0.0)
                inputSample = 1.0 - pow(1.0 - inputSample, 2.0);

            if (inputSample < -1.0)
                inputSample = -1.0;
            else if (inputSample < 0.0)
                inputSample = -1.0 + pow(1.0 + inputSample, 2.0);
            break;
        }
        return inputSample;
    }

    double decode(double inputSample, int consoleType = 0)
    {
        switch (consoleType) {
        case PUREST_CONSOLE: // PurestConsoleBuss
            inputSample = sin(inputSample);
            inputSample *= 4.0;
            break;
        case CONSOLE_6: // Console6Buss
            if (inputSample > 1.0)
                inputSample = 1.0;
            else if (inputSample > 0.0)
                inputSample = 1.0 - pow(1.0 - inputSample, 0.5);

            if (inputSample < -1.0)
                inputSample = -1.0;
            else if (inputSample < 0.0)
                inputSample = -1.0 + pow(1.0 + inputSample, 0.5);
            inputSample *= 5.0;
            break;
        }
        return inputSample;
    }

    float consoleChannel(Input& input, double mix[], int numChannels)
    {
        if (input.isConnected()) {
            float sum = 0.0f;

            // input
            float inputSamples[16] = {};
            input.readVoltages(inputSamples);

            for (int i = 0; i < numChannels; i++) {

                double inputSample = inputSamples[i];

                // inputSample *= rescale(drive, 0, 1, 0.5, 2);

                // calculate sum for VU meter
                sum += inputSample;

                // pad gain, will be boosted in consoleBuss()
                inputSample *= gainCut;

                // encode
                inputSample = encode(inputSample, consoleType);

                // add to mix
                mix[i] += inputSample;
            }
            return sum;
        }
        return 0.0f;
    }

    void consoleBuss(Output& output, double mix[], int maxChannels)
    {
        if (output.isConnected()) {
            float out[16] = {};

            for (int i = 0; i < maxChannels; i++) {
                double inputSample = mix[i];

                // decode
                inputSample = decode(inputSample, consoleType);

                // bring gain back up
                inputSample *= gainBoost;

                out[i] = (float)inputSample;
            }

            output.setChannels(maxChannels);
            output.writeVoltages(out);
        }
    }

    void process(const ProcessArgs& args) override
    {
        if (outputs[OUT_L_OUTPUT].isConnected() || outputs[OUT_R_OUTPUT].isConnected()) {
            double mixL[16] = {};
            double mixR[16] = {};
            float sumL = 0.0;
            float sumR = 0.0;
            int numChannelsL = 1;
            int numChannelsR = 1;
            int maxChannelsL = 1;
            int maxChannelsR = 1;

            // for each mixer channel
            for (int i = 0; i < 9; i++) {
                numChannelsL = inputs[IN_L_INPUTS + i].getChannels();
                maxChannelsL = std::max(maxChannelsL, numChannelsL);
                sumL = consoleChannel(inputs[IN_L_INPUTS + i], mixL, numChannelsL); // encode L

                numChannelsR = inputs[IN_R_INPUTS + i].getChannels();
                maxChannelsR = std::max(maxChannelsR, numChannelsR);
                sumR = consoleChannel(inputs[IN_R_INPUTS + i], mixR, numChannelsR); // encode R

                // channel VU light
                vuMeters[i].process(args.sampleTime, (sumL + sumR) / 5.f);
                if (lightDivider.process()) {
                    float b = vuMeters[i].getBrightness(-18.f, 0.f);
                    lights[VU_LIGHTS + i].setBrightness(b);
                }
            }

            consoleBuss(outputs[OUT_L_OUTPUT], mixL, maxChannelsL); // decode L
            consoleBuss(outputs[OUT_R_OUTPUT], mixR, maxChannelsR); // decode R
        }
    }
};

struct ConsoleWidget : ModuleWidget {

    SvgPanel* darkPanel;

    // console type item
    struct ConsoleTypeItem : MenuItem {
        Console* module;
        int consoleType;

        void onAction(const event::Action& e) override
        {
            module->consoleType = consoleType;
        }

        void step() override
        {
            rightText = (module->consoleType == consoleType) ? "✔" : "";
        }
    };

    // struct DriveSlider : ui::Slider {
    //     struct DriveQuantity : Quantity {
    //         Console* module;

    //         DriveQuantity(Console* module)
    //         {
    //             this->module = module;
    //         }
    //         void setValue(float value) override
    //         {
    //             module->drive = math::clamp(value, 0.f, 1.f);
    //         }
    //         float getValue() override
    //         {
    //             return module->drive;
    //         }
    //         float getDefaultValue() override
    //         {
    //             return 0.5f;
    //         }
    //         float getDisplayValue() override
    //         {
    //             return getValue() * 12 - 6;
    //         }
    //         void setDisplayValue(float displayValue) override
    //         {
    //             setValue(displayValue / 12 - 6);
    //         }
    //         std::string getLabel() override
    //         {
    //             return "Drive";
    //         }
    //         std::string getUnit() override
    //         {
    //             return " dB";
    //         }
    //     };

    //     DriveSlider(Console* module)
    //     {
    //         this->box.size.x = 100.0;
    //         quantity = new DriveQuantity(module);
    //     }
    //     ~DriveSlider()
    //     {
    //         delete quantity;
    //     }
    // };

    void appendContextMenu(Menu* menu) override
    {
        Console* module = dynamic_cast<Console*>(this->module);
        assert(module);

        menu->addChild(new MenuSeparator()); // separator

        MenuLabel* consoleTypeLabel = new MenuLabel(); // menu label
        consoleTypeLabel->text = "Type";
        menu->addChild(consoleTypeLabel);

        ConsoleTypeItem* console6 = new ConsoleTypeItem(); // Console6
        console6->text = "Console6";
        console6->module = module;
        console6->consoleType = CONSOLE_6;
        menu->addChild(console6);

        ConsoleTypeItem* purestConsole = new ConsoleTypeItem(); // PurestConsole
        purestConsole->text = "PurestConsole";
        purestConsole->module = module;
        purestConsole->consoleType = PUREST_CONSOLE;
        menu->addChild(purestConsole);

        // menu->addChild(new MenuSeparator()); // separator

        // menu->addChild(new DriveSlider(module));
    }

    ConsoleWidget(Console* module)
    {
        setModule(module);

        // panel
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/console_dark.svg")));

        // screws
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // lights
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 55.0), module, Console::VU_LIGHTS + 0));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 85.0), module, Console::VU_LIGHTS + 1));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 115.0), module, Console::VU_LIGHTS + 2));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 145.0), module, Console::VU_LIGHTS + 3));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 175.0), module, Console::VU_LIGHTS + 4));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 205.0), module, Console::VU_LIGHTS + 5));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 235.0), module, Console::VU_LIGHTS + 6));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 265.0), module, Console::VU_LIGHTS + 7));
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(45.0, 295.0), module, Console::VU_LIGHTS + 8));

        // inputs
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 55.0), module, Console::IN_L_INPUTS + 0));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 55.0), module, Console::IN_R_INPUTS + 0));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 85.0), module, Console::IN_L_INPUTS + 1));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 85.0), module, Console::IN_R_INPUTS + 1));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 115.0), module, Console::IN_L_INPUTS + 2));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 115.0), module, Console::IN_R_INPUTS + 2));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 145.0), module, Console::IN_L_INPUTS + 3));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 145.0), module, Console::IN_R_INPUTS + 3));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 175.0), module, Console::IN_L_INPUTS + 4));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 175.0), module, Console::IN_R_INPUTS + 4));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 205.0), module, Console::IN_L_INPUTS + 5));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 205.0), module, Console::IN_R_INPUTS + 5));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 235.0), module, Console::IN_L_INPUTS + 6));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 235.0), module, Console::IN_R_INPUTS + 6));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 265.0), module, Console::IN_L_INPUTS + 7));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 265.0), module, Console::IN_R_INPUTS + 7));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(26.25, 295.0), module, Console::IN_L_INPUTS + 8));
        addInput(createInputCentered<RwPJ301MPortSilver>(Vec(63.75, 295.0), module, Console::IN_R_INPUTS + 8));

        // outputs
        addOutput(createOutputCentered<RwPJ301MPort>(Vec(26.25, 325.0), module, Console::OUT_L_OUTPUT));
        addOutput(createOutputCentered<RwPJ301MPort>(Vec(63.75, 325.0), module, Console::OUT_R_OUTPUT));
    }
};

Model* modelConsole = createModel<Console, ConsoleWidget>("console");
