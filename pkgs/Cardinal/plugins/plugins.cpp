/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include "rack.hpp"
#include "plugin.hpp"

#include "DistrhoUtils.hpp"

// Cardinal (built-in)
#include "Cardinal/src/plugin.hpp"

#ifndef NOPLUGINS
// 21kHz
#include "21kHz/src/21kHz.hpp"

// AmalgamatedHarmonics
#include "AmalgamatedHarmonics/src/AH.hpp"

// AnimatedCircuits
#include "AnimatedCircuits/src/plugin.hpp"

// Aria
/* NOTE too much noise in original include, do this a different way
// #include "AriaModules/src/plugin.hpp"
*/
#define modelBlank modelAriaBlank
extern Model* modelSplort;
extern Model* modelSmerge;
extern Model* modelSpleet;
extern Model* modelSwerge;
extern Model* modelSplirge;
// extern Model* modelSrot;
extern Model* modelQqqq;
extern Model* modelQuack;
extern Model* modelQ;
extern Model* modelQuale;
extern Model* modelDarius;
extern Model* modelSolomon4;
extern Model* modelSolomon8;
extern Model* modelSolomon16;
extern Model* modelPsychopump;
extern Model* modelPokies4;
extern Model* modelGrabby;
extern Model* modelRotatoes4;
extern Model* modelUndular;
extern Model* modelBlank;
#undef modelBlank

// AS
/*
#define modelADSR modelASADSR
#define modelVCA modelASVCA
#define YellowRedLight ASYellowRedLight
#include "AS/src/AS.hpp"
#undef modelADSR
#undef modelVCA
#undef YellowRedLight
*/

// Atelier
/*
#include "Atelier/src/plugin.hpp"
*/

// AudibleInstruments
#include "AudibleInstruments/src/plugin.hpp"

// Autinn
/* NOTE too much noise in original include, do this a different way
// #include "Autinn/src/Autinn.hpp"
*/
#define modelChord modelAutinnChord
#define modelVibrato modelAutinnVibrato
extern Model* modelJette;
extern Model* modelFlora;
extern Model* modelOxcart;
extern Model* modelDeadband;
extern Model* modelDigi;
extern Model* modelFlopper;
extern Model* modelAmp;
extern Model* modelDC;
extern Model* modelSjip;
extern Model* modelBass;
extern Model* modelSquare;
extern Model* modelSaw;
extern Model* modelBoomerang;
extern Model* modelVibrato;
extern Model* modelVectorDriver; //deprecated
extern Model* modelCVConverter;
extern Model* modelZod;
extern Model* modelTriBand;
extern Model* modelMixer6;
extern Model* modelNon;
extern Model* modelFil;
extern Model* modelNap;
extern Model* modelMelody;
extern Model* modelChord;
#undef modelChord
#undef modelVibrato

// BaconPlugs
#define INCLUDE_COMPONENTS_HPP
#include "BaconPlugs/src/BaconPlugs.hpp"
#undef INCLUDE_COMPONENTS_HPP
#undef SCREW_WIDTH
#undef RACK_HEIGHT

// Befaco
/*
#define modelADSR modelBefacoADSR
#define modelMixer modelBefacoMixer
#include "Befaco/src/plugin.hpp"
#undef modelADSR
#undef modelMixer
*/

// Bidoo
#include "Bidoo/src/plugin.hpp"

// BogaudioModules - force dark skin as default
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define private public
#include "BogaudioModules/src/skins.hpp"
#undef private

// BogaudioModules
#define modelADSR modelBogaudioADSR
#define modelLFO modelBogaudioLFO
#define modelNoise modelBogaudioNoise
#define modelVCA modelBogaudioVCA
#define modelVCF modelBogaudioVCF
#define modelVCO modelBogaudioVCO
extern Model* modelVCO;
extern Model* modelLVCO;
extern Model* modelSine;
extern Model* modelPulse;
extern Model* modelXCO;
extern Model* modelAdditator;
extern Model* modelFMOp;
extern Model* modelChirp;
extern Model* modelLFO;
extern Model* modelLLFO;
extern Model* modelFourFO;
extern Model* modelEightFO;
extern Model* modelVCF;
extern Model* modelLVCF;
extern Model* modelFFB;
extern Model* modelEQ;
extern Model* modelEQS;
extern Model* modelLPG;
extern Model* modelLLPG;
extern Model* modelMegaGate;
extern Model* modelPEQ;
extern Model* modelPEQ6;
extern Model* modelPEQ6XF;
extern Model* modelPEQ14;
extern Model* modelPEQ14XF;
extern Model* modelDADSRH;
extern Model* modelDADSRHPlus;
extern Model* modelShaper;
extern Model* modelShaperPlus;
extern Model* modelAD;
extern Model* modelASR;
extern Model* modelADSR;
extern Model* modelVish;
extern Model* modelFollow;
extern Model* modelDGate;
extern Model* modelRGate;
extern Model* modelEdge;
extern Model* modelNoise;
extern Model* modelSampleHold;
extern Model* modelWalk2;
extern Model* modelWalk;
extern Model* modelMix8;
extern Model* modelMix8x;
extern Model* modelMix4;
extern Model* modelMix4x;
extern Model* modelMix2;
extern Model* modelMix1;
extern Model* modelVCM;
extern Model* modelMute8;
extern Model* modelPan;
extern Model* modelXFade;
extern Model* modelVCA;
extern Model* modelVCAmp;
extern Model* modelVelo;
extern Model* modelUMix;
extern Model* modelMumix;
extern Model* modelMatrix81;
extern Model* modelMatrix18;
extern Model* modelMatrix44;
extern Model* modelMatrix44Cvm;
extern Model* modelMatrix88;
extern Model* modelMatrix88Cv;
extern Model* modelMatrix88M;
extern Model* modelSwitch81;
extern Model* modelSwitch18;
extern Model* modelSwitch44;
extern Model* modelSwitch88;
extern Model* modelSwitch1616;
extern Model* modelAMRM;
extern Model* modelPressor;
extern Model* modelClpr;
extern Model* modelLmtr;
extern Model* modelNsgt;
extern Model* modelCmpDist;
extern Model* modelOneEight;
extern Model* modelEightOne;
extern Model* modelAddrSeq;
extern Model* modelAddrSeqX;
extern Model* modelPgmr;
extern Model* modelPgmrX;
extern Model* modelVU;
extern Model* modelAnalyzer;
extern Model* modelAnalyzerXL;
extern Model* modelRanalyzer;
extern Model* modelDetune;
extern Model* modelStack;
extern Model* modelReftone;
extern Model* modelMono;
extern Model* modelArp;
extern Model* modelAssign;
extern Model* modelUnison;
extern Model* modelPolyCon8;
extern Model* modelPolyCon16;
extern Model* modelPolyOff8;
extern Model* modelPolyOff16;
extern Model* modelPolyMult;
extern Model* modelBool;
extern Model* modelCmp;
extern Model* modelCVD;
extern Model* modelFlipFlop;
extern Model* modelInv;
extern Model* modelManual;
extern Model* modelFourMan;
extern Model* modelMult;
extern Model* modelOffset;
extern Model* modelSlew;
extern Model* modelSums;
extern Model* modelSwitch;
extern Model* modelLgsw;
extern Model* modelBlank3;
extern Model* modelBlank6;
#ifdef EXPERIMENTAL
extern Model* modelLag;
extern Model* modelPEQ14XR;
extern Model* modelPEQ14XV;
#endif
#ifdef TEST
extern Model* modelTest;
extern Model* modelTest2;
extern Model* modelTestExpanderBase;
extern Model* modelTestExpanderExtension;
extern Model* modelTestGl;
extern Model* modelTestVCF;
#endif
#undef modelADSR
#undef modelLFO
#undef modelNoise
#undef modelVCA
#undef modelVCF
#undef modelVCO

// cf
#include "cf/src/plugin.hpp"

