#pragma once
#include "../logic/node.h"

namespace nodeFunctionality
{
	void initialize();
	void Blend(node* theNode);
	void BrightnessContrast(node* theNode);
	void Checker(node* theNode);
	void Combine(node* theNode);
	void ConstructColor(node* theNode);
	void ConstructVector2i(node* theNode);
	void Crop(node* theNode);
	void Flip(node* theNode);
	void Float(node* theNode);
	void Grayscale(node* theNode);
	void Image(node* theNode);
	void Integer(node* theNode);
	void Invert(node* theNode);
	void LinearGradient(node* theNode);
	void Mask(node* theNode);
	void Repeat(node* theNode);
	void Rotate90(node* theNode);
	void SelectByColor(node* theNode);
	void Separate(node* theNode);
	void Solid(node* theNode);
};

