#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

struct SDL_Window;

namespace shzk
{
    enum class KeyCode : uint8_t
    {
        None = 0,

        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        Escape, Return, Tab, Backspace, Space,
        Insert, Delete, Home, End, PageUp, PageDown,
        Right, Left, Down, Up,
        CapsLock, ScrollLock, Pause, PrintScreen,

        Minus, Equals, LeftBracket, RightBracket, Backslash,
        Semicolon, Apostrophe, Grave, Comma, Period, Slash,

        LeftShift, RightShift, LeftCtrl, RightCtrl,
        LeftAlt, RightAlt, LeftSuper, RightSuper,

        Keypad0, Keypad1, Keypad2, Keypad3, Keypad4,
        Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
        KeypadDivide, KeypadMultiply, KeypadMinus, KeypadPlus,
        KeypadEnter, KeypadPeriod,

        MaxCount
    };

    enum class MouseButton : uint8_t	// Aligned with SDL
    {
        None = 0,
        Left, 
		Middle, 
		Right, 
		X1, 
		X2,

        MaxCount
    };

	enum class InputState : uint32_t
	{
		Release	= 0,
		Press   = 1,
		Repeat	= 2,

		Max,
	};

	class InputSystem
	{
	public:
		InputSystem() = default;
		~InputSystem() = default;

		void Init(SDL_Window* window);
		void Tick();
		void Shutdown();
		
        bool IsKeyDown(KeyCode k) const { return m_keyStates[(size_t)k] != InputState::Release; }
        bool IsKeyPressed(KeyCode k) const { return m_keyStates[(size_t)k] == InputState::Press; }
        bool IsMouseButtonDown(MouseButton b) const { return m_mouseButtonStates[(size_t)b] != InputState::Release; }
        bool IsMouseButtonPressed(MouseButton b) const { return m_mouseButtonStates[(size_t)b] == InputState::Press; }

        glm::vec2 GetMouseDelta() const { return m_mouseDelta; }
        glm::vec2 GetScrollDelta() const { return m_scrollDelta; }

	private:
        void ProcessEvent(SDL_Event& event);

		SDL_Window* m_window = nullptr;

        InputState m_keyStates[(size_t)KeyCode::MaxCount]               = { InputState::Release };
        InputState m_mouseButtonStates[(size_t)MouseButton::MaxCount]   = { InputState::Release };

        glm::vec2 m_mouseDelta  = { 0.f, 0.f };
        glm::vec2 m_scrollDelta = { 0.f, 0.f };
	};

