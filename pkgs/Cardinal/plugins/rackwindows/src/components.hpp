#pragma once
#include "rack.hpp"

using namespace rack;
extern Plugin* pluginInstance;

// knobs
struct RwKnobLarge : app::SvgKnob {
    RwKnobLarge()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_large.svg")));
    }
};
struct RwKnobLargeDark : app::SvgKnob {
    RwKnobLargeDark()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0.1;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_large_dark.svg")));
    }
};
struct RwKnobMedium : app::SvgKnob {
    RwKnobMedium()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_medium.svg")));
    }
};
struct RwKnobMediumDark : app::SvgKnob {
    RwKnobMediumDark()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0.1;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_medium_dark.svg")));
    }
};
struct RwKnobSmall : app::SvgKnob {
    RwKnobSmall()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_small.svg")));
    }
};
struct RwKnobSmallDark : app::SvgKnob {
    RwKnobSmallDark()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_small_dark.svg")));
    }
};
struct RwKnobTrimpot : app::SvgKnob {
    RwKnobTrimpot()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0.05;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_knob_trimpot.svg")));
    }
};

// snap knobs
struct RwSwitchKnobLarge : RwKnobLarge {
    RwSwitchKnobLarge()
    {
        snap = true;
    }
};
struct RwSwitchKnobMedium : RwKnobMedium {
    RwSwitchKnobMedium()
    {
        snap = true;
    }
};
struct RwSwitchKnobMediumDark : RwKnobMediumDark {
    RwSwitchKnobMediumDark()
    {
        minAngle = -0.8 * M_PI;
        maxAngle = 0.8 * M_PI;
        snap = true;
    }
};
struct RwSwitchKnobMediumDarkOneThird : RwKnobMediumDark {
    RwSwitchKnobMediumDarkOneThird()
    {
        minAngle = -0.33 * M_PI;
        maxAngle = 0.33 * M_PI;
        snap = true;
    }
};
struct RwSwitchKnobMediumDarkTwoThirds : RwKnobMediumDark {
    RwSwitchKnobMediumDarkTwoThirds()
    {
        minAngle = -0.74 * M_PI;
        maxAngle = 0.74 * M_PI;
        snap = true;
    }
};
struct RwSwitchKnobMediumSix : RwKnobMediumDark {
    RwSwitchKnobMediumSix()
    {
        minAngle = -0.72 * M_PI;
        maxAngle = 0.72 * M_PI;
        snap = true;
    }
};
struct RwSwitchKnobSmall : RwKnobSmall {
    RwSwitchKnobSmall()
    {
        snap = true;
    }
};
struct RwSwitchKnobSmallDark : RwKnobSmallDark {
    RwSwitchKnobSmallDark()
    {
        snap = true;
    }
};

// switches
struct RwSwitchThree : SvgSwitch {
    RwSwitchThree()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_2.svg")));
    }
};
struct RwSwitchThreeVert : SvgSwitch {
    RwSwitchThreeVert()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_vert_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_vert_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_switch_three_vert_2.svg")));
    }
};
struct RwCKSS : SvgSwitch {
    RwCKSS()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_CKSS_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_CKSS_1.svg")));
    }
};
struct RwCKSSRot : SvgSwitch {
    RwCKSSRot()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_CKSS_rot_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_CKSS_rot_1.svg")));
    }
};

// ports
struct RwPJ301MPort : app::SvgPort {
    RwPJ301MPort()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_PJ301M.svg")));
    }
};
struct RwPJ301MPortSilver : app::SvgPort {
    RwPJ301MPortSilver()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rw_PJ301M_silver.svg")));
    }
};