#include "nodeFunctionality.h"
#include "dataController.h"

sf::Shader blendShader;
sf::Shader checkerShader;
sf::Shader flipShader;
sf::Shader invertShader;
sf::Shader linearGradientShader;
sf::Shader repeatShader;
sf::Shader rotate90Shader;
sf::Shader solidShader;
sf::Shader mixShader;
sf::Shader grayscaleShader;
sf::Shader decomposeShader;
sf::Shader cropShader;

//sf::BlendMode blendMode;
sf::RenderStates rs;

void nodeFunctionality::initialize()
{
	/*blendMode.alphaEquation = sf::BlendMode::Add;
	blendMode.alphaSrcFactor = sf::BlendMode::One;
	blendMode.alphaDstFactor = sf::BlendMode::Zero;
	rs.blendMode = blendMode;*/
	rs.blendMode = sf::BlendNone;

	std::cout << "loading shaders\n";
	if (!blendShader.loadFromFile("res/nodeShaders/blend.shader", sf::Shader::Fragment))
		std::cout << "could not load blend shader\n";
	if (!checkerShader.loadFromFile("res/nodeShaders/checker.shader", sf::Shader::Fragment))
		std::cout << "could not load checker shader\n";
	if (!flipShader.loadFromFile("res/nodeShaders/flip.shader", sf::Shader::Fragment))
		std::cout << "could not load flip shader\n";
	if (!invertShader.loadFromFile("res/nodeShaders/invert.shader", sf::Shader::Fragment))
		std::cout << "could not load invert shader\n";
	if (!linearGradientShader.loadFromFile("res/nodeShaders/linearGradient.shader", sf::Shader::Fragment))
		std::cout << "could not load linearGradient shader\n";
	if (!repeatShader.loadFromFile("res/nodeShaders/repeat.shader", sf::Shader::Fragment))
		std::cout << "could not load repeat shader\n";
	if (!rotate90Shader.loadFromFile("res/nodeShaders/rotate90.shader", sf::Shader::Fragment))
		std::cout << "could not load rotate90 shader\n";
	if (!solidShader.loadFromFile("res/nodeShaders/solid.shader", sf::Shader::Fragment))
		std::cout << "could not load solidShader shader\n";
	if (!mixShader.loadFromFile("res/nodeShaders/mix.shader", sf::Shader::Fragment))
		std::cout << "could not load mix shader\n";
	if (!grayscaleShader.loadFromFile("res/nodeShaders/grayscale.shader", sf::Shader::Fragment))
		std::cout << "could not load grayscale shader\n";
	if (!decomposeShader.loadFromFile("res/nodeShaders/decompose.shader", sf::Shader::Fragment))
		std::cout << "could not load decompose shader\n";
	if (!cropShader.loadFromFile("res/nodeShaders/crop.shader", sf::Shader::Fragment))
		std::cout << "could not load crop shader\n";
}

