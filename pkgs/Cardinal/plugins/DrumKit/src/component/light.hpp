#pragma once

#include "shadow.hpp"

struct DKLEDBezel : LEDBezel {
private:
  DKShadow shadow = DKShadow();

public:
  void setSvg(std::shared_ptr<Svg> svg) {
    shadow.setBox(box);
  }

  void draw(const DrawArgs &args) override {
    /** shadow */
    shadow.draw(args.vg);

    LEDBezel::draw(args);
  }
};

template <typename BASE>
struct DKButtonLight : BASE {
  DKButtonLight() {
    this->box.size = mm2px(Vec(6.0, 6.0));
  }
};
