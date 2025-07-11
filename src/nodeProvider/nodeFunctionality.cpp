#include "nodeFunctionality.h"
#include <iostream>
#include <cmath>
#include "../pathUtils.h"
#include "../math/nooseMath.h"
#include "../utils.h"

namespace nodeFunctionality {

	sf::Shader imageShader;
	sf::Shader blendShader;
	sf::Shader brightnessContrastShader;
	sf::Shader checkerShader;
	sf::Shader flipShader;
	sf::Shader frameShader;
	sf::Shader invertShader;
	sf::Shader linearGradientShader;
	sf::Shader uniformNoiseShader;
	sf::Shader repeatShader;
	sf::Shader rotateShader;
	sf::Shader rotate90Shader;
	sf::Shader flatBlurShader;
	sf::Shader solidShader;
	sf::Shader maskShader;
	sf::Shader grayscaleShader;
	sf::Shader gammaCorrectionShader;
	sf::Shader separateShader;
	sf::Shader combineShader;
	sf::Shader cropShader;
	sf::Shader extendShader;
	sf::Shader patchShader;
	sf::Shader selectByColorShader;
	sf::Shader diffShader;
	sf::Shader findShader;
	sf::Shader thresholdShader;
	sf::Shader deltaShader;
	sf::Shader heightmapToNormalmapShader;

	sf::RenderStates rs;
}

void nodeFunctionality::initialize()
{
	rs.blendMode = sf::BlendNone;

	LOAD_NODE_SHADER(image);
	LOAD_NODE_SHADER(blend);
	LOAD_NODE_SHADER(brightnessContrast);
	LOAD_NODE_SHADER(checker);
	LOAD_NODE_SHADER(flip);
	LOAD_NODE_SHADER(frame);
	LOAD_NODE_SHADER(invert);
	LOAD_NODE_SHADER(linearGradient);
	LOAD_NODE_SHADER(uniformNoise);
	LOAD_NODE_SHADER(repeat);
	LOAD_NODE_SHADER(rotate);
	LOAD_NODE_SHADER(rotate90);
	LOAD_NODE_SHADER(flatBlur);
	LOAD_NODE_SHADER(solid);
	LOAD_NODE_SHADER(mask);
	LOAD_NODE_SHADER(grayscale);
	LOAD_NODE_SHADER(gammaCorrection);
	LOAD_NODE_SHADER(separate);
	LOAD_NODE_SHADER(combine);
	LOAD_NODE_SHADER(crop);
	LOAD_NODE_SHADER(extend);
	LOAD_NODE_SHADER(patch);
	LOAD_NODE_SHADER(selectByColor);
	LOAD_NODE_SHADER(diff);
	LOAD_NODE_SHADER(find);
	LOAD_NODE_SHADER(threshold);
	LOAD_NODE_SHADER(delta);
	LOAD_NODE_SHADER(heightmapToNormalmap);
}

void nodeFunctionality::ImageFromFile(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ size.x, size.y });

	imageShader.setUniform("tx", a->getTexture());
	imageShader.setUniform("flip", 0);

	sf::Sprite spr(a->getTexture());
	rs.shader = &imageShader;
	outputPointer->draw(spr, rs);
	*outputSize = sf::Vector2i(size.x, size.y);
}

