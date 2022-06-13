//***********************************************************************************************
//Mind Meld Modular: Modules for VCV Rack by Steve Baker and Marc Boul�
//
//Based on code from the Fundamental plugin by Andrew Belt
//See ./LICENSE.md for all licenses
//***********************************************************************************************


#pragma once

#include "../MindMeldModular.hpp"
#include "Channel.hpp"
#include "DisplayUtil.hpp"
#include "Menus.hpp"
#include "time.h"


struct ShapeMaster;


struct CrossoverParamQuantity : ParamQuantity {
	std::string getDisplayValueString() override {
		float crossover = getValue();
		if (crossover >= Channel::CROSSOVER_OFF) {
			float freq = Channel::CROSSOVER_MULT * std::pow(Channel::CROSSOVER_BASE, crossover);
			return string::f("%i", (int)(freq + 0.5f));
		}
		else {
			return "OFF";
		}
	}
};

struct RepetitionsParamQuantity : ParamQuantity {
	std::string getDisplayValueString() override {
		float reps = getValue();
		if (reps < PlayHead::INF_REPS) {
			return string::f("%i", (int)(reps + 0.5f));
		}
		else {
			return "INF";
		}
	}
};


struct ScopeSettingsButtons : OpaqueWidget {
	const float textHeight = 3.5f;// in mm
	const float textWidths[4] =        {10.84f,   7.11f, 7.11f, 15.92f};// in mm
	const std::string textStrings[4] = {"SCOPE:", "OFF", "VCA", "SIDECHAIN"};

	// user must set up
	int8_t *settingSrc = NULL;
	int* currChan = NULL;
	Channel* channels;
	ScopeBuffers* scopeBuffers;
	
	// local
	std::shared_ptr<Font> font;
	std::string fontPath;
	NVGcolor colorOff;
	float textWidthsPx[5];
	
	
	ScopeSettingsButtons() {
		box.size = mm2px(Vec(textWidths[0] + textWidths[1] + textWidths[2] + textWidths[3], textHeight));
		colorOff = MID_DARKER_GRAY;
		for (int i = 0; i < 4; i++) {
			textWidthsPx[i] = mm2px(textWidths[i]);
		}
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/RobotoCondensed-Regular.ttf"));
	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			if (font->handle >= 0) {
				NVGcolor colorOn = currChan ? CHAN_COLORS[channels[*currChan].channelSettings.cc4[1]] : SCHEME_YELLOW;
				
				// nvgStrokeColor(args.vg, SCHEME_YELLOW);
				// nvgStrokeWidth(args.vg, 0.7f);
				
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.0);
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, 10.0f);
				
				bool scopeOn = false;
				bool scopeVca = false;
				if (settingSrc) {
					scopeOn  = ((*settingSrc & SCOPE_MASK_ON) != 0);
					scopeVca = ((*settingSrc & SCOPE_MASK_VCA_nSC) != 0);
				}
				
				// ANALYSER
				float posx = 0.0f;
				nvgFillColor(args.vg, SCHEME_LIGHT_GRAY);// 0xE6,0xE6,0xE6
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[0].c_str(), NULL);
				posx += textWidthsPx[0];
				
				// OFF
				nvgFillColor(args.vg, (!scopeOn) ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[1].c_str(), NULL);
				posx += textWidthsPx[1];
				
				// VCA
				nvgFillColor(args.vg, (scopeOn && scopeVca) ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[2].c_str(), NULL);
				posx += textWidthsPx[2];
				
				// SIDECHAIN
				nvgFillColor(args.vg, (scopeOn && !scopeVca) ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[3].c_str(), NULL);
				// posx += textWidthsPx[3];
			}
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			float leftX = textWidthsPx[0];
			// click OFF
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[1]) {
				// toggle on/off bit, keep vca/sc bit unchanged
				*settingSrc ^= SCOPE_MASK_ON;
				scopeBuffers->clear();
			}
			leftX += textWidthsPx[1];
			// click VCA
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[2]) {
				// set vca/sc bit and on/off bit 
				*settingSrc |= (SCOPE_MASK_ON | SCOPE_MASK_VCA_nSC);
			}
			leftX += textWidthsPx[2];
			// click SIDECHAIN
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[3]) {
				// set vca/sc bit and on/off bit
				*settingSrc &= ~SCOPE_MASK_VCA_nSC;
				*settingSrc |= SCOPE_MASK_ON;
			}
			// leftX += textWidthsPx[3];
		}
		OpaqueWidget::onButton(e);
	}
};