void nodeFunctionality::Blend(uiNode* theNode)
{
	//std::cout << "executing blend" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(3, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, true));
	int mode = *((int*)theNode->getDataPointerForPin(2, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	blendShader.setUniform("tex0", a->getTexture());
	blendShader.setUniform("tex1", b->getTexture());
	blendShader.setUniform("mode", (float)mode);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &blendShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Checker(uiNode* theNode)
{
	//std::cout << "executing checker" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointerForPin(1, true));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	checkerShader.setUniform("squareSize", (float)(*((int*)theNode->getDataPointerForPin(0, true))));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &checkerShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::ConstructColor(uiNode* theNode)
{
	//std::cout << "executing construct color" << std::endl;

	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointerForPin(4, false));
	outputPointer->r = *((int*)theNode->getDataPointerForPin(0, true));
	outputPointer->g = *((int*)theNode->getDataPointerForPin(1, true));
	outputPointer->b = *((int*)theNode->getDataPointerForPin(2, true));
	outputPointer->a = *((int*)theNode->getDataPointerForPin(3, true));
}

void nodeFunctionality::ConstructVector2i(uiNode* theNode)
{
	//std::cout << "executing construct vector" << std::endl;

	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointerForPin(2, false));
	outputPointer->x = *((int*)(theNode->getDataPointerForPin(0, true)));
	outputPointer->y = *((int*)(theNode->getDataPointerForPin(1, true)));
}

void nodeFunctionality::Crop(uiNode* theNode)
{
	//std::cout << "executing crop" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(3, false));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointerForPin(2, true));
	sf::Vector2i* topleft = ((sf::Vector2i*) theNode->getDataPointerForPin(1, true));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(outputSize->x, outputSize->y);
	float top = topleft->y / ((float) size.y);
	float left = topleft->x / ((float) size.x);

	cropShader.setUniform("tex", a->getTexture());
	cropShader.setUniform("topLeft", sf::Glsl::Vec2(left, top));

	sf::Sprite spr(a->getTexture());
	rs.shader = &cropShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Decompose(uiNode* theNode)
{
	//std::cout << "executing decompose" << std::endl;
	
	sf::RenderTexture* outputPointerR = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::RenderTexture* outputPointerG = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* outputPointerB = ((sf::RenderTexture*) theNode->getDataPointerForPin(3, false));
	sf::RenderTexture* outputPointerA = ((sf::RenderTexture*) theNode->getDataPointerForPin(4, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true)); //inputPins[0]->getData());
	//int* rgOrB = ((int*)theNode->getDataPointerForPin(1, true));

	sf::Vector2u size = a->getSize();

	outputPointerR->create(size.x, size.y);
	outputPointerG->create(size.x, size.y);
	outputPointerB->create(size.x, size.y);
	outputPointerA->create(size.x, size.y);

	rs.shader = &decomposeShader;
	decomposeShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());

	decomposeShader.setUniform("mode", 0);
	outputPointerR->draw(spr, rs);
	decomposeShader.setUniform("mode", 1);
	outputPointerG->draw(spr, rs);
	decomposeShader.setUniform("mode", 2);
	outputPointerB->draw(spr, rs);
	decomposeShader.setUniform("mode", 3);
	outputPointerA->draw(spr, rs);
}

void nodeFunctionality::Flip(uiNode* theNode)
{
	//std::cout << "executing flip" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true)); //inputPins[0]->getData());
	int* xAxis = ((int*)theNode->getDataPointerForPin(1, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	flipShader.setUniform("tx", a->getTexture());
	flipShader.setUniform("xAxis", *xAxis % 2);
	std::cout << "xAxis % 2 = " << *xAxis % 2 << std::endl;

	sf::Sprite spr(a->getTexture());
	rs.shader = &flipShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Float(uiNode* theNode)
{
	//std::cout << "executing float" << std::endl;

	*((float*)theNode->getDataPointerForPin(1, false)) = *((float*)theNode->getDataPointerForPin(0, true));
}

void nodeFunctionality::Grayscale(uiNode* theNode)
{
	//std::cout << "executing grayscale" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	grayscaleShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &grayscaleShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Image(uiNode* theNode)
{
	//std::cout << "executing image" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	dataController::loadImageShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());
	rs.shader = &dataController::loadImageShader;
	outputPointer->draw(spr, rs);
	*outputSize = sf::Vector2i(size.x, size.y);
}

void nodeFunctionality::Integer(uiNode* theNode)
{
	//std::cout << "executing integer" << std::endl;

	*((int*)theNode->getDataPointerForPin(1, false)) = *((int*)theNode->getDataPointerForPin(0, true));
}

void nodeFunctionality::Invert(uiNode* theNode)
{
	//std::cout << "executing invert" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();
	outputPointer->create(size.x, size.y);

	invertShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &invertShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::LinearGradient(uiNode* theNode)
{
	//std::cout << "executing linear gradient" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointerForPin(0, true));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	//std::cout << "creating image of size " << imageSize->x << ", " << imageSize->y << std::endl;
	outputPointer->create(imageSize->x, imageSize->y);

	linearGradientShader.setUniform("xResolution", (float)imageSize->x);
	//linearGradientShader.setParameter("xResolution", imageSize->x);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &linearGradientShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Mix(uiNode* theNode)
{
	//std::cout << "executing mix" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(3, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, true));
	sf::RenderTexture* fac = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	mixShader.setUniform("tex0", a->getTexture());
	mixShader.setUniform("tex1", b->getTexture());
	mixShader.setUniform("fac", fac->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &mixShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Repeat(uiNode* theNode)
{
	//std::cout << "executing repeat" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));
	sf::Vector2i* newSize = ((sf::Vector2i*) theNode->getDataPointerForPin(1, true));

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

void nodeFunctionality::Rotate90(uiNode* theNode)
{
	//std::cout << "executing rotate 90" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));
	int* times = ((int*)theNode->getDataPointerForPin(1, true));
	int fixed = *times < 0 ? *times * -1 + 2 : *times;

	sf::Vector2u size = a->getSize();

	if (fixed % 2 == 1)
		outputPointer->create(size.y, size.x);
	else
		outputPointer->create(size.x, size.y);

	std::cout << fixed << std::endl;

	rotate90Shader.setUniform("tx", a->getTexture());
	rotate90Shader.setUniform("times", (float)fixed);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &rotate90Shader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Solid(uiNode* theNode)
{
	//std::cout << "executing solid" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointerForPin(1, true));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	sf::Glsl::Vec4 inColor(*((sf::Color*)(theNode->getDataPointerForPin(0, true))));
	solidShader.setUniform("color", inColor);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &solidShader;
	outputPointer->draw(spr, rs);
}