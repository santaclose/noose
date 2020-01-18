#pragma once
#include "node_system/logicalNode.h"

namespace nodeFunctionality
{
	void initialize();
	void Blend(logicalNode* theNode);
	void BrightnessContrast(logicalNode* theNode);
	void Checker(logicalNode* theNode);
	void Combine(logicalNode* theNode);
	void ConstructColor(logicalNode* theNode);
	void ConstructVector2i(logicalNode* theNode);
	void Crop(logicalNode* theNode);
	void Flip(logicalNode* theNode);
	void Float(logicalNode* theNode);
	void Grayscale(logicalNode* theNode);
	void Image(logicalNode* theNode);
	void Integer(logicalNode* theNode);
	void Invert(logicalNode* theNode);
	void LinearGradient(logicalNode* theNode);
	void Mask(logicalNode* theNode);
	void Repeat(logicalNode* theNode);
	void Rotate90(logicalNode* theNode);
	void SelectByColor(logicalNode* theNode);
	void Separate(logicalNode* theNode);
	void Solid(logicalNode* theNode);
};

