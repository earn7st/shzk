#pragma once

#include <string>
#include "application/SDLWindow.h"
#include "render/Renderer.h"
#include "render/SceneRenderer.h"
#include "resource/ResourceManager.h"
#include "scene/Scene.h"

class Application
{

public:
	Application() = default;
	Application(const std::string& name, uint32_t width = 1920, uint32_t height = 1080);

	~Application() = default;

	void Init();
	void Run();

	void Shutdown();

private:
	SDLWindow window;
	Scene scene;
	ResourceManager resourceManager;
	Renderer renderer;
	SceneRenderer sceneRenderer;

	std::string applicationName;
	double deltaTime;

	void Tick(double deltaTime);
};