// ChowDSP
#include "ChowDSP/src/plugin.hpp"
#define init initChowDSP
#include "ChowDSP/src/plugin.cpp"
#undef init

// DrumKit
#include "DrumKit/src/DrumKit.hpp"
void setupSamples();

// ESeries
#include "ESeries/src/plugin.hpp"

// ExpertSleepers-Encoders
#include "ExpertSleepers-Encoders/src/Encoders.hpp"

// Extratone
#include "Extratone/src/plugin.hpp"

// FehlerFabrik
#include "FehlerFabrik/src/plugin.hpp"

// Fundamental
/*
#include "Fundamental/src/plugin.hpp"
*/

// GlueTheGiant
#include "GlueTheGiant/src/plugin.hpp"
bool audition_mixer = false;
bool audition_depot = false;
int gtg_default_theme = 1;
int loadGtgPluginDefault(const char*, int) { return 1; }
void saveGtgPluginDefault(const char*, int) {}

// GrandeModular
#include "GrandeModular/src/plugin.hpp"

// HetrickCV
#define modelASR modelHetrickCVASR
#define modelBlankPanel modelHetrickCVBlankPanel
#define modelFlipFlop modelHetrickCVFlipFlop
#define modelMidSide modelHetrickCVMidSide
#define modelMinMax modelHetrickCVMinMax
extern Model* modelTwoToFour;
extern Model* modelAnalogToDigital;
extern Model* modelASR;
extern Model* modelBinaryGate;
extern Model* modelBinaryNoise;
extern Model* modelBitshift;
extern Model* modelBlankPanel;
extern Model* modelBoolean3;
extern Model* modelChaos1Op;
extern Model* modelChaos2Op;
extern Model* modelChaos3Op;
extern Model* modelChaoticAttractors;
extern Model* modelClockedNoise;
extern Model* modelComparator;
extern Model* modelContrast;
extern Model* modelCrackle;
extern Model* modelDataCompander;
extern Model* modelDelta;
extern Model* modelDigitalToAnalog;
extern Model* modelDust;
extern Model* modelExponent;
extern Model* modelFBSineChaos;
extern Model* modelFlipFlop;
extern Model* modelFlipPan;
extern Model* modelGateJunction;
extern Model* modelGingerbread;
extern Model* modelLogicCombine;
extern Model* modelMidSide;
extern Model* modelMinMax;
extern Model* modelRandomGates;
extern Model* modelRotator;
extern Model* modelRungler;
extern Model* modelScanner;
extern Model* modelWaveshape;
extern Model* modelXYToPolar;
#undef modelASR
#undef modelBlankPanel
#undef modelFlipFlop
#undef modelMidSide
#undef modelMinMax

// ImpromptuModular
/* NOTE too much noise in original include, do this a different way
// #include "ImpromptuModular/src/ImpromptuModular.hpp"
*/
extern Model* modelAdaptiveQuantizer;
extern Model* modelBigButtonSeq;
extern Model* modelBigButtonSeq2;
extern Model* modelChordKey;
extern Model* modelChordKeyExpander;
extern Model* modelClocked;
extern Model* modelClockedExpander;
extern Model* modelClkd;
extern Model* modelCvPad;
extern Model* modelFoundry;
extern Model* modelFoundryExpander;
extern Model* modelFourView;
extern Model* modelGateSeq64;
extern Model* modelGateSeq64Expander;
extern Model* modelHotkey;
extern Model* modelPart;
extern Model* modelPhraseSeq16;
extern Model* modelPhraseSeq32;
extern Model* modelPhraseSeqExpander;
extern Model* modelProbKey;
// extern Model* modelProbKeyExpander;
extern Model* modelSemiModularSynth;
extern Model* modelTact;
extern Model* modelTact1;
extern Model* modelTactG;
extern Model* modelTwelveKey;
extern Model* modelWriteSeq32;
extern Model* modelWriteSeq64;
extern Model* modelBlankPanel;

// ihtsyn
/*
#include "ihtsyn/src/plugin.hpp"
*/

// JW-Modules
#define modelQuantizer modelJWQuantizer
#include "JW-Modules/src/JWModules.hpp"
#undef modelQuantizer

// LifeFormModular
/* NOTE too much noise in original include, do this a different way
// #include "LifeFormModular/src/plugin.hpp"
*/
extern Model* modelTimeDiktat;
extern Model* modelSequenceModeler;
extern Model* modelPitchDiktat;
extern Model* modelPitchIntegrator;
extern Model* modelBurstIntegrator;
extern Model* modelQuadModulator;
extern Model* modelImpulseControl;
extern Model* modelQuadSteppedOffset;
extern Model* modelPercussiveVibration;
extern Model* modelQuadUtility;
extern Model* modelAdditiveVibration;
extern Model* modelComplexOsc;
extern Model* modelDriftgen;

// LittleUtils
#include "LittleUtils/src/plugin.hpp"

// LomasModules
#include "LomasModules/src/plugin.hpp"
#undef DR_WAV_IMPLEMENTATION

// LyraeModules
/* NOTE too much noise in original include, do this a different way
// #include "LyraeModules/src/plugin.hpp"
*/
#define modelDelta modelLyraeDelta
extern Model* modelSulafat;
extern Model* modelGamma;
extern Model* modelDelta;
extern Model* modelVega;
extern Model* modelBD383238;
extern Model* modelZeta;
#undef modelDelta

// MindMeldModular
/* NOTE too much noise in original include, do this a different way
// #include "MindMeldModular/src/MindMeldModular.hpp"
*/
extern Model* modelMixMasterJr;
extern Model* modelAuxExpanderJr;
extern Model* modelMixMaster;
extern Model* modelAuxExpander;
extern Model* modelMeld;
extern Model* modelUnmeld;
extern Model* modelMSMelder;
extern Model* modelEqMaster;
extern Model* modelEqExpander;
extern Model* modelBassMaster;
extern Model* modelBassMasterJr;
extern Model* modelShapeMaster;

// Mog
#include "Mog/src/plugin.hpp"

// mscHack
/* NOTE too much noise in original include, do this a different way
// #include "mscHack/src/mscHack.hpp"
*/
extern Model* modelCompressor;
extern Model* modelSynthDrums;
extern Model* modelSEQ_6x32x16;
extern Model* modelMasterClockx8;
extern Model* modelMasterClockx4;
extern Model* modelSEQ_Envelope_8;
extern Model* modelSeq_Triad2;
extern Model* modelARP700;
extern Model* modelMix_24_4_4;
extern Model* modelMix_16_4_4;
extern Model* modelMix_9_3_4;
extern Model* modelMix_4_0_4;
extern Model* modelASAF8;
extern Model* modelPingPong;
extern Model* modelStepDelay;
extern Model* modelOsc_3Ch;
extern Model* modelDronez;
extern Model* modelMorze;
extern Model* modelWindz;
extern Model* modelLorenz;
extern Model* modelAlienz;
extern Model* modelOSC_WaveMorph_3;
extern Model* modelMaude_221;

// Prism
# include "Prism/src/plugin.hpp"

// rackwindows
#include "rackwindows/src/plugin.hpp"

// repelzen
#define modelBlank modelrepelzenBlank
#define modelMixer modelrepelzenMixer
#define modelWerner modelrepelzenWerner
#define tanh_pade repelzentanh_pade
#include "repelzen/src/repelzen.hpp"
#undef modelBlank
#undef modelMixer
#undef modelWerner
#undef tanh_pade

// sonusmodular
#include "sonusmodular/src/sonusmodular.hpp"

// ValleyAudio
#include "ValleyAudio/src/Valley.hpp"

// ZetaCarinaeModules
#include "ZetaCarinaeModules/src/plugin.hpp"

// ZZC
/*
#define DISPLAYS_H
#define ZZC_SHARED_H
#define ZZC_WIDGETS_H
#define modelClock modelZZCClock
#include "ZZC/src/ZZC.hpp"
#undef modelClock
*/

