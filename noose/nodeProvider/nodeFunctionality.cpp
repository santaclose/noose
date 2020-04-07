#include "nodeFunctionality.h"
#include <iostream>

sf::Shader imageShader;
sf::Shader blendShader;
sf::Shader brightnessContrastShader;
sf::Shader checkerShader;
sf::Shader flipShader;
sf::Shader frameShader;
sf::Shader invertShader;
sf::Shader linearGradientShader;
sf::Shader repeatShader;
sf::Shader rotate90Shader;
sf::Shader solidShader;
sf::Shader maskShader;
sf::Shader grayscaleShader;
sf::Shader gammaCorrectionShader;
sf::Shader separateShader;
sf::Shader combineShader;
sf::Shader cropShader;
sf::Shader selectByColorShader;

sf::RenderStates rs;

void nodeFunctionality::initialize()
{
	rs.blendMode = sf::BlendNone;

	if (!imageShader.loadFromFile("res/shaders/loadImage.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load image shader\n";
	if (!blendShader.loadFromFile("res/nodeShaders/blend.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load blend shader\n";
	if (!brightnessContrastShader.loadFromFile("res/nodeShaders/brightness-contrast.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load brightness-contrast shader\n";
	if (!checkerShader.loadFromFile("res/nodeShaders/checker.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load checker shader\n";
	if (!flipShader.loadFromFile("res/nodeShaders/flip.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load flip shader\n";
	if (!frameShader.loadFromFile("res/nodeShaders/frame.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load frame shader\n";
	if (!invertShader.loadFromFile("res/nodeShaders/invert.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load invert shader\n";
	if (!linearGradientShader.loadFromFile("res/nodeShaders/linearGradient.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load linearGradient shader\n";
	if (!repeatShader.loadFromFile("res/nodeShaders/repeat.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load repeat shader\n";
	if (!rotate90Shader.loadFromFile("res/nodeShaders/rotate90.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load rotate90 shader\n";
	if (!solidShader.loadFromFile("res/nodeShaders/solid.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load solidShader shader\n";
	if (!maskShader.loadFromFile("res/nodeShaders/mask.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load mix shader\n";
	if (!grayscaleShader.loadFromFile("res/nodeShaders/grayscale.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load grayscale shader\n";
	if (!gammaCorrectionShader.loadFromFile("res/nodeShaders/gammaCorrection.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load gammaCorrection shader\n";
	if (!separateShader.loadFromFile("res/nodeShaders/separate.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load separate shader\n";
	if (!combineShader.loadFromFile("res/nodeShaders/combine.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load combine shader\n";
	if (!cropShader.loadFromFile("res/nodeShaders/crop.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load crop shader\n";
	if (!selectByColorShader.loadFromFile("res/nodeShaders/selectByColor.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load selectByColor shader\n";
}

void nodeFunctionality::Integer(node* theNode)
{
	*((int*)theNode->getDataPointer(1)) = *((int*)theNode->getDataPointer(0));
}

void nodeFunctionality::IntegerAddition(node* theNode)
{
	*((int*)theNode->getDataPointer(2)) =
		*((int*)theNode->getDataPointer(0)) +
		*((int*)theNode->getDataPointer(1));
}

void nodeFunctionality::IntegerSubtraction(node* theNode)
{
	*((int*)theNode->getDataPointer(2)) =
		*((int*)theNode->getDataPointer(0)) -
		*((int*)theNode->getDataPointer(1));
}

void nodeFunctionality::IntegerProduct(node* theNode)
{
	*((int*)theNode->getDataPointer(2)) =
		*((int*)theNode->getDataPointer(0)) *
		*((int*)theNode->getDataPointer(1));
}

void nodeFunctionality::IntegerDivision(node* theNode)
{
	*((int*)theNode->getDataPointer(2)) =
		*((int*)theNode->getDataPointer(0)) /
		*((int*)theNode->getDataPointer(1));
}

void nodeFunctionality::Vector2i(node* theNode)
{
	*((sf::Vector2i*)theNode->getDataPointer(1)) = *((sf::Vector2i*)theNode->getDataPointer(0));
}

void nodeFunctionality::Vector2iFromInts(node* theNode)
{
	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointer(2));
	outputPointer->x = *((int*)(theNode->getDataPointer(0)));
	outputPointer->y = *((int*)(theNode->getDataPointer(1)));
}

void nodeFunctionality::Vector2iAddition(node* theNode)
{
	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::Vector2i* a = ((sf::Vector2i*)(theNode->getDataPointer(0)));
	sf::Vector2i* b = ((sf::Vector2i*)(theNode->getDataPointer(1)));
	outputPointer->x = a->x + b->x;
	outputPointer->y = a->y + b->y;
}

void nodeFunctionality::Vector2iSubtraction(node* theNode)
{
	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::Vector2i* a = ((sf::Vector2i*)(theNode->getDataPointer(0)));
	sf::Vector2i* b = ((sf::Vector2i*)(theNode->getDataPointer(1)));
	outputPointer->x = a->x - b->x;
	outputPointer->y = a->y - b->y;
}

void nodeFunctionality::Vector2iTimesInt(node* theNode)
{
	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::Vector2i* a = ((sf::Vector2i*)(theNode->getDataPointer(0)));
	int b = *((int*)(theNode->getDataPointer(1)));
	outputPointer->x = a->x * b;
	outputPointer->y = a->y * b;
}

void nodeFunctionality::Float(node* theNode)
{
	*((float*)theNode->getDataPointer(1)) = *((float*)theNode->getDataPointer(0));
}

void nodeFunctionality::FloatAddition(node* theNode)
{
	*((float*)theNode->getDataPointer(2)) =
		*((float*)theNode->getDataPointer(0)) +
		*((float*)theNode->getDataPointer(1));
}

void nodeFunctionality::FloatSubtraction(node* theNode)
{
	*((float*)theNode->getDataPointer(2)) =
		*((float*)theNode->getDataPointer(0)) -
		*((float*)theNode->getDataPointer(1));
}

void nodeFunctionality::FloatProduct(node* theNode)
{
	*((float*)theNode->getDataPointer(2)) =
		*((float*)theNode->getDataPointer(0)) *
		*((float*)theNode->getDataPointer(1));
}

void nodeFunctionality::FloatDivision(node* theNode)
{
	*((float*)theNode->getDataPointer(2)) =
		*((float*)theNode->getDataPointer(0)) /
		*((float*)theNode->getDataPointer(1));
}

void nodeFunctionality::Color(node* theNode)
{
	*((sf::Color*)theNode->getDataPointer(1)) = *((sf::Color*)theNode->getDataPointer(0));
}

void nodeFunctionality::ColorFromInts(node* theNode)
{
	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointer(4));
	outputPointer->r = *((int*)theNode->getDataPointer(0));
	outputPointer->g = *((int*)theNode->getDataPointer(1));
	outputPointer->b = *((int*)theNode->getDataPointer(2));
	outputPointer->a = *((int*)theNode->getDataPointer(3));
}

void nodeFunctionality::Image(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	imageShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());
	rs.shader = &imageShader;
	outputPointer->draw(spr, rs);
	*outputSize = sf::Vector2i(size.x, size.y);
}

void nodeFunctionality::SeparateChannels(node* theNode)
{
	sf::RenderTexture* outputPointerR = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* outputPointerG = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* outputPointerB = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* outputPointerA = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0)); //inputPins[0]->getData());

	sf::Vector2u size = a->getSize();

	outputPointerR->create(size.x, size.y);
	outputPointerG->create(size.x, size.y);
	outputPointerB->create(size.x, size.y);
	outputPointerA->create(size.x, size.y);

	rs.shader = &separateShader;
	separateShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());

	separateShader.setUniform("mode", 0);
	outputPointerR->draw(spr, rs);
	separateShader.setUniform("mode", 1);
	outputPointerG->draw(spr, rs);
	separateShader.setUniform("mode", 2);
	outputPointerB->draw(spr, rs);
	separateShader.setUniform("mode", 3);
	outputPointerA->draw(spr, rs);
}

void nodeFunctionality::CombineChannels(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* red = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* green = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* blue = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* alpha = ((sf::RenderTexture*) theNode->getDataPointer(3));

	sf::Vector2u size = red->getSize();

	outputPointer->create(size.x, size.y);

	combineShader.setUniform("r", red->getTexture());
	combineShader.setUniform("g", green->getTexture());
	combineShader.setUniform("b", blue->getTexture());
	combineShader.setUniform("a", alpha->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &combineShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Solid(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	sf::Glsl::Vec4 inColor(*((sf::Color*)(theNode->getDataPointer(0))));
	solidShader.setUniform("color", inColor);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &solidShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Checker(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	checkerShader.setUniform("squareSize", (float)(*((int*)theNode->getDataPointer(0))));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &checkerShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::LinearGradient(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(0));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	linearGradientShader.setUniform("xResolution", (float)imageSize->x);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &linearGradientShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Blend(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointer(1));
	int mode = *((int*)theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	blendShader.setUniform("tex0", a->getTexture());
	blendShader.setUniform("tex1", b->getTexture());
	blendShader.setUniform("mode", (float)mode);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &blendShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::BrightnessContrast(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	float* brightness = ((float*)theNode->getDataPointer(1));
	float* contrast = ((float*)theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	rs.shader = &brightnessContrastShader;
	brightnessContrastShader.setUniform("tex", a->getTexture());
	brightnessContrastShader.setUniform("b", *brightness);
	brightnessContrastShader.setUniform("c", *contrast);

	sf::Sprite spr(a->getTexture());

	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Crop(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(2));
	if (outputSize->x < 1 || outputSize->y < 1)
		return;
	sf::Vector2i* topleft = ((sf::Vector2i*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(outputSize->x, outputSize->y);
	float top = topleft->y / ((float) size.y);
	float left = topleft->x / ((float) size.x);
	

	cropShader.setUniform("tex", a->getTexture());
	cropShader.setUniform("topLeft", sf::Glsl::Vec2(left, top));
	cropShader.setUniform("ratio", sf::Glsl::Vec2(size.x / ((float)outputSize->x),
												  size.y / ((float)outputSize->y)));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &cropShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Flip(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0)); //inputPins[0]->getData());
	int* xAxis = ((int*)theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	flipShader.setUniform("tx", a->getTexture());
	flipShader.setUniform("xAxis", *xAxis % 2);
	//std::cout << "xAxis % 2 = " << *xAxis % 2 << std::endl;

	sf::Sprite spr(a->getTexture());
	rs.shader = &flipShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Frame(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int* frame = ((int*)theNode->getDataPointer(1));
	sf::Color* color = ((sf::Color*) theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();
	sf::Vector2f fFrame;
	if (*frame < 0)
	{
		outputPointer->create(size.x - *frame * 2, size.y - *frame * 2);
		fFrame.x = (float)*frame / (float)size.x * -1.0f;
		fFrame.y = (float)*frame / (float)size.y * -1.0f;
	}
	else if (*frame > 0)
	{
		outputPointer->create(size.x + *frame * 2, size.y + *frame * 2);
		fFrame.x = (float)*frame / (float)size.x;
		fFrame.y = (float)*frame / (float)size.y;
	}
	else
		return;

	frameShader.setUniform("tex", a->getTexture());
	frameShader.setUniform("frame", sf::Glsl::Vec2(fFrame));
	frameShader.setUniform("color", sf::Glsl::Vec4(*color));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &frameShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Grayscale(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	grayscaleShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &grayscaleShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::GammaCorrection(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	gammaCorrectionShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &gammaCorrectionShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Invert(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();
	outputPointer->create(size.x, size.y);

	invertShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &invertShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Mask(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* fac = ((sf::RenderTexture*) theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	maskShader.setUniform("tex0", a->getTexture());
	maskShader.setUniform("fac", fac->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &maskShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Repeat(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::Vector2i* newSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	sf::Vector2u aSize = a->getSize();
	sf::Vector2f originalSize(aSize.x, aSize.y);
	outputPointer->create(newSize->x, newSize->y);

	repeatShader.setUniform("tex", a->getTexture());
	repeatShader.setUniform("originalSize", sf::Glsl::Vec2(originalSize));
	repeatShader.setUniform("newSize", sf::Glsl::Vec2(newSize->x, newSize->y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &repeatShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Rotate90(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int* times = ((int*)theNode->getDataPointer(1));
	int fixed = *times < 0 ? *times * -1 + 2 : *times;

	sf::Vector2u size = a->getSize();

	if (fixed % 2 == 1)
		outputPointer->create(size.y, size.x);
	else
		outputPointer->create(size.x, size.y);

	rotate90Shader.setUniform("tx", a->getTexture());
	rotate90Shader.setUniform("times", (float)fixed);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &rotate90Shader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::SelectByColor(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* inImage = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::Glsl::Vec4 inColor(*((sf::Color*)(theNode->getDataPointer(1))));
	float* inTolerance = ((float*) theNode->getDataPointer(2));

	sf::Vector2u size = inImage->getSize();
	outputPointer->create(size.x, size.y);

	selectByColorShader.setUniform("tx", inImage->getTexture());
	selectByColorShader.setUniform("color", inColor);
	selectByColorShader.setUniform("tolerance", *inTolerance);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &selectByColorShader;
	outputPointer->draw(spr, rs);
}
