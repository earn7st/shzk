#include "application/Application.h"

#include "VKCommon.h"

void Application::Init()
{

	_window.Init();
	_renderer.Init(_window);

	const auto gltf = _assetManager.LoadGLTF("basicmesh", "assets/basicmesh.glb");
	if (gltf) {
		for (const auto& node : gltf->topNodes) {
			_scene.AddRoot(node);
		}
		_scene.RefreshTransforms();
	}
}

void Application::Run()
{
	SDL_Event e;
	bool bWindowShouldClose = false;
	while (!bWindowShouldClose)
	{
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_EVENT_QUIT) bWindowShouldClose = true;
		}

		_scene.RefreshTransforms();
		_renderer.Draw(_scene);
	}
	
}

void Application::Shutdown()
{
	_scene.Clear();
	_assetManager.Clear();
}