#endif // NOPLUGINS

// stuff that reads config files, we dont want that
int loadConsoleType() { return 0; }
int loadDirectOutMode() { return 0; }
void saveConsoleType(int) {}
void saveDarkAsDefault(bool) {}
void saveDirectOutMode(bool) {}
void saveHighQualityAsDefault(bool) {}

// plugin instances
Plugin* pluginInstance__Cardinal;
#ifndef NOPLUGINS
Plugin* pluginInstance__21kHz;
Plugin* pluginInstance__AmalgamatedHarmonics;
Plugin* pluginInstance__AnimatedCircuits;
Plugin* pluginInstance__Aria;
// Plugin* pluginInstance__AS;
// Plugin* pluginInstance__Atelier;
Plugin* pluginInstance__AudibleInstruments;
extern Plugin* pluginInstance__Autinn;
Plugin* pluginInstance__Bacon;
// Plugin* pluginInstance__Befaco;
Plugin* pluginInstance__Bidoo;
Plugin* pluginInstance__BogaudioModules;
Plugin* pluginInstance__cf;
Plugin* pluginInstance__ChowDSP;
extern Plugin* pluginInstance__DrumKit;
Plugin* pluginInstance__ESeries;
Plugin* pluginInstance__ExpertSleepersEncoders;
Plugin* pluginInstance__Extratone;
Plugin* pluginInstance__FehlerFabrik;
// Plugin* pluginInstance__Fundamental;
Plugin* pluginInstance__GrandeModular;
Plugin* pluginInstance__GlueTheGiant;
Plugin* pluginInstance__HetrickCV;
extern Plugin* pluginInstance__ImpromptuModular;
// Plugin* pluginInstance__ihtsyn;
Plugin* pluginInstance__JW;
Plugin* pluginInstance__LifeFormModular;
Plugin* pluginInstance__LittleUtils;
Plugin* pluginInstance__Lomas;
Plugin* pluginInstance__Lyrae;
extern Plugin* pluginInstance__MindMeld;
Plugin* pluginInstance__Mog;
extern Plugin* pluginInstance__mscHack;
Plugin* pluginInstance__Prism;
Plugin* pluginInstance__rackwindows;
Plugin* pluginInstance__repelzen;
Plugin* pluginInstance__sonusmodular;
Plugin* pluginInstance__ValleyAudio;
Plugin* pluginInstance__ZetaCarinaeModules;
// Plugin* pluginInstance__ZZC;
#endif // NOPLUGINS

namespace rack {

namespace asset {
std::string pluginManifest(const std::string& dirname);
std::string pluginPath(const std::string& dirname);
}

// regular plugins
namespace plugin {

struct StaticPluginLoader {
    Plugin* const plugin;
    FILE* file;
    json_t* rootJ;

    StaticPluginLoader(Plugin* const p, const char* const name)
        : plugin(p),
          file(nullptr),
          rootJ(nullptr)
    {
#ifdef DEBUG
        DEBUG("Loading plugin module %s", name);
#endif

        p->path = asset::pluginPath(name);

        const std::string manifestFilename = asset::pluginManifest(name);

        if ((file = std::fopen(manifestFilename.c_str(), "r")) == nullptr)
        {
            d_stderr2("Manifest file %s does not exist", manifestFilename.c_str());
            return;
        }

        json_error_t error;
        if ((rootJ = json_loadf(file, 0, &error)) == nullptr)
        {
            d_stderr2("JSON parsing error at %s %d:%d %s", manifestFilename.c_str(), error.line, error.column, error.text);
            return;
        }

        // force ABI, we use static plugins so this doesnt matter as long as it builds
        json_t* const version = json_string((APP_VERSION_MAJOR + ".0").c_str());
        json_object_set(rootJ, "version", version);
        json_decref(version);
    }

    ~StaticPluginLoader()
    {
        if (rootJ != nullptr)
        {
            plugin->fromJson(rootJ);
            json_decref(rootJ);
            plugins.push_back(plugin);
        }

        if (file != nullptr)
            std::fclose(file);
    }

    bool ok() const noexcept
    {
        return rootJ != nullptr;
    }