struct ShapeCommandsButtons : OpaqueWidget {// must use Opaque since LightWidget will not call onDragEnd()
	const float textHeight = 3.5f;// in mm
	const float textWidths[5] =        {9.14f,  10.33f,  13.21f,    11.15f,   12.84f};// in mm
	const std::string textStrings[5] = {"COPY", "PASTE", "REVERSE", "INVERT", "RANDOM"};
	
	// user must set up
	int* currChan = NULL;
	Channel* channels;
	
	// local
	std::shared_ptr<Font> font;
	std::string fontPath;
	NVGcolor colorOff;
	int buttonPressed;// -1 when nothing pressed, button index when a button is pressed
	float textWidthsPx[5];
	// Shape shapeCpBuffer;
	
	
	ShapeCommandsButtons() {
		box.size = mm2px(Vec(textWidths[0] + textWidths[1] + textWidths[2] + textWidths[3] + textWidths[4], textHeight));
		colorOff = MID_DARKER_GRAY;
		buttonPressed = -1;
		for (int i = 0; i < 5; i++) {
			textWidthsPx[i] = mm2px(textWidths[i]);
		}
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/RobotoCondensed-Regular.ttf"));
	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			if (font->handle >= 0) {
				// nvgStrokeColor(args.vg, SCHEME_YELLOW);
				// nvgStrokeWidth(args.vg, 0.7f);
				NVGcolor colorOn = currChan ? CHAN_COLORS[channels[*currChan].channelSettings.cc4[1]] : SCHEME_YELLOW;
				
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.0);
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, 10.0f);
						
				// COPY
				float posx = 0.0f;
				nvgFillColor(args.vg, buttonPressed == 0 ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[0].c_str(), NULL);
				posx += textWidthsPx[0];
				
				// PASTE
				nvgFillColor(args.vg, buttonPressed == 1 ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[1].c_str(), NULL);
				posx += textWidthsPx[1];
				
				// REVERSE
				nvgFillColor(args.vg, buttonPressed == 2 ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[2].c_str(), NULL);
				posx += textWidthsPx[2];
				
				// INVERT
				nvgFillColor(args.vg, buttonPressed == 3? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[3].c_str(), NULL);
				posx += textWidthsPx[3];
				
				// RANDOM
				nvgFillColor(args.vg, buttonPressed == 4 ? colorOn : colorOff);
				nvgText(args.vg, posx + 3.0f, box.size.y / 2.0f, textStrings[4].c_str(), NULL);
			}
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			float leftX = 0;
			// click COPY
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[0]) {
				// Internal memory version:
				// channels[*currChan].copyShapeTo(&shapeCpBuffer);
				
				// Clipboard version: 
				json_t* shapeJ = channels[*currChan].getShape()->dataToJsonShape();
				json_t* clipboardJ = json_object();		
				json_object_set_new(clipboardJ, "MindMeld-ShapeMaster-Clipboard-Shape", shapeJ);
				char* shapeClip = json_dumps(clipboardJ, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
				json_decref(clipboardJ);
				glfwSetClipboardString(APP->window->win, shapeClip);
				free(shapeClip);
				
				buttonPressed = 0;
			}
			leftX += textWidthsPx[0];
			// click PASTE
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[1]) {
				// Push ShapeCompleteChange history action (rest is done further below)
				ShapeCompleteChange* h = new ShapeCompleteChange;
				h->shapeSrc = channels[*currChan].getShape();
				h->oldShape = new Shape();
				h->shapeSrc->copyShapeTo(h->oldShape);

				// Internal memory version:
				// channels[*currChan].pasteShapeFrom(&shapeCpBuffer);
				
				// Clipboard version:
				bool successPaste = false;
				const char* shapeClip = glfwGetClipboardString(APP->window->win);
				if (!shapeClip) {
					WARN("IOP error getting clipboard string");
				}
				else {
					json_error_t error;
					json_t* clipboardJ = json_loads(shapeClip, 0, &error);
					if (!clipboardJ) {
						WARN("IOP error json parsing clipboard");
					}
					else {
						DEFER({json_decref(clipboardJ);});
						// MindMeld-ShapeMaster-Clipboard-Shape
						json_t* shapeJ = json_object_get(clipboardJ, "MindMeld-ShapeMaster-Clipboard-Shape");
						if (!shapeJ) {
							WARN("IOP error no MindMeld-ShapeMaster-Clipboard-Shape present in clipboard");
						}
						else {
							channels[*currChan].getShape()->dataFromJsonShape(shapeJ);
							successPaste = true;
						}
					}	
				}
				
				buttonPressed = 1;
				
				if (successPaste) {
					h->newShape = new Shape();
					h->shapeSrc->copyShapeTo(h->newShape);
					h->name = "paste shape";
					APP->history->push(h);
				}
				else {
					delete h;// h->oldShape will be automatically deleted by desctructor
				}
			}
			leftX += textWidthsPx[1];
			// click REVERSE
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[2]) {
				channels[*currChan].reverseShape();
				buttonPressed = 2;
				
				// Push InvertOrReverseChange history action (rest is done further below)
				InvertOrReverseChange* h = new InvertOrReverseChange;
				h->shapeSrc = channels[*currChan].getShape();
				h->isReverse = true;
				h->name = "reverse shape";
				APP->history->push(h);

			}
			leftX += textWidthsPx[2];
			// click INVERSE
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[3]) {
				channels[*currChan].invertShape();
				buttonPressed = 3;
				
				// Push InvertOrReverseChange history action (rest is done further below)
				InvertOrReverseChange* h = new InvertOrReverseChange;
				h->shapeSrc = channels[*currChan].getShape();
				h->isReverse = false;
				h->name = "invert shape";
				APP->history->push(h);
			}
			leftX += textWidthsPx[3];
			// click RANDOM
			if (e.pos.x > leftX && e.pos.x < leftX + textWidthsPx[4]) {
				if ((APP->window->getMods() & RACK_MOD_CTRL) != 0) {
					channels[*currChan].randomizeShape(true);
				}
				else {
					ui::Menu *menu = createMenu();
					addRandomMenu(menu, &channels[*currChan]);
				}
				buttonPressed = 4;
			}
		}
		OpaqueWidget::onButton(e);
	}
	
	void onDragEnd(const event::DragEnd &e) override {// required since if mouse button release happens outside the box of the opaque widget, it will not trigger onButton (to detect GLFW_RELEASE)
		buttonPressed = -1;
		OpaqueWidget::onDragEnd(e);
	}
};


