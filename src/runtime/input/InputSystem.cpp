#include "InputSystem.h"
#include "runtime/global/Engine.h"
#include "runtime/window/WindowSystem.h"
#include "runtime/log/Log.h"

#include <SDL3/SDL.h>

namespace shzk
{
	void InputSystem::Init(SDL_Window* window)
	{
		m_window = window;
	
		// SDL_SetWindowRelativeMouseMode(m_window, true);
	}

	void InputSystem::Tick()
	{
	
		for (auto& s : m_keyStates)	
			if (s == InputState::Press) s = InputState::Repeat;
		for (auto& s : m_mouseButtonStates)
			if (s == InputState::Press) s = InputState::Repeat;

		m_mouseDelta = { 0.f, 0.f };
		m_scrollDelta = { 0.f, 0.f };

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ProcessEvent(event);
		}

		if (m_keyStates[(size_t)KeyCode::Escape] == InputState::Press)
		{
			Engine::GetWindowSystem()->RequestClose();
		}
	}

	void InputSystem::Shutdown()
	{
		if (m_window)
		{
			SDL_SetWindowRelativeMouseMode(m_window, false);
		}
		m_window = nullptr;
	}

	void InputSystem::ProcessEvent(SDL_Event& event)
	{
		switch (event.type)
		{
		case SDL_EVENT_KEY_DOWN:
		{
			KeyCode kc = SDLScancodeToKeyCode(event.key.scancode);
			m_keyStates[(size_t)kc] = event.key.repeat ? InputState::Repeat : InputState::Press;
			// SHZK_LOG_INFO("key pressed {}", (size_t)kc);
			break;
		}
		case SDL_EVENT_KEY_UP:
		{
			m_keyStates[(size_t)SDLScancodeToKeyCode(event.key.scancode)] = InputState::Release;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			m_mouseButtonStates[(size_t)SDLButtonToMouseButton(event.button.button)] = InputState::Press;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			m_mouseButtonStates[(size_t)SDLButtonToMouseButton(event.button.button)] = InputState::Release;
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		{
			m_mouseDelta.x += event.motion.xrel;
			m_mouseDelta.y += event.motion.yrel;
			// SHZK_LOG_INFO("mouse motion: x:{}, y:{}", m_mouseDelta.x, m_mouseDelta.y);
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			m_scrollDelta.x += event.wheel.x;
			m_scrollDelta.y += event.wheel.y;
			break;
		}
		case SDL_EVENT_QUIT:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			Engine::GetWindowSystem()->RequestClose();
			break;
		}
	}
}