    void removeModule(const char* const slugToRemove) const noexcept
    {
        json_t* const modules = json_object_get(rootJ, "modules");
        DISTRHO_SAFE_ASSERT_RETURN(modules != nullptr,);

        size_t i;
        json_t* v;
        json_array_foreach(modules, i, v)
        {
            if (json_t* const slug = json_object_get(v, "slug"))
            {
                if (const char* const value = json_string_value(slug))
                {
                    if (std::strcmp(value, slugToRemove) == 0)
                    {
                        json_array_remove(modules, i);
                        break;
                    }
                }
            }
        }
    }
};

static void initStatic__Cardinal()
{
    Plugin* const p = new Plugin;
    pluginInstance__Cardinal = p;

    const StaticPluginLoader spl(p, "Cardinal");
    if (spl.ok())
    {
        p->addModel(modelCardinalBlank);
        p->addModel(modelGlBars);
        p->addModel(modelHostAudio2);
        p->addModel(modelHostAudio8);
        p->addModel(modelHostCV);
        p->addModel(modelHostMIDI);
        p->addModel(modelHostMIDICC);
        p->addModel(modelHostMIDIGate);
        p->addModel(modelHostMIDIMap);
        p->addModel(modelHostParameters);
        p->addModel(modelHostTime);
        p->addModel(modelTextEditor);
       #ifndef STATIC_BUILD
        p->addModel(modelAudioFile);
        p->addModel(modelCarla);
        p->addModel(modelIldaeil);
        p->addModel(modelMPV);
       #else
        spl.removeModule("AudioFile");
        spl.removeModule("Carla");
        spl.removeModule("Ildaeil");
        spl.removeModule("MPV");
       #endif
    }
}

#ifndef NOPLUGINS
static void initStatic__21kHz()
{
    Plugin* const p = new Plugin;
    pluginInstance__21kHz = p;

    const StaticPluginLoader spl(p, "21kHz");
    if (spl.ok())
    {
        p->addModel(modelPalmLoop);
        p->addModel(modelD_Inf);
        p->addModel(modelTachyonEntangler);
    }
}

static void initStatic__AmalgamatedHarmonics()
{
    Plugin* const p = new Plugin;
    pluginInstance__AmalgamatedHarmonics = p;

    const StaticPluginLoader spl(p, "AmalgamatedHarmonics");
    if (spl.ok())
    {
        p->addModel(modelArp31);
        p->addModel(modelArp32);
        p->addModel(modelBombe);
        p->addModel(modelChord);
        p->addModel(modelCircle);
        p->addModel(modelGalaxy);
        p->addModel(modelGenerative);
        p->addModel(modelImp);
        p->addModel(modelImperfect2);
        p->addModel(modelProgress2);
        p->addModel(modelRuckus);
        p->addModel(modelScaleQuantizer2);
        p->addModel(modelSLN);
        p->addModel(modelMuxDeMux);
        p->addModel(modelPolyProbe);
        p->addModel(modelPolyScope);
        p->addModel(modelPolyUtils);
        p->addModel(modelPolyVolt);
        p->addModel(modelScaleQuantizer);
        p->addModel(modelArpeggiator2);
        p->addModel(modelProgress);
    }
}

static void initStatic__AnimatedCircuits()
{
    Plugin* const p = new Plugin;
    pluginInstance__AnimatedCircuits = p;

    const StaticPluginLoader spl(p, "AnimatedCircuits");
    if (spl.ok())
    {
        p->addModel(model_AC_Folding);
    }
}

static void initStatic__Aria()
{
    Plugin* const p = new Plugin;
    pluginInstance__Aria = p;

    const StaticPluginLoader spl(p, "AriaModules");
    if (spl.ok())
    {
#define modelBlank modelAriaBlank
        p->addModel(modelSplort);
        p->addModel(modelSmerge);
        p->addModel(modelSpleet);
        p->addModel(modelSwerge);
        p->addModel(modelSplirge);
        // p->addModel(modelSrot);
        p->addModel(modelQqqq);
        p->addModel(modelQuack);
        p->addModel(modelQ);
        p->addModel(modelQuale);
        p->addModel(modelDarius);
        p->addModel(modelSolomon4);
        p->addModel(modelSolomon8);
        p->addModel(modelSolomon16);
        p->addModel(modelPsychopump);
        p->addModel(modelPokies4);
        p->addModel(modelGrabby);
        p->addModel(modelRotatoes4);
        p->addModel(modelUndular);
        p->addModel(modelBlank);
#undef modelBlank
        // NOTE disabled in Cardinal due to online requirement
        spl.removeModule("Arcane");
        spl.removeModule("Atout");
        spl.removeModule("Aleister");
    }
}

/*
static void initStatic__AS()
{
    Plugin* const p = new Plugin;
    pluginInstance__AS = p;

    const StaticPluginLoader spl(p, "AS");
    if (spl.ok())
    {
#define modelADSR modelASADSR
#define modelVCA modelASVCA
        //OSCILLATORS
        p->addModel(modelSineOsc);
        p->addModel(modelSawOsc);

        //TOOLS
        p->addModel(modelADSR);
        p->addModel(modelVCA);
        p->addModel(modelQuadVCA);
        p->addModel(modelTriLFO);
        p->addModel(modelAtNuVrTr);
        p->addModel(modelBPMClock);
        p->addModel(modelSEQ16);
        p->addModel(modelMixer2ch);
        p->addModel(modelMixer4ch);
        p->addModel(modelMixer8ch);
        p->addModel(modelMonoVUmeter);
        p->addModel(modelStereoVUmeter);
        p->addModel(modelMultiple2_5);
        p->addModel(modelMerge2_5);
        p->addModel(modelSteps);
        p->addModel(modelLaunchGate);
        p->addModel(modelKillGate);
        p->addModel(modelFlow);
        p->addModel(modelSignalDelay);
        p->addModel(modelTriggersMKI);
        p->addModel(modelTriggersMKII);
        p->addModel(modelTriggersMKIII);
        p->addModel(modelBPMCalc);
        p->addModel(modelBPMCalc2);
        p->addModel(modelCv2T);
        p->addModel(modelZeroCV2T);
        p->addModel(modelReScale);

        //EFFECTS
        p->addModel(modelDelayPlusFx);
        p->addModel(modelDelayPlusStereoFx);
        p->addModel(modelPhaserFx);
        p->addModel(modelReverbFx);
        p->addModel(modelReverbStereoFx);
        p->addModel(modelSuperDriveFx);
        p->addModel(modelSuperDriveStereoFx);
        p->addModel(modelTremoloFx);
        p->addModel(modelTremoloStereoFx);
        p->addModel(modelWaveShaper);
        p->addModel(modelWaveShaperStereo);

        //BLANK PANELS
        p->addModel(modelBlankPanel4);
        p->addModel(modelBlankPanel6);
        p->addModel(modelBlankPanel8);
        p->addModel(modelBlankPanelSpecial);
#undef modelADSR
#undef modelVCA
    }
}
*/

/*
static void initStatic__Atelier()
{
    Plugin* const p = new Plugin;
    pluginInstance__Atelier = p;

    const StaticPluginLoader spl(p, "Atelier");
    if (spl.ok())
    {
        p->addModel(modelPalette);
    }
}
*/

static void initStatic__AudibleInstruments()
{
    Plugin* const p = new Plugin;
    pluginInstance__AudibleInstruments = p;

    const StaticPluginLoader spl(p, "AudibleInstruments");
    if (spl.ok())
    {
        p->addModel(modelBraids);
        p->addModel(modelPlaits);
        p->addModel(modelElements);
        p->addModel(modelTides);
        p->addModel(modelTides2);
        p->addModel(modelClouds);
        p->addModel(modelWarps);
        p->addModel(modelRings);
        p->addModel(modelLinks);
        p->addModel(modelKinks);
        p->addModel(modelShades);
        p->addModel(modelBranches);
        p->addModel(modelBlinds);
        p->addModel(modelVeils);
        p->addModel(modelFrames);
        p->addModel(modelMarbles);
        p->addModel(modelStages);
        p->addModel(modelRipples);
        p->addModel(modelShelves);
        p->addModel(modelStreams);
    }
}

static void initStatic__Autinn()
{
    Plugin* const p = new Plugin;
    pluginInstance__Autinn = p;

    const StaticPluginLoader spl(p, "Autinn");
    if (spl.ok())
    {
#define modelChord modelAutinnChord
#define modelVibrato modelAutinnVibrato
        p->addModel(modelAmp);
        p->addModel(modelDeadband);
        p->addModel(modelBass);
        p->addModel(modelCVConverter);
        p->addModel(modelDC);
        p->addModel(modelDigi);
        p->addModel(modelFlopper);
        p->addModel(modelFlora);
        p->addModel(modelJette);
        p->addModel(modelBoomerang);
        p->addModel(modelOxcart);
        p->addModel(modelSaw);
        p->addModel(modelSjip);
        p->addModel(modelSquare);
        p->addModel(modelVibrato);
        p->addModel(modelVectorDriver);
        p->addModel(modelZod);
        p->addModel(modelTriBand);
        p->addModel(modelMixer6);
        p->addModel(modelNon);
        p->addModel(modelFil);
        p->addModel(modelNap);
        p->addModel(modelMelody);
        p->addModel(modelChord);
#undef modelChord
#undef modelVibrato
    }
}

static void initStatic__Bacon()
{
    Plugin* const p = new Plugin;
    pluginInstance__Bacon = p;

    const StaticPluginLoader spl(p, "BaconPlugs");
    if (spl.ok())
    {
        p->addModel(modelHarMoNee);
        p->addModel(modelGlissinator);
        p->addModel(modelPolyGnome);
        p->addModel(modelQuantEyes);
        p->addModel(modelSampleDelay);
#ifdef BUILD_SORTACHORUS
        p->addModel(modelSortaChorus);
#endif
        p->addModel(modelChipNoise);
        p->addModel(modelChipWaves);
        p->addModel(modelChipYourWave);
        p->addModel(modelOpen303);
#ifdef BUILD_GENERICLSFR
        p->addModel(modelGenericLFSR);
#endif
        p->addModel(modelKarplusStrongPoly);
        p->addModel(modelALingADing);
        p->addModel(modelBitulator);
#ifdef BUILD_PHASER
        p->addModel(modelPhaser);
#endif
        p->addModel(modelPolyGenerator);
    }
}

/*
static void initStatic__Befaco()
{
    Plugin* const p = new Plugin;
    pluginInstance__Befaco = p;

    const StaticPluginLoader spl(p, "Befaco");
    if (spl.ok())
    {
#define modelADSR modelBefacoADSR
#define modelMixer modelBefacoMixer
        p->addModel(modelEvenVCO);
        p->addModel(modelRampage);
        p->addModel(modelABC);
        p->addModel(modelSpringReverb);
        p->addModel(modelMixer);
        p->addModel(modelSlewLimiter);
        p->addModel(modelDualAtenuverter);
        p->addModel(modelPercall);
        p->addModel(modelHexmixVCA);
        p->addModel(modelChoppingKinky);
        p->addModel(modelKickall);
        p->addModel(modelSamplingModulator);
        p->addModel(modelMorphader);
        p->addModel(modelADSR);
        p->addModel(modelSTMix);
        p->addModel(modelMuxlicer);
        p->addModel(modelMex);
#undef modelADSR
#undef modelMixer
    }
}
*/

static void initStatic__Bidoo()
{
    Plugin* const p = new Plugin;
    pluginInstance__Bidoo = p;

    const StaticPluginLoader spl(p, "Bidoo");
    if (spl.ok())
    {
        p->addModel(modelTOCANTE);
        p->addModel(modelLATE);
        p->addModel(modelDIKTAT);
        p->addModel(modelDTROY);
        p->addModel(modelBORDL);
        p->addModel(modelZOUMAI);
        p->addModel(modelMU);
        p->addModel(modelCHUTE);
        p->addModel(modelLOURDE);
        p->addModel(modelDILEMO);
        p->addModel(modelLAMBDA);
        p->addModel(modelBANCAU);
        p->addModel(modelACNE);
        p->addModel(modelMS);
        p->addModel(modelDUKE);
        p->addModel(modelMOIRE);
        p->addModel(modelPILOT);
        p->addModel(modelHUITRE);
        p->addModel(modelOUAIVE);
        p->addModel(modelEDSAROS);
        p->addModel(modelPOUPRE);
        p->addModel(modelMAGMA);
        p->addModel(modelOAI);
        p->addModel(modelCANARD);
        p->addModel(modelEMILE);
        p->addModel(modelFORK);
        p->addModel(modelTIARE);
        p->addModel(modelLIMONADE);
        p->addModel(modelLIMBO);
        p->addModel(modelPERCO);
        p->addModel(modelBAFIS);
        p->addModel(modelBAR);
        p->addModel(modelMINIBAR);
        p->addModel(modelZINC);
        p->addModel(modelFREIN);
        p->addModel(modelHCTIP);
        p->addModel(modelSPORE);
        p->addModel(modelDFUZE);
        p->addModel(modelREI);
        p->addModel(modelRABBIT);
        p->addModel(modelBISTROT);
        p->addModel(modelSIGMA);
        p->addModel(modelFLAME);
        p->addModel(modelVOID);

        // NOTE disabled in Cardinal due to curl usage
        // p->addModel(modelANTN);
        spl.removeModule("antN");
    }
}

static void initStatic__BogaudioModules()
{
    Plugin* const p = new Plugin;
    pluginInstance__BogaudioModules = p;

    const StaticPluginLoader spl(p, "BogaudioModules");
    if (spl.ok())
    {
        // Make sure to use dark theme as default
        Skins& skins(Skins::skins());
        skins._default = "dark";
#define modelADSR modelBogaudioADSR
#define modelLFO modelBogaudioLFO
#define modelNoise modelBogaudioNoise
#define modelVCA modelBogaudioVCA
#define modelVCF modelBogaudioVCF
#define modelVCO modelBogaudioVCO
        p->addModel(modelVCO);
        p->addModel(modelLVCO);
        p->addModel(modelSine);
        p->addModel(modelPulse);
        p->addModel(modelXCO);
        p->addModel(modelAdditator);
        p->addModel(modelFMOp);
        p->addModel(modelChirp);
        p->addModel(modelLFO);
        p->addModel(modelLLFO);
        p->addModel(modelFourFO);
        p->addModel(modelEightFO);
        p->addModel(modelVCF);
        p->addModel(modelLVCF);
        p->addModel(modelFFB);
        p->addModel(modelEQ);
        p->addModel(modelEQS);
        p->addModel(modelLPG);
        p->addModel(modelLLPG);
        p->addModel(modelMegaGate);
        p->addModel(modelPEQ);
        p->addModel(modelPEQ6);
        p->addModel(modelPEQ6XF);
        p->addModel(modelPEQ14);
        p->addModel(modelPEQ14XF);
        p->addModel(modelDADSRH);
        p->addModel(modelDADSRHPlus);
        p->addModel(modelShaper);
        p->addModel(modelShaperPlus);
        p->addModel(modelAD);
        p->addModel(modelASR);
        p->addModel(modelADSR);
        p->addModel(modelVish);
        p->addModel(modelFollow);
        p->addModel(modelDGate);
        p->addModel(modelRGate);
        p->addModel(modelEdge);
        p->addModel(modelNoise);
        p->addModel(modelSampleHold);
        p->addModel(modelWalk2);
        p->addModel(modelWalk);
        p->addModel(modelMix8);
        p->addModel(modelMix8x);
        p->addModel(modelMix4);
        p->addModel(modelMix4x);
        p->addModel(modelMix2);
        p->addModel(modelMix1);
        p->addModel(modelVCM);
        p->addModel(modelMute8);
        p->addModel(modelPan);
        p->addModel(modelXFade);
        p->addModel(modelVCA);
        p->addModel(modelVCAmp);
        p->addModel(modelVelo);
        p->addModel(modelUMix);
        p->addModel(modelMumix);
        p->addModel(modelMatrix81);
        p->addModel(modelMatrix18);
        p->addModel(modelMatrix44);
        p->addModel(modelMatrix44Cvm);
        p->addModel(modelMatrix88);
        p->addModel(modelMatrix88Cv);
        p->addModel(modelMatrix88M);
        p->addModel(modelSwitch81);
        p->addModel(modelSwitch18);
        p->addModel(modelSwitch44);
        p->addModel(modelSwitch88);
        p->addModel(modelSwitch1616);
        p->addModel(modelAMRM);
        p->addModel(modelPressor);
        p->addModel(modelClpr);
        p->addModel(modelLmtr);
        p->addModel(modelNsgt);
        p->addModel(modelCmpDist);
        p->addModel(modelOneEight);
        p->addModel(modelEightOne);
        p->addModel(modelAddrSeq);
        p->addModel(modelAddrSeqX);
        p->addModel(modelPgmr);
        p->addModel(modelPgmrX);
        p->addModel(modelVU);
        p->addModel(modelAnalyzer);
        p->addModel(modelAnalyzerXL);
        p->addModel(modelRanalyzer);
        p->addModel(modelDetune);
        p->addModel(modelStack);
        p->addModel(modelReftone);
        p->addModel(modelMono);
        p->addModel(modelArp);
        p->addModel(modelAssign);
        p->addModel(modelUnison);
        p->addModel(modelPolyCon8);
        p->addModel(modelPolyCon16);
        p->addModel(modelPolyOff8);
        p->addModel(modelPolyOff16);
        p->addModel(modelPolyMult);
        p->addModel(modelBool);
        p->addModel(modelCmp);
        p->addModel(modelCVD);
        p->addModel(modelFlipFlop);
        p->addModel(modelInv);
        p->addModel(modelManual);
        p->addModel(modelFourMan);
        p->addModel(modelMult);
        p->addModel(modelOffset);
        p->addModel(modelSlew);
        p->addModel(modelSums);
        p->addModel(modelSwitch);
        p->addModel(modelLgsw);
        p->addModel(modelBlank3);
        p->addModel(modelBlank6);
#ifdef EXPERIMENTAL
        p->addModel(modelLag);
        p->addModel(modelPEQ14XR);
        p->addModel(modelPEQ14XV);
#endif
#ifdef TEST
        p->addModel(modelTest);
        p->addModel(modelTest2);
        p->addModel(modelTestExpanderBase);
        p->addModel(modelTestExpanderExtension);
        p->addModel(modelTestGl);
        p->addModel(modelTestVCF);
#endif
#undef modelADSR
#undef modelLFO
#undef modelNoise
#undef modelVCA
#undef modelVCF
#undef modelVCO
    }
}

static void initStatic__cf()
{
    Plugin* const p = new Plugin;
    pluginInstance__cf = p;

    const StaticPluginLoader spl(p, "cf");
    if (spl.ok())
    {
        p->addModel(modelMETRO);
        p->addModel(modelEACH);
        p->addModel(modeltrSEQ);
        p->addModel(modelLEDSEQ);
        p->addModel(modelL3DS3Q);
        p->addModel(modelSLIDERSEQ);
        p->addModel(modelPLAYER);
        p->addModel(modelPLAY);
        p->addModel(modelMONO);
        p->addModel(modelSTEREO);
        p->addModel(modelSUB);
        p->addModel(modelMASTER);
        p->addModel(modelVARIABLE);
        p->addModel(modelALGEBRA);
        p->addModel(modelFUNKTION);
        p->addModel(modelCHOKE);
        p->addModel(modelFOUR);
        p->addModel(modelSTEPS);
        p->addModel(modelPEAK);
        p->addModel(modelCUTS);
        p->addModel(modelBUFFER);
        p->addModel(modelDISTO);
        p->addModel(modelCUBE);
        p->addModel(modelPATCH);
        p->addModel(modelLABEL);
        p->addModel(modelDAVE);
    }
}

static void initStatic__ChowDSP()
{
    Plugin* const p = new Plugin;
    pluginInstance__ChowDSP = p;

    const StaticPluginLoader spl(p, "ChowDSP");
    if (spl.ok())
    {
        p->addModel(modelChowTape);
        p->addModel(modelChowPhaserFeedback);
        p->addModel(modelChowPhaserMod);
        p->addModel(modelChowFDN);
        p->addModel(modelChowRNN);
        p->addModel(modelChowModal);
        p->addModel(modelChowDer);
        p->addModel(modelWarp);
        p->addModel(modelWerner);
        p->addModel(modelChowPulse);
        p->addModel(modelChowTapeCompression);
        p->addModel(modelChowTapeChew);
        p->addModel(modelChowTapeDegrade);
        p->addModel(modelChowTapeLoss);
        p->addModel(modelChowChorus);

        // Credit crashes on save, see https://github.com/DISTRHO/Cardinal/issues/98
        // p->addModel(modelCredit);
        spl.removeModule("Credit");
    }
}

static void initStatic__DrumKit()
{
    Plugin* const p = new Plugin;
    pluginInstance__DrumKit = p;

    const StaticPluginLoader spl(p, "DrumKit");
    if (spl.ok())
    {
        setupSamples();
        p->addModel(modelBD9);
        p->addModel(modelSnare);
        p->addModel(modelClosedHH);
        p->addModel(modelOpenHH);
        p->addModel(modelDMX);
        p->addModel(modelCR78);
        p->addModel(modelSBD);
        p->addModel(modelGnome);
        p->addModel(modelSequencer);
        p->addModel(modelTomi);
        p->addModel(modelBaronial);
        p->addModel(modelMarionette);
    }
}

static void initStatic__ESeries()
{
    Plugin* const p = new Plugin;
    pluginInstance__ESeries = p;

    const StaticPluginLoader spl(p, "ESeries");
    if (spl.ok())
    {
        p->addModel(modelE340);
    }
}

static void initStatic__ExpertSleepersEncoders()
{
    Plugin* const p = new Plugin;
    pluginInstance__ExpertSleepersEncoders = p;

    const StaticPluginLoader spl(p, "ExpertSleepers-Encoders");
    if (spl.ok())
    {
        p->addModel(model8GT);
        p->addModel(model8CV);
        p->addModel(modelES40);
        p->addModel(modelES5);
        p->addModel(modelSMUX);
        p->addModel(modelCalibrator);
    }
}

static void initStatic__Extratone()
{
    Plugin* const p = new Plugin;
    pluginInstance__Extratone = p;

    const StaticPluginLoader spl(p, "Extratone");
    if (spl.ok())
    {
        p->addModel(modelModulo);
        p->addModel(modelMesoglea);
        p->addModel(modelMesoglea2);
        p->addModel(modelOpabinia);
        p->addModel(modelSplitterburst);
        p->addModel(modelPuzzlebox);
        p->addModel(modelDarwinism);
        // p->addModel(modelHalluciMemory);
        p->addModel(modelIchneumonid);
        p->addModel(modelMeganeura);
        p->addModel(modelPureneura);
        p->addModel(modelMesohyl);
        p->addModel(modelXtrtnBlank);
    }
}

static void initStatic__FehlerFabrik()
{
    Plugin* const p = new Plugin;
    pluginInstance__FehlerFabrik = p;

    const StaticPluginLoader spl(p, "FehlerFabrik");
    if (spl.ok())
    {
        p->addModel(modelPSIOP);
        p->addModel(modelPlanck);
        p->addModel(modelLuigi);
        p->addModel(modelAspect);
        p->addModel(modelMonte);
        p->addModel(modelArpanet);
        p->addModel(modelSigma);
        p->addModel(modelFax);
        p->addModel(modelRasoir);
        p->addModel(modelChi);
        p->addModel(modelNova);
        p->addModel(modelLilt);
        p->addModel(modelBotzinger);
    }
}

/*
static void initStatic__Fundamental()
{
    Plugin* const p = new Plugin;
    pluginInstance__Fundamental = p;

    const StaticPluginLoader spl(p, "Fundamental");
    if (spl.ok())
    {
        p->addModel(modelVCO);
        p->addModel(modelVCO2);
        p->addModel(modelVCF);
        p->addModel(modelVCA_1);
        p->addModel(modelVCA);
        p->addModel(modelLFO);
        p->addModel(modelLFO2);
        p->addModel(modelDelay);
        p->addModel(modelADSR);
        p->addModel(modelMixer);
        p->addModel(modelVCMixer);
        p->addModel(model_8vert);
        p->addModel(modelUnity);
        p->addModel(modelMutes);
        p->addModel(modelPulses);
        p->addModel(modelScope);
        p->addModel(modelSEQ3);
        p->addModel(modelSequentialSwitch1);
        p->addModel(modelSequentialSwitch2);
        p->addModel(modelOctave);
        p->addModel(modelQuantizer);
        p->addModel(modelSplit);
        p->addModel(modelMerge);
        p->addModel(modelSum);
        p->addModel(modelViz);
        p->addModel(modelMidSide);
        p->addModel(modelNoise);
        p->addModel(modelRandom);

        // show all plugins, helping those familiar with v1 Rack modules
        if (json_t* const modules = json_object_get(spl.rootJ, "modules"))
        {
            size_t i;
            json_t* v;
            json_array_foreach(modules, i, v)
            {
                json_object_set(v, "hidden", json_false());
            }
        }
    }
}
*/

static void initStatic__GlueTheGiant()
{
    Plugin* const p = new Plugin;
    pluginInstance__GlueTheGiant = p;

    const StaticPluginLoader spl(p, "GlueTheGiant");
    if (spl.ok())
    {
        p->addModel(modelGigBus);
        p->addModel(modelMiniBus);
        p->addModel(modelSchoolBus);
        p->addModel(modelMetroCityBus);
        p->addModel(modelBusDepot);
        p->addModel(modelBusRoute);
        p->addModel(modelRoad);
        p->addModel(modelEnterBus);
        p->addModel(modelExitBus);
    }
}

static void initStatic__GrandeModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__GrandeModular = p;