#ifdef SM_PRO
struct ProSvgWithMessage : ProSvg {
	DisplayInfo* displayInfo;
	void onButton(const event::Button& e) override {
		displayInfo->displayMessageTimeOff = time(0) + 4;
		displayInfo->displayMessage = "Thank you for supporting MindMeld";
		SvgWidget::onButton(e);
	}
};
#endif


struct BigNumbers : TransparentWidget {
	// user must set up
	int* currChan = NULL;
	Channel* channels;
	DisplayInfo* displayInfo;
	
	
	// local
	std::shared_ptr<Font> font;
	std::string fontPath;
	NVGcolor color;
	std::string text;
	math::Vec textOffset;
	
	
	BigNumbers() {
		box.size = mm2px(Vec(40.0f, 15.0f));
		color = SCHEME_LIGHT_GRAY;
		textOffset = Vec(box.size.div(2.0f));
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/RobotoCondensed-Regular.ttf"));
	}
		
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			if (currChan != NULL) {
				time_t currTime = time(0);
				if (currTime - displayInfo->lastMovedKnobTime < 4) {
					int srcId = displayInfo->lastMovedKnobId;
					int chan = *currChan;
					bool inactive = false;// not all controls will use this
					switch (srcId) {
						case (LENGTH_SYNC_PARAM):
						case (LENGTH_UNSYNC_PARAM):
							text = channels[chan].getLengthText(&inactive);
							break;
						case (REPETITIONS_PARAM):
							text = channels[chan].getRepetitionsText(&inactive);
							break;
						case (OFFSET_PARAM):
							text = channels[chan].getOffsetText(&inactive);
							break;
						case (SWING_PARAM):
							text = channels[chan].getSwingText(&inactive);
							break;
						case (PHASE_PARAM):
							text = channels[chan].getPhaseText();
							break;
						case (RESPONSE_PARAM):
							text = channels[chan].getResponseText();
							break;
						case (WARP_PARAM):
							text = channels[chan].getWarpText();
							break;
						case (AMOUNT_PARAM):
							text = channels[chan].getAmountText();
							break;
						case (SLEW_PARAM):
							text = channels[chan].getSlewText();
							break;
						case (SMOOTH_PARAM):
							text = channels[chan].getSmoothText();
							break;
						case (CROSSOVER_PARAM):
							text = channels[chan].getCrossoverText(&inactive);
							break;
						case (HIGH_PARAM):
							text = channels[chan].getHighText(&inactive);
							break;
						case (LOW_PARAM):
							text = channels[chan].getLowText(&inactive);
							break;
						case (TRIGLEV_PARAM):
							text = channels[chan].getTrigLevelText(&inactive);
							break;					
						default:
							text = "";
					}
					if (inactive) text = "";	
					
					if (font->handle >= 0 && text.compare("") != 0) {
						nvgFillColor(args.vg, color);
						nvgFontFaceId(args.vg, font->handle);
						nvgTextLetterSpacing(args.vg, 0.0);
						nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
						nvgFontSize(args.vg, 24.0f);
						nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL);
					}
				}
			}
		}
	}
};



