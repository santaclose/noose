#pragma once
#include "uiNode.h"

namespace nodeFunctionality
{
	void initialize();
	void Checker(uiNode* theNode);
	void ConstructColor(uiNode* theNode);
	void ConstructVector2i(uiNode* theNode);
	void Flip(uiNode* theNode);
	void Float(uiNode* theNode);
	void Image(uiNode* theNode);
	void Integer(uiNode* theNode);
	void Invert(uiNode* theNode);
	void LinearGradient(uiNode* theNode);
	void Multiply(uiNode* theNode);
	void Output(uiNode* theNode);
	void Repeat(uiNode* theNode);
	void Rotate90(uiNode* theNode);
};