    const StaticPluginLoader spl(p, "GrandeModular");
    if (spl.ok())
    {
        p->addModel(modelClip);
        p->addModel(modelMerge8);
        p->addModel(modelMergeSplit4);
        p->addModel(modelMicrotonalChords);
        p->addModel(modelMicrotonalNotes);
        p->addModel(modelNoteMT);
        p->addModel(modelPolyMergeResplit);
        p->addModel(modelPolySplit);
        p->addModel(modelQuant);
        p->addModel(modelQuantIntervals);
        p->addModel(modelQuantMT);
        p->addModel(modelSampleDelays);
        p->addModel(modelScale);
        p->addModel(modelSplit8);
        p->addModel(modelTails);
        p->addModel(modelVarSampleDelays);
    }
}

static void initStatic__HetrickCV()
{
    Plugin* const p = new Plugin;
    pluginInstance__HetrickCV = p;

    const StaticPluginLoader spl(p, "HetrickCV");
    if (spl.ok())
    {
#define modelASR modelHetrickCVASR
#define modelBlankPanel modelHetrickCVBlankPanel
#define modelFlipFlop modelHetrickCVFlipFlop
#define modelMidSide modelHetrickCVMidSide
#define modelMinMax modelHetrickCVMinMax
        p->addModel(modelTwoToFour);
        p->addModel(modelAnalogToDigital);
        p->addModel(modelASR);
        p->addModel(modelBinaryGate);
        p->addModel(modelBinaryNoise);
        p->addModel(modelBitshift);
        p->addModel(modelBlankPanel);
        p->addModel(modelBoolean3);
        p->addModel(modelChaos1Op);
        p->addModel(modelChaos2Op);
        p->addModel(modelChaos3Op);
        p->addModel(modelChaoticAttractors);
        p->addModel(modelClockedNoise);
        p->addModel(modelComparator);
        p->addModel(modelContrast);
        p->addModel(modelCrackle);
        p->addModel(modelDataCompander);
        p->addModel(modelDelta);
        p->addModel(modelDigitalToAnalog);
        p->addModel(modelDust);
        p->addModel(modelExponent);
        p->addModel(modelFBSineChaos);
        p->addModel(modelFlipFlop);
        p->addModel(modelFlipPan);
        p->addModel(modelGateJunction);
        p->addModel(modelGingerbread);
        p->addModel(modelLogicCombine);
        p->addModel(modelMidSide);
        p->addModel(modelMinMax);
        p->addModel(modelRandomGates);
        p->addModel(modelRotator);
        p->addModel(modelRungler);
        p->addModel(modelScanner);
        p->addModel(modelWaveshape);
        p->addModel(modelXYToPolar);
#undef modelASR
#undef modelBlankPanel
#undef modelFlipFlop
#undef modelMidSide
#undef modelMinMax
    }
}

static void initStatic__ImpromptuModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__ImpromptuModular = p;