struct SmLabelBase : widget::OpaqueWidget {
	// This struct is adapted from Rack's LedDisplayChoice in app/LedDisplay.{c,h}pp

	// user must set up
	int8_t* knobLabelColorsSrc = NULL;
	int* currChan = NULL;
	Channel* channels;

	// local
	std::string text;
	std::shared_ptr<Font> font;
	std::string fontPath;
	math::Vec textOffset;
	NVGcolor color;
	bool inactive = false;


	SmLabelBase() {
		box.size = mm2px(Vec(10.6f, 5.0f));
		color = DISP_COLORS[0];
		textOffset = Vec(3.0f, 11.3f);
		text = "---";
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/RobotoCondensed-Regular.ttf"));
	};

	virtual void prepareText() {}

	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			prepareText();// may set inactive = true if control can be inactive in some cases
			
			if (inactive) {
				color = MID_GRAY;
			}
			else if (knobLabelColorsSrc) {
				if (*knobLabelColorsSrc < 2) {
					color = CHAN_COLORS[*knobLabelColorsSrc == 0 ? 0 : 8];
				}
				else if (currChan) {
					color = CHAN_COLORS[channels[*currChan].channelSettings.cc4[1]];
				}
			}

			nvgScissor(args.vg, RECT_ARGS(args.clipBox));
			if (font->handle >= 0) {
				nvgFillColor(args.vg, color);
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.0);

				nvgFontSize(args.vg, 10.5f);
				nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL);
			}
			nvgResetScissor(args.vg);
		}
	}

	void onButton(const event::Button& e) override {
		OpaqueWidget::onButton(e);

		if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT || e.button == GLFW_MOUSE_BUTTON_RIGHT)) {
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
	}
};



struct TrigModeLabel : SmLabelBase {
	TrigModeLabel() {
		box.size = mm2px(Vec(10.5f, 5.0f));
	}
	
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getTrigModeText();
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			ui::Menu *menu = createMenu();
			addTrigModeMenu(menu, &channels[*currChan]);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
};



struct PlayModeLabel : SmLabelBase {
	PlayModeLabel() {
		box.size = mm2px(Vec(10.5f, 5.0f));
	}
	
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getPlayModeText();
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			ui::Menu *menu = createMenu();
			addPlayModeMenu(menu, &channels[*currChan]);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
};



struct KnobLabelRepetitions : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getRepetitionsText(&inactive);
		}
	}
	// cv level slider (TODO if applicable, get code from KnobLabelLength)
};



struct GridXLabel : SmLabelBase {
	NVGcolor labelColor = nvgRGB(204, 204, 204);
	clock_t lastTimes[3] = {CLOCKS_PER_SEC, CLOCKS_PER_SEC, CLOCKS_PER_SEC};
	int lastNums[3] = {0, 0, 0};
	int lastIndex = 0;// points location of next key to store

	
	GridXLabel() {
		box.size = mm2px(Vec(22.62f, 5.0f));
	}
	
	void prepareText() override {
		if (currChan) {
			int snapValue = channels[*currChan].getGridX();
			text = string::f("%i", snapValue);
		}
		else {
			text = "16";
		}
	}
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			if (currChan) {
				color = CHAN_COLORS[channels[*currChan].channelSettings.cc4[1]];
			}

			nvgScissor(args.vg, RECT_ARGS(args.clipBox));
			if (font->handle >= 0) {
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.0);
				nvgFontSize(args.vg, 10.5f);
				