void nodeFunctionality::Solid(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)imageSize->x, (unsigned int)imageSize->y});

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

	*outputPointer = sf::RenderTexture({ (unsigned int)imageSize->x, (unsigned int)imageSize->y});

	checkerShader.setUniform("squareSize", (float)(*((int*)theNode->getDataPointer(0))));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &checkerShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::LinearGradient(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(0));
	int vertical = *((int*)theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)imageSize->x, (unsigned int)imageSize->y});

	linearGradientShader.setUniform("vertical", (float) vertical);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &linearGradientShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::UniformNoise(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*)theNode->getDataPointer(0));
	int seed = *((int*)theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)imageSize->x, (unsigned int)imageSize->y});

	uniformNoiseShader.setUniform("seed", seed);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &uniformNoiseShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::SeparateChannels(node* theNode)
{
	sf::RenderTexture* outputPointerR = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* outputPointerG = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* outputPointerB = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* outputPointerA = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0)); //inputPins[0]->getData());

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointerR = sf::RenderTexture({ size.x, size.y });
	*outputPointerG = sf::RenderTexture({ size.x, size.y });
	*outputPointerB = sf::RenderTexture({ size.x, size.y });
	*outputPointerA = sf::RenderTexture({ size.x, size.y });

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
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	combineShader.setUniform("r", red->getTexture());
	combineShader.setUniform("g", green->getTexture());
	combineShader.setUniform("b", blue->getTexture());
	combineShader.setUniform("a", alpha->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &combineShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Blend(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointer(1));
	int mode = *((int*)theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

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
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

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
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)outputSize->x, (unsigned int)outputSize->y});
	float top = topleft->y / ((float)size.y);
	float left = topleft->x / ((float)size.x);


	cropShader.setUniform("tex", a->getTexture());
	cropShader.setUniform("topLeft", sf::Glsl::Vec2(left, top));
	cropShader.setUniform("ratio", sf::Glsl::Vec2(size.x / ((float)outputSize->x),
		size.y / ((float)outputSize->y)));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &cropShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Extend(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int side = *((int*)theNode->getDataPointer(1)) % 4;
	int pixels = *((int*)theNode->getDataPointer(2));
	sf::Color color = *((sf::Color*)theNode->getDataPointer(3));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	float ratio;
	if (side == 1 || side == 3)
	{
		*outputPointer = sf::RenderTexture({size.x + pixels, size.y});
		ratio = (float)pixels / (float)size.x;
	}
	else
	{
		*outputPointer = sf::RenderTexture({size.x, size.y + pixels});
		ratio = (float)pixels / (float)size.y;
	}

	extendShader.setUniform("tx", a->getTexture());
	extendShader.setUniform("side", (float)side);
	extendShader.setUniform("ratio", (float)ratio);
	extendShader.setUniform("color", sf::Glsl::Vec4(color));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &extendShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Patch(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* image = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* patch = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* position = ((sf::Vector2i*) theNode->getDataPointer(2));

	sf::Vector2u imageSize = image->getSize();
	sf::Vector2u patchSize = patch->getSize();
	if (imageSize.x < 1 || imageSize.y < 1)
		return;

	*outputPointer = sf::RenderTexture({imageSize.x, imageSize.y});

	float top = (float) position->y / (float) imageSize.y;
	float left = (float) position->x / (float) imageSize.x;

	float ratioX = (float) patchSize.x / (float) imageSize.x;
	float ratioY = (float) patchSize.y / (float) imageSize.y;

	patchShader.setUniform("tx", image->getTexture());
	patchShader.setUniform("patchTx", patch->getTexture());
	patchShader.setUniform("topLeft", sf::Glsl::Vec2(left, top));
	patchShader.setUniform("ratio", sf::Glsl::Vec2(ratioX, ratioY));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &patchShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Flip(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int vertically = *((int*)theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	flipShader.setUniform("tx", a->getTexture());
	flipShader.setUniform("vertically", (float) vertically);

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
		if (size.x - *frame * 2 < 1 || size.y - *frame * 2 < 1)
			return;
		*outputPointer = sf::RenderTexture({size.x - *frame * 2, size.y - *frame * 2});
		fFrame.x = (float)*frame / (float)size.x * -1.0f;
		fFrame.y = (float)*frame / (float)size.y * -1.0f;
	}
	else if (*frame > 0)
	{
		if (size.x + *frame * 2 < 1 || size.y + *frame * 2 < 1)
			return;
		*outputPointer = sf::RenderTexture({size.x + *frame * 2, size.y + *frame * 2});
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
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	grayscaleShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &grayscaleShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::GammaCorrection(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	gammaCorrectionShader.setUniform("tex", a->getTexture());
	gammaCorrectionShader.setUniform("gamma", *((float*) theNode->getDataPointer(1)));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &gammaCorrectionShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Invert(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

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
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

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
	if ((unsigned int)newSize->x < 1 || (unsigned int)newSize->y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)newSize->x, (unsigned int)newSize->y});

	repeatShader.setUniform("tex", a->getTexture());
	repeatShader.setUniform("originalSize", sf::Glsl::Vec2(originalSize));
	repeatShader.setUniform("newSize", sf::Glsl::Vec2(newSize->x, newSize->y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &repeatShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Rotate(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	float radians = *((float*) theNode->getDataPointer(1));

	sf::Vector2u aSize = a->getSize();
	sf::Vector2f originalSize(aSize.x, aSize.y);
	float diagonal = std::ceilf(nooseMath::length(originalSize));
	sf::Vector2f newSize(diagonal, diagonal);
	if ((unsigned int)newSize.x < 1 || (unsigned int)newSize.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)newSize.x, (unsigned int)newSize.y});

	a->setSmooth(true);
	rotateShader.setUniform("tex", a->getTexture());
	rotateShader.setUniform("angle", radians);
	rotateShader.setUniform("originalSize", sf::Glsl::Vec2(originalSize));
	rotateShader.setUniform("newSize", sf::Glsl::Vec2(newSize.x, newSize.y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &rotateShader;
	outputPointer->draw(spr, rs);
	a->setSmooth(false);
}

void nodeFunctionality::Rotate90(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int* times = ((int*)theNode->getDataPointer(1));
	int fixed = *times < 0 ? *times * -1 + 2 : *times;

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	if (fixed % 2 == 1)
		*outputPointer = sf::RenderTexture({size.y, size.x});
	else
		*outputPointer = sf::RenderTexture({size.x, size.y});

	rotate90Shader.setUniform("tx", a->getTexture());
	rotate90Shader.setUniform("times", (float)fixed);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &rotate90Shader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Scale(node* theNode)
{
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(4));
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	float* factor = ((float*)theNode->getDataPointer(1));
	bool linearSamplingIfTrueOtherwiseNearest = nooseMath::mod(*((int*)theNode->getDataPointer(2)), 2) == 0;

	if (!a->generateMipmap()) // generate mipmap for better minification
		std::cout << "[Node provider] Scale node failed to generate mipmap\n";

	outputSize->x = a->getSize().x * *factor;
	outputSize->y = a->getSize().y * *factor;
	if ((unsigned int)outputSize->x < 1 || (unsigned int)outputSize->y < 1)
		return;

	*outputPointer = sf::RenderTexture({ (unsigned int)outputSize->x, (unsigned int)outputSize->y});

	a->setSmooth(linearSamplingIfTrueOtherwiseNearest);
	imageShader.setUniform("tx", a->getTexture());
	imageShader.setUniform("flip", 0);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &imageShader;
	outputPointer->draw(spr, rs);
	a->setSmooth(false);
}

void nodeFunctionality::SelectByColor(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* inImage = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::Glsl::Vec4 inColor(*((sf::Color*) (theNode->getDataPointer(1))));
	float* inTolerance = ((float*) theNode->getDataPointer(2));

	sf::Vector2u size = inImage->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	selectByColorShader.setUniform("tx", inImage->getTexture());
	selectByColorShader.setUniform("color", inColor);
	selectByColorShader.setUniform("tolerance", *inTolerance);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &selectByColorShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::FlatBlur(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* inImage = ((sf::RenderTexture*) theNode->getDataPointer(0));
	float inRadius = *((float*) theNode->getDataPointer(1));
	int inSamples = *((int*) theNode->getDataPointer(2));

	sf::Vector2u size = inImage->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({size.x, size.y});

	flatBlurShader.setUniform("tx", inImage->getTexture());
	flatBlurShader.setUniform("radius", inRadius);
	flatBlurShader.setUniform("samples", inSamples);
	flatBlurShader.setUniform("resolution", sf::Glsl::Vec2(size.x, size.y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &flatBlurShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Diff(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*)theNode->getDataPointer(0));
	sf::RenderTexture* b = ((sf::RenderTexture*)theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ size.x, size.y });

	diffShader.setUniform("tex0", a->getTexture());
	diffShader.setUniform("tex1", b->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &diffShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Find(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*)theNode->getDataPointer(0));
	sf::RenderTexture* b = ((sf::RenderTexture*)theNode->getDataPointer(1));

	sf::Vector2u aSize = a->getSize();
	sf::Vector2u bSize = b->getSize();
	if (aSize.x < 1 || aSize.y < 1 || bSize.x < 1 || bSize.y < 1)
		return;
	*outputPointer = sf::RenderTexture({ aSize.x, aSize.y });

	findShader.setUniform("image", a->getTexture());
	findShader.setUniform("kernel", b->getTexture());
	findShader.setUniform("imageSize", sf::Glsl::Vec2(aSize.x, aSize.y));
	findShader.setUniform("kernelSize", sf::Glsl::Vec2(bSize.x, bSize.y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &findShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Threshold(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(2));
	sf::RenderTexture* inImage = ((sf::RenderTexture*)theNode->getDataPointer(0));
	float* inThreshold = ((float*)theNode->getDataPointer(1));

	sf::Vector2u size = inImage->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ size.x, size.y });

	thresholdShader.setUniform("tx", inImage->getTexture());
	thresholdShader.setUniform("threshold", *inThreshold);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &thresholdShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Delta(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(1));
	sf::RenderTexture* inImage = ((sf::RenderTexture*)theNode->getDataPointer(0));

	sf::Vector2u size = inImage->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ size.x, size.y });

	deltaShader.setUniform("tx", inImage->getTexture());
	deltaShader.setUniform("size", sf::Glsl::Vec2(size.x, size.y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &deltaShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::HeightmapToNormalmap(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(3));
	sf::RenderTexture* inImage = ((sf::RenderTexture*)theNode->getDataPointer(0));
	float* inMaxHeight = ((float*)theNode->getDataPointer(1));
	int* inMode= ((int*)theNode->getDataPointer(2));

	sf::Vector2u size = inImage->getSize();
	if (size.x < 1 || size.y < 1)
		return;

	*outputPointer = sf::RenderTexture({ size.x, size.y });

	heightmapToNormalmapShader.setUniform("tx", inImage->getTexture());
	heightmapToNormalmapShader.setUniform("size", sf::Glsl::Vec2(size.x, size.y));
	heightmapToNormalmapShader.setUniform("maxHeight", *inMaxHeight);
	heightmapToNormalmapShader.setUniform("mode", *inMode);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &heightmapToNormalmapShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Color(node* theNode)
{
	*((sf::Color*)theNode->getDataPointer(1)) = *((sf::Color*)theNode->getDataPointer(0));
}

void nodeFunctionality::ColorFromRGBAInts(node* theNode)
{
	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointer(4));
	outputPointer->r = *((int*)theNode->getDataPointer(0));
	outputPointer->g = *((int*)theNode->getDataPointer(1));
	outputPointer->b = *((int*)theNode->getDataPointer(2));
	outputPointer->a = *((int*)theNode->getDataPointer(3));
}

void nodeFunctionality::RGBAIntsFromColor(node* theNode)
{
	sf::Color* inputPointer = ((sf::Color*)theNode->getDataPointer(0));
	*((int*)theNode->getDataPointer(1)) = inputPointer->r;
	*((int*)theNode->getDataPointer(2)) = inputPointer->g;
	*((int*)theNode->getDataPointer(3)) = inputPointer->b;
	*((int*)theNode->getDataPointer(4)) = inputPointer->a;
}

void nodeFunctionality::ColorFromString(node* theNode)
{
	std::string* inStringPointer = ((std::string*)theNode->getDataPointer(0));
	sf::Color* outColorPointer = ((sf::Color*)theNode->getDataPointer(1));
	utils::colorFromHexString(*inStringPointer, *outColorPointer);
}

void nodeFunctionality::ColorFromImage(node* theNode)
{
	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointer(2));
	sf::RenderTexture* inImage = ((sf::RenderTexture*)theNode->getDataPointer(0));
	sf::Vector2i* inCoords = ((sf::Vector2i*)(theNode->getDataPointer(1)));

	if (inImage->getSize().x < 1 || inImage->getSize().y < 1)
		return;

	unsigned int xCoord = nooseMath::mod(inCoords->x, inImage->getSize().x);
	unsigned int yCoord = nooseMath::mod(inCoords->y, inImage->getSize().y);

	*outputPointer = inImage->getTexture().copyToImage().getPixel({ xCoord, yCoord });
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

void nodeFunctionality::SeparateVector2i(node* theNode)
{
	sf::Vector2i* input = (sf::Vector2i*) theNode->getDataPointer(0);
	*((int*)theNode->getDataPointer(1)) = input->x;
	*((int*)theNode->getDataPointer(2)) = input->y;
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
	if (*((int*)theNode->getDataPointer(1)) == 0)
	{
		std::cout << "[Node provider] Cannot divide by zero\n";
		return;
	}

	*((int*)theNode->getDataPointer(2)) =
		*((int*)theNode->getDataPointer(0)) /
		*((int*)theNode->getDataPointer(1));
}

void nodeFunctionality::IntegerMax(node* theNode)
{
	int a = *((int*)theNode->getDataPointer(0));
	int b = *((int*)theNode->getDataPointer(1));
	*((int*)theNode->getDataPointer(2)) = a > b ? a : b;
}

void nodeFunctionality::IntegerMin(node* theNode)
{
	int a = *((int*)theNode->getDataPointer(0));
	int b = *((int*)theNode->getDataPointer(1));
	*((int*)theNode->getDataPointer(2)) = a < b ? a : b;
}

void nodeFunctionality::IntegerAbs(node* theNode)
{
	int in = *((int*)theNode->getDataPointer(0));
	*((int*)theNode->getDataPointer(1)) = in < 0 ? -in : in;
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

void nodeFunctionality::FloatMax(node* theNode)
{
	float a = *((float*)theNode->getDataPointer(0));
	float b = *((float*)theNode->getDataPointer(1));
	*((float*)theNode->getDataPointer(2)) = a > b ? a : b;
}

void nodeFunctionality::FloatMin(node* theNode)
{
	float a = *((float*)theNode->getDataPointer(0));
	float b = *((float*)theNode->getDataPointer(1));
	*((float*)theNode->getDataPointer(2)) = a < b ? a : b;
}

void nodeFunctionality::FloatAbs(node* theNode)
{
	float in = *((float*)theNode->getDataPointer(0));
	*((float*)theNode->getDataPointer(1)) = in < 0 ? -in : in;
}

void nodeFunctionality::String(node* theNode)
{
	*((std::string*)theNode->getDataPointer(1)) = *((std::string*)theNode->getDataPointer(0));
}

void nodeFunctionality::Font(node* theNode)
{
	*((sf::Font*)theNode->getDataPointer(1)) = *((sf::Font*)theNode->getDataPointer(0));
}

void nodeFunctionality::ImageFromText(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*)theNode->getDataPointer(4));
	sf::Font* inFontPointer = ((sf::Font*)theNode->getDataPointer(0));
	int* inSizePointer = ((int*)theNode->getDataPointer(1));
	sf::Color* inColorPointer = ((sf::Color*)theNode->getDataPointer(2));
	std::string* inStringPointer = ((std::string*)theNode->getDataPointer(3));

	sf::Text tempText(*inFontPointer, *inStringPointer, *inSizePointer);
	tempText.setStyle(sf::Text::Regular);
	tempText.setFillColor(*inColorPointer);

	sf::FloatRect bounds = tempText.getGlobalBounds();
	int outputWidth = ((int)(bounds.size.x + bounds.position.x)) + 1;
	int outputHeight = ((int)(bounds.size.y + bounds.position.y)) + 1;
	if (outputWidth < 1 || outputHeight < 1)
		return;

	sf::RenderTexture tempTexture;
	*outputPointer = sf::RenderTexture({ (unsigned int)outputWidth, (unsigned int)outputHeight});
	tempTexture = sf::RenderTexture({ (unsigned int)outputWidth, (unsigned int)outputHeight });
	sf::Color clearColor = sf::Color::Transparent;
	clearColor.r = inColorPointer->r;
	clearColor.g = inColorPointer->g;
	clearColor.b = inColorPointer->b;
	tempTexture.clear(clearColor);
	tempTexture.draw(tempText);

	sf::Sprite spr(tempTexture.getTexture());
	imageShader.setUniform("flip", 1);
	imageShader.setUniform("tx", tempTexture.getTexture());
	rs.shader = &imageShader;
	outputPointer->draw(spr, rs);
}