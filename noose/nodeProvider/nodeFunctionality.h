#pragma once
#include "../logic/node.h"

namespace nodeFunctionality
{
	void initialize();

	void Integer(node* theNode);
	void IntegerAddition(node* theNode);
	void IntegerSubtraction(node* theNode);
	void IntegerProduct(node* theNode);
	void IntegerDivision(node* theNode);
	void Vector2i(node* theNode);
	void Vector2iFromInts(node* theNode);
	void Vector2iAddition(node* theNode);
	void Vector2iSubtraction(node* theNode);
	void Vector2iTimesInt(node* theNode);
	void Float(node* theNode);
	void FloatAddition(node* theNode);
	void FloatSubtraction(node* theNode);
	void FloatProduct(node* theNode);
	void FloatDivision(node* theNode);
	void Color(node* theNode);
	void ColorFromInts(node* theNode);
	void Image(node* theNode);
	void SeparateChannels(node* theNode);
	void CombineChannels(node* theNode);
	void Solid(node* theNode);
	void Checker(node* theNode);
	void LinearGradient(node* theNode);
	void Blend(node* theNode);
	void BrightnessContrast(node* theNode);
	void Crop(node* theNode);
	void Flip(node* theNode);
	void Frame(node* theNode);
	void Grayscale(node* theNode);
	void Invert(node* theNode);
	void Mask(node* theNode);
	void Repeat(node* theNode);
	void Rotate90(node* theNode);
	void SelectByColor(node* theNode);
};