    const StaticPluginLoader spl(p, "ImpromptuModular");
    if (spl.ok())
    {
        p->addModel(modelAdaptiveQuantizer);
        p->addModel(modelBigButtonSeq);
        p->addModel(modelBigButtonSeq2);
        p->addModel(modelChordKey);
        p->addModel(modelChordKeyExpander);
        p->addModel(modelClocked);
        p->addModel(modelClockedExpander);
        p->addModel(modelClkd);
        p->addModel(modelCvPad);
        p->addModel(modelFoundry);
        p->addModel(modelFoundryExpander);
        p->addModel(modelFourView);
        p->addModel(modelGateSeq64);
        p->addModel(modelGateSeq64Expander);
        p->addModel(modelHotkey);
        p->addModel(modelPart);
        p->addModel(modelPhraseSeq16);
        p->addModel(modelPhraseSeq32);
        p->addModel(modelPhraseSeqExpander);
        p->addModel(modelProbKey);
        // p->addModel(modelProbKeyExpander);
        p->addModel(modelSemiModularSynth);
        p->addModel(modelTact);
        p->addModel(modelTact1);
        p->addModel(modelTactG);
        p->addModel(modelTwelveKey);
        p->addModel(modelWriteSeq32);
        p->addModel(modelWriteSeq64);
        p->addModel(modelBlankPanel);
    }
}