				nvgFillColor(args.vg, labelColor);
				text = std::string("GRID-X: [ ");
				nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL);
				float width1 = nvgTextBounds(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL, NULL);
							
				nvgFillColor(args.vg, color);
				prepareText();
				nvgText(args.vg, textOffset.x + width1, textOffset.y, text.c_str(), NULL);
				float width2 = nvgTextBounds(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL, NULL);
				
				nvgFillColor(args.vg, labelColor);
				text = std::string(" ]");
				nvgText(args.vg, textOffset.x + width1 + width2 + 0.2f, textOffset.y, text.c_str(), NULL);
			}
			nvgResetScissor(args.vg);
		}
	}		
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			ui::Menu *menu = createMenu();
			addGridXMenu(menu, &channels[*currChan]);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
	
	void onHoverKey(const event::HoverKey& e) override {
		if (e.action == GLFW_PRESS) {
			int num = -1;
			if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9) {
				num = e.key - GLFW_KEY_0;
			}
			else if (e.key >= GLFW_KEY_KP_0 && e.key <= GLFW_KEY_KP_9) {
				num = e.key - GLFW_KEY_KP_0;
			}

			if (num != -1) {
				// enter into ring buffer
				lastTimes[lastIndex] = clock();
				lastNums[lastIndex] = num;
				int im2 = (lastIndex - 2 + 3) % 3;
				int im1 = (lastIndex - 1 + 3) % 3;
				int im0 = lastIndex;
				lastIndex = (lastIndex + 1) % 3;
				
				// test three-digit number
				if (((float)(lastTimes[im0] - lastTimes[im2]))/(float)CLOCKS_PER_SEC < 2.0f ) {
					num = lastNums[im2] * 100 + lastNums[im1] * 10 + lastNums[im0]; 
					lastTimes[0] = CLOCKS_PER_SEC; 
					lastTimes[1] = CLOCKS_PER_SEC; 
					lastTimes[2] = CLOCKS_PER_SEC; 
					// DEBUG("3: %i (%i*100+%i*10+%i)", num, lastNums[im2], lastNums[im1], lastNums[im0]);
				}
				// test two-digit number
				else if (((float)(lastTimes[im0] - lastTimes[im1]))/(float)CLOCKS_PER_SEC < 1.0f ) {
					num = lastNums[im1] * 10 + lastNums[im0];
					// DEBUG("2: %i (%i * 10 + %i)", num, lastNums[im1], lastNums[im0]);
				}
				// else it's a one-digit number, and it's in "num"
				// else DEBUG("1: %i", num);

				// if withing 2-128 range, then grab it
				if (num >= 2 && num <= 128) {
					channels[*currChan].setGridX(num, true);					
				}
			}
		}
	}
};



struct RangeLabel : SmLabelBase {
	NVGcolor labelColor = nvgRGB(204, 204, 204);
	
	RangeLabel() {
		box.size = mm2px(Vec(22.62f, 5.0f));
	}
	
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getRangeText();
		}
		else {
			text = "0-10V";
		}
	}
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (!(font = APP->window->loadFont(fontPath))) {
				return;
			}
			if (currChan) {
				color = CHAN_COLORS[channels[*currChan].channelSettings.cc4[1]];
			}

			nvgScissor(args.vg, RECT_ARGS(args.clipBox));
			if (font->handle >= 0) {
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.0);
				nvgFontSize(args.vg, 10.5f);
				
				nvgFillColor(args.vg, labelColor);
				text = std::string("RANGE: [ ");
				nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL);
				float width1 = nvgTextBounds(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL, NULL);
							
				nvgFillColor(args.vg, color);
				prepareText();
				nvgText(args.vg, textOffset.x + width1, textOffset.y, text.c_str(), NULL);
				float width2 = nvgTextBounds(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL, NULL);
				
				nvgFillColor(args.vg, labelColor);
				text = std::string(" ]");
				nvgText(args.vg, textOffset.x + width1 + width2 + 0.2f, textOffset.y, text.c_str(), NULL);
			}
			nvgResetScissor(args.vg);
		}
	}	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			ui::Menu *menu = createMenu();
			addRangeMenu(menu, &channels[*currChan]);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
};



struct PresetLabel : SmLabelBase {
	const std::string defaultLabelText = "Select Preset";
	
	PresetAndShapeManager* presetAndShapeManager = NULL;
	bool* presetOrShapeDirty = NULL;
	bool* unsupportedSync = NULL;
	
	PresetLabel() {
		box.size = mm2px(Vec(40.0f - 4.5f * 2.0f, 5.0f));// room for arrow buttons
		textOffset = Vec(0.0f, 10.4f);
	}
	
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getPresetPath();
			if (text.empty()) {
				text = defaultLabelText;
			}
			else {
				text = system::getFilename(text);// string::filenameBase(string::filename(text));
				text.erase(text.end() - 5, text.end());// remove extension
				if (presetOrShapeDirty != NULL && *presetOrShapeDirty) {
					text.insert(0, "*");
				}
				else if (unsupportedSync != NULL && *unsupportedSync) {
					text.insert(0, "\u00B0");
				}
			}
		}
		else {
			text = defaultLabelText;
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {			
			presetAndShapeManager->createPresetOrShapeMenu(&(channels[*currChan]), true);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
};
struct ShapeLabel : SmLabelBase {
	const std::string defaultLabelText = "Select Shape";
	
	PresetAndShapeManager* presetAndShapeManager = NULL;
	bool* presetOrShapeDirty = NULL;
	
	ShapeLabel() {
		box.size = mm2px(Vec(40.0f - 4.5f * 2.0f, 5.0f));// room for arrow buttons
		textOffset = Vec(0.0f, 10.4f);
	}
	
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getShapePath();
			if (text.empty()) {
				text = defaultLabelText;
			}
			else {
				text = system::getFilename(text);// string::filenameBase(string::filename(text));
				text.erase(text.end() - 5, text.end());// remove extension
				if (presetOrShapeDirty != NULL && *presetOrShapeDirty) {
					text.insert(0, "*");
				}
			}
		}
		else {
			text = defaultLabelText;
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {			
			presetAndShapeManager->createPresetOrShapeMenu(&(channels[*currChan]), false);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		else {
			SmLabelBase::onButton(e);
		}
	}
};
struct PresetOrShapeArrowButton : Switch {
	bool* buttonPressedSrc = NULL;
	PresetAndShapeManager* presetAndShapeManager;
	int chanNum;
	
	PresetOrShapeArrowButton() {
		momentary = true;
		box.size = mm2px(Vec(3.5f, 5.0f));// room for arrow buttons
	}
	void onDragStart(const event::DragStart& e) override {
		if (buttonPressedSrc) {
			*buttonPressedSrc = true;
		}
		Switch::onDragStart(e);
	}
	void onDragEnd(const event::DragEnd& e) override {
		if (buttonPressedSrc) {
			*buttonPressedSrc = false;
		}
		Switch::onDragEnd(e);
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			presetAndShapeManager->cleanWorkload(chanNum);
			event::Action eAction;
			onAction(eAction);
			e.consume(this);			
		}
		else {
			Switch::onButton(e);
		}
	}
};



