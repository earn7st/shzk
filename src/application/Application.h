#pragma once

#include <string>
#include "application/SDLWindow.h"
#include "render/Renderer.h"
#include "resource/ResourceManager.h"
#include "scene/Scene.h"

class Application
{

public:
	Application() = default;
	Application(const std::string& name, uint32_t width = 1920, uint32_t height = 1080);

	~Application() = default;

	void Initialize();
	void Run();
	void Tick(double deltaTime);
	void Shutdown();

private:
	SDLWindow window;
	ResourceManager resourceManager;
	Scene scene;
	Renderer renderer;

	std::string applicationName;
	double deltaTime;
};
