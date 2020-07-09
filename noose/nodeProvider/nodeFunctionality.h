#pragma once
#include "../logic/node.h"

namespace nodeFunctionality
{
	void initialize();

	// insert image
	void ImageFromFile(node* theNode);
	void Solid(node* theNode);
	void Checker(node* theNode);
	void LinearGradient(node* theNode);
	// image processing
	void SeparateChannels(node* theNode);
	void CombineChannels(node* theNode);
	void Blend(node* theNode);
	void BrightnessContrast(node* theNode);
	void Crop(node* theNode);
	void Extend(node* theNode);
	void Patch(node* theNode);
	void Flip(node* theNode);
	void Frame(node* theNode);
	void Grayscale(node* theNode);
	void GammaCorrection(node* theNode);
	void Invert(node* theNode);
	void Mask(node* theNode);
	void Repeat(node* theNode);
	void Rotate90(node* theNode);
	void SelectByColor(node* theNode);
	// color operations
	void Color(node* theNode);
	void ColorFromRGBAInts(node* theNode);
	void RGBAIntsFromColor(node* theNode);
	// int vector operations
	void Vector2i(node* theNode);
	void Vector2iFromInts(node* theNode);
	void SeparateVector2i(node* theNode);
	void Vector2iAddition(node* theNode);
	void Vector2iSubtraction(node* theNode);
	void Vector2iTimesInt(node* theNode);
	// integer operations
	void Integer(node* theNode);
	void IntegerAddition(node* theNode);
	void IntegerSubtraction(node* theNode);
	void IntegerProduct(node* theNode);
	void IntegerDivision(node* theNode);
	// float operations
	void Float(node* theNode);
	void FloatAddition(node* theNode);
	void FloatSubtraction(node* theNode);
	void FloatProduct(node* theNode);
	void FloatDivision(node* theNode);
};

