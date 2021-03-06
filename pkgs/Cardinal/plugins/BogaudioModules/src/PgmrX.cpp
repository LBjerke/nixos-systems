
#include "PgmrX.hpp"

void PgmrX::processAlways(const ProcessArgs& args) {
	int position = 0;
	int baseID = 0;
	if (baseConnected()) {
		PgmrExpanderMessage* bm = fromBase();
		baseID = bm->baseID;
		position = bm->position;
		_rangeOffset = bm->rangeOffset;
		_rangeScale = bm->rangeScale;
	}
	setBaseIDAndPosition(baseID, position);
	if (_position < 1) {
		for (int i = 0; i < 4; ++i) {
			_localElements[i]->selectedLight.value = 0.0f;
		}
	}

	if (expanderConnected()) {
		PgmrExpanderMessage* te = toExpander();
		te->baseID = _baseID;
		te->position = _position > 0 ? _position + 1 : 0;
		te->rangeOffset = _rangeOffset;
		te->rangeScale = _rangeScale;
	}
}

struct PgmrXWidget : BGModuleWidget {
	static constexpr int hp = 12;

	PgmrXWidget(PgmrX* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PgmrX");
		createScrews();

		// generated by svg_widgets.rb
		auto cva1ParamPosition = Vec(9.5, 40.5);
		auto cvb1ParamPosition = Vec(9.5, 94.5);
		auto cvc1ParamPosition = Vec(9.5, 148.5);
		auto cvd1ParamPosition = Vec(9.5, 202.5);
		auto select1ParamPosition = Vec(13.5, 267.0);
		auto cva2ParamPosition = Vec(54.5, 40.5);
		auto cvb2ParamPosition = Vec(54.5, 94.5);
		auto cvc2ParamPosition = Vec(54.5, 148.5);
		auto cvd2ParamPosition = Vec(54.5, 202.5);
		auto select2ParamPosition = Vec(58.5, 267.0);
		auto cva3ParamPosition = Vec(99.5, 40.5);
		auto cvb3ParamPosition = Vec(99.5, 94.5);
		auto cvc3ParamPosition = Vec(99.5, 148.5);
		auto cvd3ParamPosition = Vec(99.5, 202.5);
		auto select3ParamPosition = Vec(103.5, 267.0);
		auto cva4ParamPosition = Vec(144.5, 40.5);
		auto cvb4ParamPosition = Vec(144.5, 94.5);
		auto cvc4ParamPosition = Vec(144.5, 148.5);
		auto cvd4ParamPosition = Vec(144.5, 202.5);
		auto select4ParamPosition = Vec(148.5, 267.0);

		auto select1InputPosition = Vec(10.5, 290.0);
		auto select2InputPosition = Vec(55.5, 290.0);
		auto select3InputPosition = Vec(100.5, 290.0);
		auto select4InputPosition = Vec(145.5, 290.0);

		auto select1OutputPosition = Vec(10.5, 330.0);
		auto select2OutputPosition = Vec(55.5, 330.0);
		auto select3OutputPosition = Vec(100.5, 330.0);
		auto select4OutputPosition = Vec(145.5, 330.0);

		auto select1LightPosition = Vec(19.3, 255.0);
		auto select2LightPosition = Vec(64.3, 255.0);
		auto select3LightPosition = Vec(109.3, 255.0);
		auto select4LightPosition = Vec(154.3, 255.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(cva1ParamPosition, module, PgmrX::CVA1_PARAM));
		addParam(createParam<Knob26>(cvb1ParamPosition, module, PgmrX::CVB1_PARAM));
		addParam(createParam<Knob26>(cvc1ParamPosition, module, PgmrX::CVC1_PARAM));
		addParam(createParam<Knob26>(cvd1ParamPosition, module, PgmrX::CVD1_PARAM));
		addParam(createParam<Button18>(select1ParamPosition, module, PgmrX::SELECT1_PARAM));
		addParam(createParam<Knob26>(cva2ParamPosition, module, PgmrX::CVA2_PARAM));
		addParam(createParam<Knob26>(cvb2ParamPosition, module, PgmrX::CVB2_PARAM));
		addParam(createParam<Knob26>(cvc2ParamPosition, module, PgmrX::CVC2_PARAM));
		addParam(createParam<Knob26>(cvd2ParamPosition, module, PgmrX::CVD2_PARAM));
		addParam(createParam<Button18>(select2ParamPosition, module, PgmrX::SELECT2_PARAM));
		addParam(createParam<Knob26>(cva3ParamPosition, module, PgmrX::CVA3_PARAM));
		addParam(createParam<Knob26>(cvb3ParamPosition, module, PgmrX::CVB3_PARAM));
		addParam(createParam<Knob26>(cvc3ParamPosition, module, PgmrX::CVC3_PARAM));
		addParam(createParam<Knob26>(cvd3ParamPosition, module, PgmrX::CVD3_PARAM));
		addParam(createParam<Button18>(select3ParamPosition, module, PgmrX::SELECT3_PARAM));
		addParam(createParam<Knob26>(cva4ParamPosition, module, PgmrX::CVA4_PARAM));
		addParam(createParam<Knob26>(cvb4ParamPosition, module, PgmrX::CVB4_PARAM));
		addParam(createParam<Knob26>(cvc4ParamPosition, module, PgmrX::CVC4_PARAM));
		addParam(createParam<Knob26>(cvd4ParamPosition, module, PgmrX::CVD4_PARAM));
		addParam(createParam<Button18>(select4ParamPosition, module, PgmrX::SELECT4_PARAM));

		addInput(createInput<Port24>(select1InputPosition, module, PgmrX::SELECT1_INPUT));
		addInput(createInput<Port24>(select2InputPosition, module, PgmrX::SELECT2_INPUT));
		addInput(createInput<Port24>(select3InputPosition, module, PgmrX::SELECT3_INPUT));
		addInput(createInput<Port24>(select4InputPosition, module, PgmrX::SELECT4_INPUT));

		addOutput(createOutput<Port24>(select1OutputPosition, module, PgmrX::SELECT1_OUTPUT));
		addOutput(createOutput<Port24>(select2OutputPosition, module, PgmrX::SELECT2_OUTPUT));
		addOutput(createOutput<Port24>(select3OutputPosition, module, PgmrX::SELECT3_OUTPUT));
		addOutput(createOutput<Port24>(select4OutputPosition, module, PgmrX::SELECT4_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(select1LightPosition, module, PgmrX::SELECT1_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select2LightPosition, module, PgmrX::SELECT2_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select3LightPosition, module, PgmrX::SELECT3_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select4LightPosition, module, PgmrX::SELECT4_LIGHT));
	}
};

Model* modelPgmrX = createModel<PgmrX, PgmrXWidget>("Bogaudio-PgmrX", "PGMRX", "4-step chainable expander for PGMR", "Sequencer", "Expander", "Polyphonic");
