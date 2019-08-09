#include "nodeFunctionality.h"
#include "dataController.h"

sf::Shader checkerShader;
sf::Shader flipShader;
sf::Shader invertShader;
sf::Shader linearGradientShader;
sf::Shader multiplyShader;
sf::Shader repeatShader;
sf::Shader rotate90Shader;

void nodeFunctionality::initialize()
{
	if (!checkerShader.loadFromFile("res/nodeShaders/checker.shader", sf::Shader::Fragment))
		std::cout << "could not load checker shader\n";
	if (!flipShader.loadFromFile("res/nodeShaders/flip.shader", sf::Shader::Fragment))
		std::cout << "could not load flip shader\n";
	if (!invertShader.loadFromFile("res/nodeShaders/invert.shader", sf::Shader::Fragment))
		std::cout << "could not load invert shader\n";
	if (!linearGradientShader.loadFromFile("res/nodeShaders/linearGradient.shader", sf::Shader::Fragment))
		std::cout << "could not load linearGradient shader\n";
	if (!multiplyShader.loadFromFile("res/nodeShaders/multiply.shader", sf::Shader::Fragment))
		std::cout << "could not load multiply shader\n";
	if (!repeatShader.loadFromFile("res/nodeShaders/repeat.shader", sf::Shader::Fragment))
		std::cout << "could not load repeat shader\n";
	if (!rotate90Shader.loadFromFile("res/nodeShaders/rotate90.shader", sf::Shader::Fragment))
		std::cout << "could not load rotate90 shader\n";
}

void nodeFunctionality::Checker(uiNode* theNode)
{
	std::cout << "executing checker" << std::endl;
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointerForPin(0, true));

	outputPointer->create(imageSize->x, imageSize->y);

	checkerShader.setUniform("squareSize", (float)(*((int*)theNode->getDataPointerForPin(1, true))));

	sf::Sprite spr(outputPointer->getTexture());
	outputPointer->draw(spr, &checkerShader);
}

void nodeFunctionality::ConstructColor(uiNode* theNode)
{
	std::cout << "executing construct color" << std::endl;
	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointerForPin(4, false));
	outputPointer->r = *((int*)theNode->getDataPointerForPin(0, true));
	outputPointer->g = *((int*)theNode->getDataPointerForPin(1, true));
	outputPointer->b = *((int*)theNode->getDataPointerForPin(2, true));
	outputPointer->a = *((int*)theNode->getDataPointerForPin(3, true));
}

void nodeFunctionality::ConstructVector2i(uiNode* theNode)
{
	std::cout << "executing construct vector" << std::endl;
	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointerForPin(2, false));
	outputPointer->x = *((int*)(theNode->getDataPointerForPin(0, true)));
	outputPointer->y = *((int*)(theNode->getDataPointerForPin(1, true)));
}

void nodeFunctionality::Flip(uiNode* theNode)
{
	std::cout << "executing flip" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true)); //inputPins[0]->getData());
	int* xAxis = ((int*)theNode->getDataPointerForPin(1, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	//shaders[6].setParameter("tx", a->getTexture());
	//shaders[6].setParameter("xAxis", *xAxis % 2);
	flipShader.setUniform("tx", a->getTexture());
	flipShader.setUniform("xAxis", *xAxis % 2);
	std::cout << "xAxis % 2 = " << *xAxis % 2 << std::endl;

	sf::Sprite spr(a->getTexture());
	//outputPointer->draw(spr, &shaders[6]);
	outputPointer->draw(spr, &flipShader);
}

void nodeFunctionality::Float(uiNode* theNode)
{
	std::cout << "executing float" << std::endl;
	*((float*)theNode->getDataPointerForPin(1, false)) = *((float*)theNode->getDataPointerForPin(0, true));
}

void nodeFunctionality::Image(uiNode* theNode)
{
	std::cout << "executing image" << std::endl;
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointerForPin(2, true));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	dataController::loadImageShader.setUniform("tx", a->getTexture());
//	dataController::loadImageShader.setParameter("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());
	outputPointer->draw(spr, &dataController::loadImageShader);
	*outputSize = sf::Vector2i(size.x, size.y);
}

void nodeFunctionality::Integer(uiNode* theNode)
{
	std::cout << "executing integer" << std::endl;
	*((int*)theNode->getDataPointerForPin(1, false)) = *((int*)theNode->getDataPointerForPin(0, true));
}

void nodeFunctionality::Invert(uiNode* theNode)
{
	std::cout << "executing invert" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));

	sf::Vector2u size = a->getSize();
	outputPointer->create(size.x, size.y);

	invertShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	outputPointer->draw(spr, &invertShader);
}

void nodeFunctionality::LinearGradient(uiNode* theNode)
{
	std::cout << "executing linear gradient" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, false));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointerForPin(0, true));

	std::cout << "creating image of size " << imageSize->x << ", " << imageSize->y << std::endl;
	outputPointer->create(imageSize->x, imageSize->y);

	linearGradientShader.setUniform("xResolution", (float)imageSize->x);
	//linearGradientShader.setParameter("xResolution", imageSize->x);

	sf::Sprite spr(outputPointer->getTexture());
	outputPointer->draw(spr, &linearGradientShader);
}

void nodeFunctionality::Multiply(uiNode* theNode)
{
	std::cout << "executing multiply" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointerForPin(2, false));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointerForPin(0, true));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointerForPin(1, true));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	multiplyShader.setUniform("tex0", a->getTexture());
	multiplyShader.setUniform("tex1", b->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	outputPointer->draw(spr, &multiplyShader);
}

void nodeFunctionality::Output(uiNode* theNode)
{
	std::cout << "executing output node (doing nothing)\n";
}

void nodeFunctionality::Repeat(uiNode* theNode)
{
	std::cout << "executing repeat" << std::endl;

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
	outputPointer->draw(spr, &repeatShader);
}

void nodeFunctionality::Rotate90(uiNode* theNode)
{
	std::cout << "executing rotate 90" << std::endl;
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
	outputPointer->draw(spr, &rotate90Shader);
}