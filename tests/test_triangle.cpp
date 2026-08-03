#include <iostream>
#include <memory>

#include "runtime/editor/Editor.h"
#include "runtime/global/Engine.h"

std::shared_ptr<shzk::Scene> InitTestScene()
{
	auto transformComponent = std::make_shared<shzk::TransformComponent>();

	auto node = std::make_shared<shzk::Node>();
	node->m_name = "Triangle";
	node->AddComponent(transformComponent);

	auto scene = std::make_shared<shzk::Scene>();
	scene->AddNode(node);
	return scene;
}

int main()
{

	shzk::EngineInitInfo engineInitInfo{};
	engineInitInfo.name = "shzk";
	engineInitInfo.width = 1280;
	engineInitInfo.height = 720;

	shzk::Engine::Init(engineInitInfo);

	shzk::Scene scene = InitTestScene();
	shzk::Engine::Get()->SetActiveScene(scene);

	shzk::Editor* editor = new shzk::Editor();
	editor->Run();
	editor->Shutdown();

	return 0;
}