struct KnobLabelLength : SmLabelBase {
	Param* lengthSyncParamSrc = NULL;// must update this knob when menu is used to select a new sync ratio. this is actually the sync length param for channel 0, must index accordingly for other channels
	Param* lengthUnsyncParamSrc = NULL;// must update this knob when menu is used to select a new unsynced length. this is actually the unsync length param for channel 0, must index accordingly for other channels
	
	const Vec triSize = mm2px(Vec(1.5f, 1.2f));
	const float triMarginX = mm2px(0.7f);
	
	
	void prepareText() override {
		inactive = false;// used for invalid lengths when synced
		if (currChan) {
			text = channels[*currChan].getLengthText(&inactive);
		}
	}
	
	void draw(const DrawArgs &args) override {
		SmLabelBase::draw(args);
		// draw downward triangle size-permitting
		if (text.size() <= 4 && currChan != NULL && channels[*currChan].isSync()) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, box.size.x - triMarginX - triSize.x, box.size.y * 0.55f - triSize.y * 0.5f);
			nvgLineTo(args.vg, box.size.x - triMarginX, box.size.y * 0.55f - triSize.y * 0.5f);
			nvgLineTo(args.vg, box.size.x - triMarginX - triSize.x * 0.5f, box.size.y * 0.55f + triSize.y * 0.5f);
			nvgClosePath(args.vg);
			nvgFillColor(args.vg, nvgRGB(204, 204, 204));
			nvgFill(args.vg);
		}
	}
	
	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT || e.button == GLFW_MOUSE_BUTTON_RIGHT)) {
			ui::Menu *menu = createMenu();
			#ifdef SM_PRO
			if (channels[*currChan].isSync()) {
				addSyncRatioMenuTwoLevel(menu, lengthSyncParamSrc + NUM_CHAN_PARAMS * *currChan, &channels[*currChan]);
			}
			else
			#endif
			{
				addUnsyncRatioMenu(menu, lengthUnsyncParamSrc + NUM_CHAN_PARAMS * *currChan, &channels[*currChan]);
			}
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
		}
		// else if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			// ui::Menu *menu = createMenu();

			// // cv level slider (TODO)
			// int chan = *currChan;
			// CvLevelSlider *cvLevSlider = new CvLevelSlider(&(trackEqsSrc[trk].freqCvAtten[band]));
			// cvLevSlider->box.size.x = 200.0f;
			// menu->addChild(cvLevSlider);

			// event::Action eAction;
			// onAction(eAction);
			// e.consume(this);
		// }
		else {
			SmLabelBase::onButton(e);
		}
	}
};


struct KnobLabelOffset : SmLabelBase {
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getOffsetText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};

