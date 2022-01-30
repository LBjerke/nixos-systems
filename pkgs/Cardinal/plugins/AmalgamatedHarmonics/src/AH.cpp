#include "AH.hpp"

// The plugin-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, 
	// and module name
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

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