/*
static void initStatic__ihtsyn()
{
    Plugin* const p = new Plugin;
    pluginInstance__ihtsyn = p;

    const StaticPluginLoader spl(p, "ihtsyn");
    if (spl.ok())
    {
        p->addModel(modelPitchMangler);
        p->addModel(modelTwistedVerb);
        p->addModel(modelHiVerb);
        p->addModel(modelMVerb);
    }
}
*/

static void initStatic__JW()
{
    Plugin* const p = new Plugin;
    pluginInstance__JW = p;

    const StaticPluginLoader spl(p, "JW-Modules");
    if (spl.ok())
    {
#define modelQuantizer modelJWQuantizer
        p->addModel(modelAdd5);
        p->addModel(modelBouncyBalls);
        p->addModel(modelCat);
        p->addModel(modelTree);
        p->addModel(modelFullScope);
        p->addModel(modelGridSeq);
        p->addModel(modelEightSeq);
        p->addModel(modelDivSeq);
        p->addModel(modelMinMax);
        p->addModel(modelNoteSeq);
        p->addModel(modelNoteSeqFu);
        p->addModel(modelNoteSeq16);
        p->addModel(modelTrigs);
        p->addModel(modelOnePattern);
        p->addModel(modelPatterns);
        p->addModel(modelQuantizer);
        p->addModel(modelSimpleClock);
        p->addModel(modelD1v1de);
        p->addModel(modelPres1t);
        p->addModel(modelThingThing);
        p->addModel(modelWavHead);
        p->addModel(modelXYPad);
        p->addModel(modelBlankPanel1hp);
        p->addModel(modelBlankPanelSmall);
        p->addModel(modelBlankPanelMedium);
        p->addModel(modelBlankPanelLarge);
        p->addModel(modelCoolBreeze);
        p->addModel(modelPete);
       #ifndef STATIC_BUILD
        p->addModel(modelStr1ker);
       #else
        spl.removeModule("Str1ker");
       #endif
#undef modelQuantizer
    }
}

static void initStatic__LifeFormModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__LifeFormModular= p;

    const StaticPluginLoader spl(p, "LifeFormModular");
    if (spl.ok())
    {
        p->addModel(modelTimeDiktat);
        p->addModel(modelSequenceModeler);
        p->addModel(modelPitchDiktat);
        p->addModel(modelPitchIntegrator);
        p->addModel(modelBurstIntegrator);
        p->addModel(modelQuadModulator);
        p->addModel(modelImpulseControl);
        p->addModel(modelQuadSteppedOffset);
        p->addModel(modelPercussiveVibration);
        p->addModel(modelQuadUtility);
        p->addModel(modelAdditiveVibration);
        p->addModel(modelComplexOsc);
        p->addModel(modelDriftgen);
    }
}

static void initStatic__LittleUtils()
{
    Plugin* const p = new Plugin;
    pluginInstance__LittleUtils = p;

    const StaticPluginLoader spl(p, "LittleUtils");
    if (spl.ok())
    {
        p->addModel(modelButtonModule);
        p->addModel(modelPulseGenerator);
        p->addModel(modelBias_Semitone);
        p->addModel(modelMulDiv);
        p->addModel(modelTeleportInModule);
        p->addModel(modelTeleportOutModule);
    }
}

static void initStatic__Lomas()
{
    Plugin* const p = new Plugin;
    pluginInstance__Lomas = p;

    const StaticPluginLoader spl(p, "LomasModules");
    if (spl.ok())
    {
        p->addModel(modelAdvancedSampler);
        p->addModel(modelGateSequencer);
    }
}

static void initStatic__Lyrae()
{
    Plugin* const p = new Plugin;
    pluginInstance__Lyrae = p;

    const StaticPluginLoader spl(p, "LyraeModules");
    if (spl.ok())
    {
#define modelDelta modelLyraeDelta
        p->addModel(modelSulafat);
        p->addModel(modelGamma);
        p->addModel(modelDelta);
        p->addModel(modelVega);
        p->addModel(modelBD383238);
        p->addModel(modelZeta);
#undef modelDelta
    }
}

static void initStatic__MindMeld()
{
    Plugin* const p = new Plugin;
    pluginInstance__MindMeld = p;

    const StaticPluginLoader spl(p, "MindMeldModular");
    if (spl.ok())
    {
        p->addModel(modelMixMasterJr);
        p->addModel(modelAuxExpanderJr);
        p->addModel(modelMixMaster);
        p->addModel(modelAuxExpander);
        p->addModel(modelMeld);
        p->addModel(modelUnmeld);
        p->addModel(modelMSMelder);
        p->addModel(modelEqMaster);
        p->addModel(modelEqExpander);
        p->addModel(modelBassMaster);
        p->addModel(modelBassMasterJr);
        p->addModel(modelShapeMaster);
    }
}

static void initStatic__Mog()
{
    Plugin* const p = new Plugin;
    pluginInstance__Mog = p;

    const StaticPluginLoader spl(p, "Mog");
    if (spl.ok())
    {
        p->addModel(modelNetwork);
        p->addModel(modelNexus);
    }
}