struct KnobLabelSwing : SmLabelBase {
	void prepareText() override {
		// inactive = false;
		if (currChan) {
			text = channels[*currChan].getSwingText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelPhase : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getPhaseText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};

struct KnobLabelResponse : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getResponseText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelWarp : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getWarpText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelLevel : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getAmountText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelSlew : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getSlewText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelSmooth : SmLabelBase {
	void prepareText() override {
		if (currChan) {
			text = channels[*currChan].getSmoothText();
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelCrossover : SmLabelBase {
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getCrossoverText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};

struct KnobLabelHigh : SmLabelBase {
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getHighText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};

struct KnobLabelLow : SmLabelBase {
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getLowText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct KnobLabelTrigLevel : SmLabelBase {
	void prepareText() override {
		inactive = false;
		if (currChan) {
			text = channels[*currChan].getTrigLevelText(&inactive);
		}
	}
	// cv level slider (TODO, get code from KnobLabelLength)
};


struct SmChannelButton : OpaqueWidget {
	Channel* channels;
	int* currChan = NULL;
	PackedBytes4* miscSettings2GlobalSrc = NULL;
	bool* trigExpPresentSrc = NULL;
	int lastChan = -1;
	bool* running;

	widget::FramebufferWidget* fb;
	widget::SvgWidget* sw;
	std::vector<std::shared_ptr<Svg>> frames;
	bool manualDrawTopOverride = false;
	const NVGcolor haloColor = nvgRGB(0xE5, 0xB8, 0x4E);// this should match the color of fill of the on button
	
	// procedure to make from tight button svgs:
	// 1- make width (W) = 53.792
	// 2- make posx (X) = (6.001 + 0.8262857) * i;  where i is 0-indexed (i.e. n-1 when n is En-on.svg)
	SmChannelButton() {
		fb = new widget::FramebufferWidget;
		addChild(fb);
		sw = new widget::SvgWidget;
		fb->addChild(sw);
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E1-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E2-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E3-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E4-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E5-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E6-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E7-on.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comp/shape/E8-on.svg")));
	}

	void addFrame(std::shared_ptr<Svg> svg) {
		frames.push_back(svg);
		if (!sw->svg) {
			sw->setSvg(svg);
			box.size = sw->box.size;
			fb->box.size = sw->box.size;
		}
	}
	
	void draw(const DrawArgs &args) override {
		if (manualDrawTopOverride) {
			OpaqueWidget::draw(args);
		}
	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			if (settings::haloBrightness != 0.f && currChan) {
				drawRectHalo(args, Vec(box.size.y, box.size.y), haloColor, mm2px((6.001 + 0.8262857) * (*currChan)));
			}
			manualDrawTopOverride = true;
			draw(args);
			manualDrawTopOverride = false;
		}
		OpaqueWidget::drawLayer(args, layer);
	}
	
	void onButton(const event::Button& e) override {
		if (e.action != GLFW_PRESS) {
			return;
		}

		int chan = 0;
		for (; chan < 8; chan++) {
			float buttonIx = mm2px((6.001f + 0.8262857f) * chan);
			if (e.pos.x >= buttonIx && e.pos.x <= (buttonIx + mm2px(6.001f))) {
				break;
			}
		}
		if (chan == 8) {
			return;
		}		
	
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			ui::Menu *menu = createMenu();
			createChannelMenu(menu, channels, chan, miscSettings2GlobalSrc, *trigExpPresentSrc, running);
		}
		else if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			ChannelNumChange* h = new ChannelNumChange;
			h->currChanSrc = currChan;
			h->oldChanNum = *currChan;
			h->newChanNum = chan;
			APP->history->push(h);
			
			*currChan = chan;
		}
		event::Action eAction;
		onAction(eAction);
		e.consume(this);
	}
	
	void step() override {
		if (currChan && (*currChan != lastChan)) {
			lastChan = *currChan;
			// this if() is onChange();
			if (!frames.empty()) {
				int index = math::clamp(*currChan, 0, (int) frames.size() - 1);
				sw->setSvg(frames[index]);
				fb->dirty = true;
			}
		}
		OpaqueWidget::step();
	}
};

// struct MyParamWidget : ParamWidget {
	// void createContextMenu() {};
// };

struct SmPlayButton : MmPlayButton {
	int* currChan;
	Channel* channels;

	void onDragStart(const event::DragStart& e) override {
		ParamQuantity* paramQuantity = getParamQuantity();
		if (paramQuantity && paramQuantity->getValue() > 0.5f && (APP->window->getMods() & RACK_MOD_CTRL) != 0) {
			paramQuantity->setValue(0.0f);
			channels[*currChan].initRun(true);
		}
		MmPlayButton::onDragStart(e);
	}

	// void onButton(const event::Button& e) override {
		// if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT) && 
				// (APP->window->getMods() & RACK_MOD_CTRL) != 0) {			
			// channels[*currChan].initRun(true);
			// event::Action eAction;
			// onAction(eAction);
			// e.consume(this);
			// return;
		// }
		// MmPlayButton::onButton(e);
	// }	
};

typedef MmFreezeButton SmFreezeButton;

struct SmRunButton : LedButton2 {
	ShapeMaster* shapeMasterSrc;
	void onDragStart(const event::DragStart& e) override {
		LedButton2::onDragStart(e);
		ParamQuantity* paramQuantity = getParamQuantity();
		if (paramQuantity) {
			if (paramQuantity->getValue() >= 0.5f) {
				// Push RunningChange history action
				RunChange* h = new RunChange;
				h->shapeMasterSrc = shapeMasterSrc;
				APP->history->push(h);
			}
		}
	}
};

struct SmLoopButton : MmLoopButton {
	int* currChan;
	Channel* channels;
	
	void onDragStart(const event::DragStart& e) override {		
		bool maxChanged = false;
		ParamQuantity* paramQuantity = getParamQuantity();
		if (paramQuantity) {			
			if (paramQuantity->getValue() >= 0.5f && paramQuantity->getValue() < 1.5f && !channels[*currChan].currTrigModeAllowsLoop()) {
				maxChanged = true;
				paramQuantity->maxValue = 1.0f;
			}
		}
		MmLoopButton::onDragStart(e);
		if (maxChanged) {
			paramQuantity->maxValue = 2.0f;
		}
	}
	// alternate implementation, but will not work right with undo (could undo ourselves back into loop mode)
	// void onButton(const event::Button& e) override {
		// MmLoopButton::onButton(e);
		// if (paramQuantity && e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT)) {			
			// if (paramQuantity->getValue() >= 0.5f && paramQuantity->getValue() < 1.5f && !channels[*currChan].currTrigModeAllowsLoop()) {
				// paramQuantity->setValue(2.0f);
			// }
		// }
	// }
	
};

struct SmSyncButton : MmSyncButton {
	int* currChan;
	Channel* channels;
	Input* inClock;
	DisplayInfo* displayInfo;
	
	
	void onDragStart(const event::DragStart& e) override {		
		#ifdef SM_PRO
		ParamQuantity* paramQuantity = getParamQuantity();
		if (inClock->isConnected() || (paramQuantity && paramQuantity->getValue() >= 0.5f) ) {
			MmSyncButton::onDragStart(e);
		}
		else {
			// clock is disconnected and sync is off
			if (paramQuantity) {			
				paramQuantity->maxValue = 0.0f;
			}
			displayInfo->displayMessageTimeOff = time(0) + 4;
			displayInfo->displayMessage = "Please connect a clock cable first";
			// osdialog_message(OSDIALOG_INFO, OSDIALOG_OK, "Please connect a clock cable first");
			if (paramQuantity) {			
				paramQuantity->maxValue = 1.0f;
			}
		}
		#else
		displayInfo->displayMessageTimeOff = time(0) + 4;
		displayInfo->displayMessage = "Sync is only available in the Pro version";
		MmSyncButton::onDragStart(e);
		#endif
	}
};

typedef MmLockButton SmLockButton;

typedef MmHeadphonesButton SmAuditionScButton;


struct SmSidechainSettingsButton : MmGearButton {
	int* currChan;
	Channel* channels;

	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT)) {			
			createSidechainSettingsMenu(&(channels[*currChan]));
			event::Action eAction;
			onAction(eAction);
			e.consume(this);
			return;
		}
		MmGearButton::onButton(e);
	}
};



