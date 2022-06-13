#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p)
{
    pluginInstance = p;

    // Add modules here
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

    // Any other plugin initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

/* Other stuff */

/* #console type (Console, Console MM)
======================================================================================== */
void saveConsoleType(int consoleType)
{
    json_t* settingsJ = json_object();
    json_object_set_new(settingsJ, "consoleType", json_boolean(consoleType));
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "w");
    if (file) {
        json_dumpf(settingsJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(settingsJ);
}

int loadConsoleType()
{
    bool ret = false;
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "r");
    if (!file) {
        saveConsoleType(false);
        return ret;
    }
    json_error_t error;
    json_t* settingsJ = json_loadf(file, 0, &error);
    if (!settingsJ) {
        // invalid setting json file
        fclose(file);
        saveConsoleType(false);
        return ret;
    }
    json_t* consoleTypeJ = json_object_get(settingsJ, "consoleType");
    if (consoleTypeJ)
        ret = json_boolean_value(consoleTypeJ);

    fclose(file);
    json_decref(settingsJ);
    return ret;
}

/* #slew type (Rasp)
======================================================================================== */
void saveSlewType(int slewType)
{
    json_t* settingsJ = json_object();
    json_object_set_new(settingsJ, "slewType", json_integer(slewType));
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "w");
    if (file) {
        json_dumpf(settingsJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(settingsJ);
}

int loadSlewType()
{
    bool ret = false;
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "r");
    if (!file) {
        saveSlewType(false);
        return ret;
    }
    json_error_t error;
    json_t* settingsJ = json_loadf(file, 0, &error);
    if (!settingsJ) {
        // invalid setting json file
        fclose(file);
        saveSlewType(false);
        return ret;
    }
    json_t* slewTypeJ = json_object_get(settingsJ, "slewType");
    if (slewTypeJ)
        ret = json_integer_value(slewTypeJ);

    fclose(file);
    json_decref(settingsJ);
    return ret;
}

/* #direct output mode (Console MM)
======================================================================================== */
void saveDirectOutMode(bool directOutMode)
{
    json_t* settingsJ = json_object();
    json_object_set_new(settingsJ, "directOutMode", json_boolean(directOutMode));
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "w");
    if (file) {
        json_dumpf(settingsJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(settingsJ);
}

int loadDirectOutMode()
{
    bool ret = false;
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "r");
    if (!file) {
        saveDirectOutMode(false);
        return ret;
    }
    json_error_t error;
    json_t* settingsJ = json_loadf(file, 0, &error);
    if (!settingsJ) {
        // invalid setting json file
        fclose(file);
        saveDirectOutMode(false);
        return ret;
    }
    json_t* directOutModeJ = json_object_get(settingsJ, "directOutMode");
    if (directOutModeJ)
        ret = json_boolean_value(directOutModeJ);

    fclose(file);
    json_decref(settingsJ);
    return ret;
}

/* #delay mode (Golem)
======================================================================================== */
void saveDelayMode(int delayMode)
{
    json_t* settingsJ = json_object();
    json_object_set_new(settingsJ, "delayMode", json_boolean(delayMode));
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "w");
    if (file) {
        json_dumpf(settingsJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(settingsJ);
}

int loadDelayMode()
{
    bool ret = false;
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "r");
    if (!file) {
        saveDelayMode(false);
        return ret;
    }
    json_error_t error;
    json_t* settingsJ = json_loadf(file, 0, &error);
    if (!settingsJ) {
        // invalid setting json file
        fclose(file);
        saveDelayMode(false);
        return ret;
    }
    json_t* delayModeJ = json_object_get(settingsJ, "delayMode");
    if (delayModeJ)
        ret = json_boolean_value(delayModeJ);

    fclose(file);
    json_decref(settingsJ);
    return ret;
}

/* #themes
======================================================================================== */
// https://github.com/MarcBoule/Geodesics/blob/master/src/Geodesics.cpp
void saveDarkAsDefault(bool darkAsDefault)
{
    json_t* settingsJ = json_object();
    json_object_set_new(settingsJ, "darkAsDefault", json_boolean(darkAsDefault));
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "w");
    if (file) {
        json_dumpf(settingsJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(settingsJ);
}

// https://github.com/MarcBoule/Geodesics/blob/master/src/Geodesics.cpp
bool loadDarkAsDefault()
{
    bool ret = false;
    std::string settingsFilename = asset::user("Rackwindows.json");
    FILE* file = fopen(settingsFilename.c_str(), "r");
    if (!file) {
        saveDarkAsDefault(false);
        return ret;
    }
    json_error_t error;
    json_t* settingsJ = json_loadf(file, 0, &error);
    if (!settingsJ) {
        // invalid setting json file
        fclose(file);
        saveDarkAsDefault(false);
        return ret;
    }
    json_t* darkAsDefaultJ = json_object_get(settingsJ, "darkAsDefault");
    if (darkAsDefaultJ)
        ret = json_boolean_value(darkAsDefaultJ);

    fclose(file);
    json_decref(settingsJ);
    return ret;
}

// https://github.com/ValleyAudio/ValleyRackFree/blob/v1.0/src/Common/DSP/NonLinear.hpp
inline float tanhDriveSignal(float x, float drive)
{
    x *= drive;

    if (x < -1.3f) {
        return -1.f;
    } else if (x < -0.75f) {
        return (x * x + 2.6f * x + 1.69f) * 0.833333f - 1.f;
    } else if (x > 1.3f) {
        return 1.f;
    } else if (x > 0.75f) {
        return 1.f - (x * x - 2.6f * x + 1.69f) * 0.833333f;
    }
    return x;
}
