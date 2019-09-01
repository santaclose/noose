#pragma once
#include "uiNode.h"

namespace nodeFunctionality
{
	void initialize();
	void Blend(uiNode* theNode);
	void Checker(uiNode* theNode);
	void ConstructColor(uiNode* theNode);
	void ConstructVector2i(uiNode* theNode);
	void Crop(uiNode* theNode);
	void Decompose(uiNode* theNode);
	void Flip(uiNode* theNode);
	void Float(uiNode* theNode);
	void Grayscale(uiNode* theNode);
	void Image(uiNode* theNode);
	void Integer(uiNode* theNode);
	void Invert(uiNode* theNode);
	void LinearGradient(uiNode* theNode);
	void Mix(uiNode* theNode);
	void Repeat(uiNode* theNode);
	void Rotate90(uiNode* theNode);
	void Solid(uiNode* theNode);
};