struct SmKnob : Mm8mmKnobGrayWithArc {
	int* currChan;
	Channel* channels;// will likely need this for CV arcs
	DisplayInfo* displayInfo;
	
	void onDragMove(const event::DragMove& e) override {
		MmKnobWithArc::onDragMove(e);
		ParamQuantity* paramQuantity = getParamQuantity();
		if (paramQuantity) {
			displayInfo->lastMovedKnobId = paramQuantity->paramId % NUM_CHAN_PARAMS;
			displayInfo->lastMovedKnobTime = time(0);
		}
	}
};

typedef SmKnob SmKnobLeft;

struct SmKnobTop : SmKnob {
	SmKnobTop() {
		topCentered = true;
	}
};
struct SmKnobRight : SmKnob {
	SmKnobRight() {
		rightWhenNottopCentered = true;
	}
};



// Block 1

typedef SmKnobLeft SmRepetitionsKnob;



// Block 2

typedef SmKnobLeft SmLengthSyncKnob;

typedef SmKnobTop SmLengthUnsyncKnob;

typedef SmKnobLeft SmOffsetKnob;

typedef SmKnobTop SmSwingKnob;



// Block 3

typedef SmKnobLeft SmPhaseKnob;

typedef SmKnobTop SmResponseKnob;

typedef SmKnobTop SmWarpKnob;



// Block 4

typedef SmKnobRight SmLevelKnob;

typedef SmKnobLeft SmSlewKnob;

typedef SmKnobLeft SmSmoothKnob;



// Block 5

typedef SmKnobLeft SmCrossoverKnob;

typedef SmKnobRight SmHighKnob;

typedef SmKnobRight SmLowKnob;



// Block 6 (sidechain)

typedef SmKnobLeft SmTrigLevelKnob;