static void initStatic__mscHack()
{
    Plugin* const p = new Plugin;
    pluginInstance__mscHack = p;

    const StaticPluginLoader spl(p, "mscHack");
    if (spl.ok())
    {
        p->addModel(modelCompressor);
        p->addModel(modelSynthDrums);
        p->addModel(modelSEQ_6x32x16);
        p->addModel(modelMasterClockx4);
        //p->addModel(modelMasterClockx8);
        p->addModel(modelSEQ_Envelope_8);
        p->addModel(modelSeq_Triad2);
        p->addModel(modelARP700);
        p->addModel(modelMix_4_0_4);
        p->addModel(modelMix_9_3_4);
        p->addModel(modelMix_16_4_4);
        p->addModel(modelMix_24_4_4);
        p->addModel(modelASAF8);
        p->addModel(modelPingPong);
        p->addModel(modelStepDelay);
        p->addModel(modelOsc_3Ch);
        p->addModel(modelDronez);
        p->addModel(modelMorze);
        p->addModel(modelWindz);
        p->addModel(modelLorenz);
        p->addModel(modelAlienz);
        p->addModel(modelOSC_WaveMorph_3);
        p->addModel(modelMaude_221);
    }
}

static void initStatic__Prism()
{
    Plugin* const p = new Plugin;
    pluginInstance__Prism = p;

    const StaticPluginLoader spl(p, "Prism");
    if (spl.ok())
    {
        p->addModel(modelRainbow);
        p->addModel(modelRainbowScaleExpander);
        p->addModel(modelDroplet);
    }
}

static void initStatic__rackwindows()
{
    Plugin* const p = new Plugin;
    pluginInstance__rackwindows = p;

    const StaticPluginLoader spl(p, "rackwindows");
    if (spl.ok())
    {
        // p->addModel(modelAcceleration);
        p->addModel(modelBitshiftgain);
        p->addModel(modelCapacitor);
        p->addModel(modelCapacitor_stereo);
        p->addModel(modelChorus);
        p->addModel(modelConsole);
        p->addModel(modelConsole_mm);
        p->addModel(modelDistance);
        p->addModel(modelGolem);
        p->addModel(modelHolt);
        p->addModel(modelHombre);
        p->addModel(modelInterstage);
        p->addModel(modelMonitoring);
        p->addModel(modelMv);
        p->addModel(modelRasp);
        p->addModel(modelReseq);
        p->addModel(modelTape);
        p->addModel(modelTremolo);
        p->addModel(modelVibrato);
    }
}

static void initStatic__repelzen()
{
    Plugin* const p = new Plugin;
    pluginInstance__repelzen = p;

    const StaticPluginLoader spl(p, "repelzen");
    if (spl.ok())
    {
#define modelBlank modelrepelzenBlank
#define modelMixer modelrepelzenMixer
#define modelWerner modelrepelzenWerner
        p->addModel(modelBlank);
        p->addModel(modelBurst);
        p->addModel(modelFolder);
        p->addModel(modelErwin);
        p->addModel(modelWerner);
        p->addModel(modelMixer);
#undef modelBlank
#undef modelMixer
#undef modelWerner
    }
}

static void initStatic__sonusmodular()
{
    Plugin* const p = new Plugin;
    pluginInstance__sonusmodular = p;

    const StaticPluginLoader spl(p, "sonusmodular");
    if (spl.ok())
    {
        p->addModel(modelAddiction);
        p->addModel(modelBitter);
        p->addModel(modelBymidside);
        p->addModel(modelCampione);
        p->addModel(modelChainsaw);
        p->addModel(modelCtrl);
        p->addModel(modelDeathcrush);
        p->addModel(modelFraction);
        p->addModel(modelHarmony);
        p->addModel(modelLadrone);
        p->addModel(modelLuppolo);
        p->addModel(modelLuppolo3);
        p->addModel(modelMicromacro);
        p->addModel(modelMrcheb);
        p->addModel(modelMultimulti);
        p->addModel(modelNeurosc);
        p->addModel(modelOktagon);
        p->addModel(modelOsculum);
        p->addModel(modelParamath);
        p->addModel(modelPiconoise);
        p->addModel(modelPith);
        p->addModel(modelPusher);
        p->addModel(modelRingo);
        p->addModel(modelScramblase);
        p->addModel(modelTropicana);
        p->addModel(modelTwoff);
        p->addModel(modelYabp);
    }
}

static void initStatic__ValleyAudio()
{
    Plugin* const p = new Plugin;
    pluginInstance__ValleyAudio = p;

    const StaticPluginLoader spl(p, "ValleyAudio");
    if (spl.ok())
    {
        p->addModel(modelTopograph);
        p->addModel(modelUGraph);
        p->addModel(modelDexter);
        p->addModel(modelPlateau);
        p->addModel(modelInterzone);
        p->addModel(modelAmalgam);
        p->addModel(modelFeline);
        p->addModel(modelTerrorform);
    }
}

static void initStatic__ZetaCarinaeModules()
{
    Plugin* p = new Plugin;
    pluginInstance__ZetaCarinaeModules = p;

    const StaticPluginLoader spl(p, "ZetaCarinaeModules");
    if (spl.ok())
    {
        p->addModel(modelBrownianBridge);
        p->addModel(modelOrnsteinUhlenbeck);
        p->addModel(modelIOU);
        p->addModel(modelWarbler);
        p->addModel(modelRosenchance);
        p->addModel(modelGuildensTurn);
        p->addModel(modelRosslerRustler);
        p->addModel(modelFirefly);
    }
}

/*
static void initStatic__ZZC()
{
    Plugin* p = new Plugin;
    pluginInstance__ZZC = p;

    const StaticPluginLoader spl(p, "ZZC");
    if (spl.ok())
    {
#define modelClock modelZZCClock
        p->addModel(modelClock);
        p->addModel(modelDivider);
        p->addModel(modelFN3);
        p->addModel(modelSCVCA);
        p->addModel(modelSH8);
        p->addModel(modelSRC);
        p->addModel(modelDiv);
        p->addModel(modelDivExp);
        p->addModel(modelPolygate);
#undef modelClock
    }
}
*/
#endif // NOPLUGINS

void initStaticPlugins()
{
    initStatic__Cardinal();
#ifndef NOPLUGINS
    initStatic__21kHz();
    initStatic__AmalgamatedHarmonics();
    initStatic__AnimatedCircuits();
    initStatic__Aria();
    // initStatic__AS();
    // initStatic__Atelier();
    initStatic__AudibleInstruments();
    initStatic__Autinn();
    initStatic__Bacon();
    // initStatic__Befaco();
    initStatic__Bidoo();
    initStatic__BogaudioModules();
    initStatic__cf();
    initStatic__ChowDSP();
    initStatic__DrumKit();
    initStatic__ESeries();
    initStatic__ExpertSleepersEncoders();
    initStatic__Extratone();
    initStatic__FehlerFabrik();
    // initStatic__Fundamental();
    initStatic__GlueTheGiant();
    initStatic__GrandeModular();
    initStatic__HetrickCV();
    initStatic__ImpromptuModular();
    // initStatic__ihtsyn();
    initStatic__JW();
    initStatic__LifeFormModular();
    initStatic__LittleUtils();
    initStatic__Lomas();
    initStatic__Lyrae();
    initStatic__MindMeld();
    initStatic__Mog();
    initStatic__mscHack();
    initStatic__Prism();
    initStatic__rackwindows();
    initStatic__repelzen();
    initStatic__sonusmodular();
    initStatic__ValleyAudio();
    initStatic__ZetaCarinaeModules();
    // initStatic__ZZC();
#endif // NOPLUGINS
}

void destroyStaticPlugins()
{
    for (Plugin* p : plugins)
        delete p;
    plugins.clear();
}

}
}