    constexpr KeyCode SDLScancodeToKeyCode(SDL_Scancode code)
    {
        switch (code)
        {
        case SDL_SCANCODE_A: return KeyCode::A;
        case SDL_SCANCODE_B: return KeyCode::B;
        case SDL_SCANCODE_C: return KeyCode::C;
        case SDL_SCANCODE_D: return KeyCode::D;
        case SDL_SCANCODE_E: return KeyCode::E;
        case SDL_SCANCODE_F: return KeyCode::F;
        case SDL_SCANCODE_G: return KeyCode::G;
        case SDL_SCANCODE_H: return KeyCode::H;
        case SDL_SCANCODE_I: return KeyCode::I;
        case SDL_SCANCODE_J: return KeyCode::J;
        case SDL_SCANCODE_K: return KeyCode::K;
        case SDL_SCANCODE_L: return KeyCode::L;
        case SDL_SCANCODE_M: return KeyCode::M;
        case SDL_SCANCODE_N: return KeyCode::N;
        case SDL_SCANCODE_O: return KeyCode::O;
        case SDL_SCANCODE_P: return KeyCode::P;
        case SDL_SCANCODE_Q: return KeyCode::Q;
        case SDL_SCANCODE_R: return KeyCode::R;
        case SDL_SCANCODE_S: return KeyCode::S;
        case SDL_SCANCODE_T: return KeyCode::T;
        case SDL_SCANCODE_U: return KeyCode::U;
        case SDL_SCANCODE_V: return KeyCode::V;
        case SDL_SCANCODE_W: return KeyCode::W;
        case SDL_SCANCODE_X: return KeyCode::X;
        case SDL_SCANCODE_Y: return KeyCode::Y;
        case SDL_SCANCODE_Z: return KeyCode::Z;

        case SDL_SCANCODE_1: return KeyCode::Num1;
        case SDL_SCANCODE_2: return KeyCode::Num2;
        case SDL_SCANCODE_3: return KeyCode::Num3;
        case SDL_SCANCODE_4: return KeyCode::Num4;
        case SDL_SCANCODE_5: return KeyCode::Num5;
        case SDL_SCANCODE_6: return KeyCode::Num6;
        case SDL_SCANCODE_7: return KeyCode::Num7;
        case SDL_SCANCODE_8: return KeyCode::Num8;
        case SDL_SCANCODE_9: return KeyCode::Num9;
        case SDL_SCANCODE_0: return KeyCode::Num0;

        case SDL_SCANCODE_F1:  return KeyCode::F1;
        case SDL_SCANCODE_F2:  return KeyCode::F2;
        case SDL_SCANCODE_F3:  return KeyCode::F3;
        case SDL_SCANCODE_F4:  return KeyCode::F4;
        case SDL_SCANCODE_F5:  return KeyCode::F5;
        case SDL_SCANCODE_F6:  return KeyCode::F6;
        case SDL_SCANCODE_F7:  return KeyCode::F7;
        case SDL_SCANCODE_F8:  return KeyCode::F8;
        case SDL_SCANCODE_F9:  return KeyCode::F9;
        case SDL_SCANCODE_F10: return KeyCode::F10;
        case SDL_SCANCODE_F11: return KeyCode::F11;
        case SDL_SCANCODE_F12: return KeyCode::F12;

        case SDL_SCANCODE_ESCAPE:     return KeyCode::Escape;
        case SDL_SCANCODE_RETURN:     return KeyCode::Return;
        case SDL_SCANCODE_TAB:        return KeyCode::Tab;
        case SDL_SCANCODE_BACKSPACE:  return KeyCode::Backspace;
        case SDL_SCANCODE_SPACE:      return KeyCode::Space;
        case SDL_SCANCODE_INSERT:     return KeyCode::Insert;
        case SDL_SCANCODE_DELETE:     return KeyCode::Delete;
        case SDL_SCANCODE_HOME:       return KeyCode::Home;
        case SDL_SCANCODE_END:        return KeyCode::End;
        case SDL_SCANCODE_PAGEUP:     return KeyCode::PageUp;
        case SDL_SCANCODE_PAGEDOWN:   return KeyCode::PageDown;
        case SDL_SCANCODE_RIGHT:      return KeyCode::Right;
        case SDL_SCANCODE_LEFT:       return KeyCode::Left;
        case SDL_SCANCODE_DOWN:       return KeyCode::Down;
        case SDL_SCANCODE_UP:         return KeyCode::Up;
        case SDL_SCANCODE_CAPSLOCK:   return KeyCode::CapsLock;
        case SDL_SCANCODE_SCROLLLOCK: return KeyCode::ScrollLock;
        case SDL_SCANCODE_PAUSE:      return KeyCode::Pause;
        case SDL_SCANCODE_PRINTSCREEN:return KeyCode::PrintScreen;

        case SDL_SCANCODE_MINUS:        return KeyCode::Minus;
        case SDL_SCANCODE_EQUALS:       return KeyCode::Equals;
        case SDL_SCANCODE_LEFTBRACKET:  return KeyCode::LeftBracket;
        case SDL_SCANCODE_RIGHTBRACKET: return KeyCode::RightBracket;
        case SDL_SCANCODE_BACKSLASH:    return KeyCode::Backslash;
        case SDL_SCANCODE_SEMICOLON:    return KeyCode::Semicolon;
        case SDL_SCANCODE_APOSTROPHE:   return KeyCode::Apostrophe;
        case SDL_SCANCODE_GRAVE:        return KeyCode::Grave;
        case SDL_SCANCODE_COMMA:        return KeyCode::Comma;
        case SDL_SCANCODE_PERIOD:       return KeyCode::Period;
        case SDL_SCANCODE_SLASH:        return KeyCode::Slash;

        case SDL_SCANCODE_LSHIFT: return KeyCode::LeftShift;
        case SDL_SCANCODE_RSHIFT: return KeyCode::RightShift;
        case SDL_SCANCODE_LCTRL:  return KeyCode::LeftCtrl;
        case SDL_SCANCODE_RCTRL:  return KeyCode::RightCtrl;
        case SDL_SCANCODE_LALT:   return KeyCode::LeftAlt;
        case SDL_SCANCODE_RALT:   return KeyCode::RightAlt;
        case SDL_SCANCODE_LGUI:   return KeyCode::LeftSuper;
        case SDL_SCANCODE_RGUI:   return KeyCode::RightSuper;

        case SDL_SCANCODE_KP_1:       return KeyCode::Keypad1;
        case SDL_SCANCODE_KP_2:       return KeyCode::Keypad2;
        case SDL_SCANCODE_KP_3:       return KeyCode::Keypad3;
        case SDL_SCANCODE_KP_4:       return KeyCode::Keypad4;
        case SDL_SCANCODE_KP_5:       return KeyCode::Keypad5;
        case SDL_SCANCODE_KP_6:       return KeyCode::Keypad6;
        case SDL_SCANCODE_KP_7:       return KeyCode::Keypad7;
        case SDL_SCANCODE_KP_8:       return KeyCode::Keypad8;
        case SDL_SCANCODE_KP_9:       return KeyCode::Keypad9;
        case SDL_SCANCODE_KP_0:       return KeyCode::Keypad0;
        case SDL_SCANCODE_KP_DIVIDE:  return KeyCode::KeypadDivide;
        case SDL_SCANCODE_KP_MULTIPLY:return KeyCode::KeypadMultiply;
        case SDL_SCANCODE_KP_MINUS:   return KeyCode::KeypadMinus;
        case SDL_SCANCODE_KP_PLUS:    return KeyCode::KeypadPlus;
        case SDL_SCANCODE_KP_ENTER:   return KeyCode::KeypadEnter;
        case SDL_SCANCODE_KP_PERIOD:  return KeyCode::KeypadPeriod;

        default: return KeyCode::None;
        }
    }

    constexpr MouseButton SDLButtonToMouseButton(Uint8 sdlButton)
    {
        if (sdlButton >= SDL_BUTTON_LEFT && sdlButton <= SDL_BUTTON_X2)
            return static_cast<MouseButton>(sdlButton);
        return MouseButton::None;
    }
}