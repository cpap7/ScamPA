#pragma once

#include <stdint.h>
#include <iostream>

namespace SPA {
	typedef enum class EKeyCode_t : uint16_t {
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	enum class EKeyState {
		None = -1,
		Pressed,
		Held,
		Released
	};

	inline std::ostream& operator<<(std::ostream& a_os, EKeyCode_t a_key_code) {
		a_os << static_cast<int32_t>(a_key_code);
		return a_os;
	}	
}

#define SPA_KEY_SPACE           ::SPA::EKeyCode_t::Space
#define SPA_KEY_APOSTROPHE      ::SPA::EKeyCode_t::Apostrophe    /* ' */
#define SPA_KEY_COMMA           ::SPA::EKeyCode_t::Comma         /* , */
#define SPA_KEY_MINUS           ::SPA::EKeyCode_t::Minus         /* - */
#define SPA_KEY_PERIOD          ::SPA::EKeyCode_t::Period        /* . */
#define SPA_KEY_SLASH           ::SPA::EKeyCode_t::Slash         /* / */
#define SPA_KEY_0               ::SPA::EKeyCode_t::D0
#define SPA_KEY_1               ::SPA::EKeyCode_t::D1
#define SPA_KEY_2               ::SPA::EKeyCode_t::D2
#define SPA_KEY_3               ::SPA::EKeyCode_t::D3
#define SPA_KEY_4               ::SPA::EKeyCode_t::D4
#define SPA_KEY_5               ::SPA::EKeyCode_t::D5
#define SPA_KEY_6               ::SPA::EKeyCode_t::D6
#define SPA_KEY_7               ::SPA::EKeyCode_t::D7
#define SPA_KEY_8               ::SPA::EKeyCode_t::D8
#define SPA_KEY_9               ::SPA::EKeyCode_t::D9
#define SPA_KEY_SEMICOLON       ::SPA::EKeyCode_t::Semicolon     /* ; */
#define SPA_KEY_EQUAL           ::SPA::EKeyCode_t::Equal         /* = */
#define SPA_KEY_A               ::SPA::EKeyCode_t::A
#define SPA_KEY_B               ::SPA::EKeyCode_t::B
#define SPA_KEY_C               ::SPA::EKeyCode_t::C
#define SPA_KEY_D               ::SPA::EKeyCode_t::D
#define SPA_KEY_E               ::SPA::EKeyCode_t::E
#define SPA_KEY_F               ::SPA::EKeyCode_t::F
#define SPA_KEY_G               ::SPA::EKeyCode_t::G
#define SPA_KEY_H               ::SPA::EKeyCode_t::H
#define SPA_KEY_I               ::SPA::EKeyCode_t::I
#define SPA_KEY_J               ::SPA::EKeyCode_t::J
#define SPA_KEY_K               ::SPA::EKeyCode_t::K
#define SPA_KEY_L               ::SPA::EKeyCode_t::L
#define SPA_KEY_M               ::SPA::EKeyCode_t::M
#define SPA_KEY_N               ::SPA::EKeyCode_t::N
#define SPA_KEY_O               ::SPA::EKeyCode_t::O
#define SPA_KEY_P               ::SPA::EKeyCode_t::P
#define SPA_KEY_Q               ::SPA::EKeyCode_t::Q
#define SPA_KEY_R               ::SPA::EKeyCode_t::R
#define SPA_KEY_S               ::SPA::EKeyCode_t::S
#define SPA_KEY_T               ::SPA::EKeyCode_t::T
#define SPA_KEY_U               ::SPA::EKeyCode_t::U
#define SPA_KEY_V               ::SPA::EKeyCode_t::V
#define SPA_KEY_W               ::SPA::EKeyCode_t::W
#define SPA_KEY_X               ::SPA::EKeyCode_t::X
#define SPA_KEY_Y               ::SPA::EKeyCode_t::Y
#define SPA_KEY_Z               ::SPA::EKeyCode_t::Z
#define SPA_KEY_LEFT_BRACKET    ::SPA::EKeyCode_t::LeftBracket   /* [ */
#define SPA_KEY_BACKSLASH       ::SPA::EKeyCode_t::Backslash     /* \ */
#define SPA_KEY_RIGHT_BRACKET   ::SPA::EKeyCode_t::RightBracket  /* ] */
#define SPA_KEY_GRAVE_ACCENT    ::SPA::EKeyCode_t::GraveAccent   /* ` */
#define SPA_KEY_WORLD_1         ::SPA::EKeyCode_t::World1        /* non-US #1 */
#define SPA_KEY_WORLD_2         ::SPA::EKeyCode_t::World2        /* non-US #2 */

/* Function keys */
#define SPA_KEY_ESCAPE          ::SPA::EKeyCode_t::Escape
#define SPA_KEY_ENTER           ::SPA::EKeyCode_t::Enter
#define SPA_KEY_TAB             ::SPA::EKeyCode_t::Tab
#define SPA_KEY_BACKSPACE       ::SPA::EKeyCode_t::Backspace
#define SPA_KEY_INSERT          ::SPA::EKeyCode_t::Insert
#define SPA_KEY_DELETE          ::SPA::EKeyCode_t::Delete
#define SPA_KEY_RIGHT           ::SPA::EKeyCode_t::Right
#define SPA_KEY_LEFT            ::SPA::EKeyCode_t::Left
#define SPA_KEY_DOWN            ::SPA::EKeyCode_t::Down
#define SPA_KEY_UP              ::SPA::EKeyCode_t::Up
#define SPA_KEY_PAGE_UP         ::SPA::EKeyCode_t::PageUp
#define SPA_KEY_PAGE_DOWN       ::SPA::EKeyCode_t::PageDown
#define SPA_KEY_HOME            ::SPA::EKeyCode_t::Home
#define SPA_KEY_END             ::SPA::EKeyCode_t::End
#define SPA_KEY_CAPS_LOCK       ::SPA::EKeyCode_t::CapsLock
#define SPA_KEY_SCROLL_LOCK     ::SPA::EKeyCode_t::ScrollLock
#define SPA_KEY_NUM_LOCK        ::SPA::EKeyCode_t::NumLock
#define SPA_KEY_PRINT_SCREEN    ::SPA::EKeyCode_t::PrintScreen
#define SPA_KEY_PAUSE           ::SPA::EKeyCode_t::Pause
#define SPA_KEY_F1              ::SPA::EKeyCode_t::F1
#define SPA_KEY_F2              ::SPA::EKeyCode_t::F2
#define SPA_KEY_F3              ::SPA::EKeyCode_t::F3
#define SPA_KEY_F4              ::SPA::EKeyCode_t::F4
#define SPA_KEY_F5              ::SPA::EKeyCode_t::F5
#define SPA_KEY_F6              ::SPA::EKeyCode_t::F6
#define SPA_KEY_F7              ::SPA::EKeyCode_t::F7
#define SPA_KEY_F8              ::SPA::EKeyCode_t::F8
#define SPA_KEY_F9              ::SPA::EKeyCode_t::F9
#define SPA_KEY_F10             ::SPA::EKeyCode_t::F10
#define SPA_KEY_F11             ::SPA::EKeyCode_t::F11
#define SPA_KEY_F12             ::SPA::EKeyCode_t::F12
#define SPA_KEY_F13             ::SPA::EKeyCode_t::F13
#define SPA_KEY_F14             ::SPA::EKeyCode_t::F14
#define SPA_KEY_F15             ::SPA::EKeyCode_t::F15
#define SPA_KEY_F16             ::SPA::EKeyCode_t::F16
#define SPA_KEY_F17             ::SPA::EKeyCode_t::F17
#define SPA_KEY_F18             ::SPA::EKeyCode_t::F18
#define SPA_KEY_F19             ::SPA::EKeyCode_t::F19
#define SPA_KEY_F20             ::SPA::EKeyCode_t::F20
#define SPA_KEY_F21             ::SPA::EKeyCode_t::F21
#define SPA_KEY_F22             ::SPA::EKeyCode_t::F22
#define SPA_KEY_F23             ::SPA::EKeyCode_t::F23
#define SPA_KEY_F24             ::SPA::EKeyCode_t::F24
#define SPA_KEY_F25             ::SPA::EKeyCode_t::F25

/* Keypad */
#define SPA_KEY_KP_0            ::SPA::EKeyCode_t::KP0
#define SPA_KEY_KP_1            ::SPA::EKeyCode_t::KP1
#define SPA_KEY_KP_2            ::SPA::EKeyCode_t::KP2
#define SPA_KEY_KP_3            ::SPA::EKeyCode_t::KP3
#define SPA_KEY_KP_4            ::SPA::EKeyCode_t::KP4
#define SPA_KEY_KP_5            ::SPA::EKeyCode_t::KP5
#define SPA_KEY_KP_6            ::SPA::EKeyCode_t::KP6
#define SPA_KEY_KP_7            ::SPA::EKeyCode_t::KP7
#define SPA_KEY_KP_8            ::SPA::EKeyCode_t::KP8
#define SPA_KEY_KP_9            ::SPA::EKeyCode_t::KP9
#define SPA_KEY_KP_DECIMAL      ::SPA::EKeyCode_t::KPDecimal
#define SPA_KEY_KP_DIVIDE       ::SPA::EKeyCode_t::KPDivide
#define SPA_KEY_KP_MULTIPLY     ::SPA::EKeyCode_t::KPMultiply
#define SPA_KEY_KP_SUBTRACT     ::SPA::EKeyCode_t::KPSubtract
#define SPA_KEY_KP_ADD          ::SPA::EKeyCode_t::KPAdd
#define SPA_KEY_KP_ENTER        ::SPA::EKeyCode_t::KPEnter
#define SPA_KEY_KP_EQUAL        ::SPA::EKeyCode_t::KPEqual

#define SPA_KEY_LEFT_SHIFT      ::SPA::EKeyCode_t::LeftShift
#define SPA_KEY_LEFT_CONTROL    ::SPA::EKeyCode_t::LeftControl
#define SPA_KEY_LEFT_ALT        ::SPA::EKeyCode_t::LeftAlt
#define SPA_KEY_LEFT_SUPER      ::SPA::EKeyCode_t::LeftSuper
#define SPA_KEY_RIGHT_SHIFT     ::SPA::EKeyCode_t::RightShift
#define SPA_KEY_RIGHT_CONTROL   ::SPA::EKeyCode_t::RightControl
#define SPA_KEY_RIGHT_ALT       ::SPA::EKeyCode_t::RightAlt
#define SPA_KEY_RIGHT_SUPER     ::SPA::EKeyCode_t::RightSuper
#define SPA_KEY_MENU            ::SPA::EKeyCode_t::Menu

namespace SPA {
	// Platform conversion dictionaries go here
	inline std::unordered_map<EKeyCode_t, int> windows_keycode_map = {
		{ SPA_KEY_SPACE,			0x20 },
		{ SPA_KEY_APOSTROPHE,		0xDE },
		{ SPA_KEY_COMMA,			0xBC },
		{ SPA_KEY_MINUS,			0xBD },
		{ SPA_KEY_PERIOD,			0xBE },
		{ SPA_KEY_SLASH,			0xBF },
		{ SPA_KEY_0,				0x30 },
		{ SPA_KEY_1,				0x31 },
		{ SPA_KEY_2,				0x32 },
		{ SPA_KEY_3,				0x33 },
		{ SPA_KEY_4,				0x34 },
		{ SPA_KEY_5,				0x35 },
		{ SPA_KEY_6,				0x36 },
		{ SPA_KEY_7,				0x37 },
		{ SPA_KEY_8,				0x38 },
		{ SPA_KEY_9,				0x39 },
		{ SPA_KEY_SEMICOLON,		0xBA },
		{ SPA_KEY_EQUAL,			0xBB },
		{ SPA_KEY_A,				0x41 },
		{ SPA_KEY_B,				0x42 },
		{ SPA_KEY_C,				0x43 },
		{ SPA_KEY_D,				0x44 },
		{ SPA_KEY_E,				0x45 },
		{ SPA_KEY_F,				0x46 },
		{ SPA_KEY_G,				0x47 },
		{ SPA_KEY_H,				0x48 },
		{ SPA_KEY_I,				0x49 },
		{ SPA_KEY_J,				0x4A },
		{ SPA_KEY_K,				0x4B },
		{ SPA_KEY_L,				0x4C },
		{ SPA_KEY_M,				0x4D },
		{ SPA_KEY_N,				0x4E },
		{ SPA_KEY_O,				0x4F },
		{ SPA_KEY_P,				0x50 },
		{ SPA_KEY_Q,				0x51 },
		{ SPA_KEY_R,				0x52 },
		{ SPA_KEY_S,				0x53 },
		{ SPA_KEY_T,				0x54 },
		{ SPA_KEY_U,				0x55 },
		{ SPA_KEY_V,				0x56 },
		{ SPA_KEY_W,				0x57 },
		{ SPA_KEY_X,				0x58 },
		{ SPA_KEY_Y,				0x59 },
		{ SPA_KEY_Z,				0x5A },
		{ SPA_KEY_LEFT_BRACKET,		0xDB },
		{ SPA_KEY_BACKSLASH,		0xDC },
		{ SPA_KEY_RIGHT_BRACKET,	0xDD },
		{ SPA_KEY_GRAVE_ACCENT,		0xC0 },
		// { SPA_KEY_WORLD_1, },
		// { SPA_KEY_WORLD_2, },
		{ SPA_KEY_ESCAPE,			0x1B },
		{ SPA_KEY_ENTER,			0x0D },
		{ SPA_KEY_TAB,				0x09 },
		{ SPA_KEY_BACKSPACE,		0x08 },
		{ SPA_KEY_INSERT,			0x2D },
		{ SPA_KEY_DELETE,			0x2E },
		{ SPA_KEY_RIGHT,			0x27 },
		{ SPA_KEY_LEFT,				0x25 },
		{ SPA_KEY_DOWN,				0x28 },
		{ SPA_KEY_UP,				0x26 },
		{ SPA_KEY_PAGE_UP,			0x21 },
		{ SPA_KEY_PAGE_DOWN,		0x22 },
		{ SPA_KEY_HOME,				0x24 },
		{ SPA_KEY_END,				0x23 },
		{ SPA_KEY_CAPS_LOCK,		0x14 },
		{ SPA_KEY_SCROLL_LOCK,		0x91 },
		{ SPA_KEY_NUM_LOCK,			0x90 },
		{ SPA_KEY_PRINT_SCREEN,		0x2C },
		{ SPA_KEY_PAUSE,			0x13 },
		{ SPA_KEY_F1,				0x70 },
		{ SPA_KEY_F2,				0x71 },
		{ SPA_KEY_F3,				0x72 },
		{ SPA_KEY_F4,				0x73 },
		{ SPA_KEY_F5,				0x74 },
		{ SPA_KEY_F6,				0x75 },
		{ SPA_KEY_F7,				0x76 },
		{ SPA_KEY_F8,				0x77 },
		{ SPA_KEY_F9,				0x78 },
		{ SPA_KEY_F10,				0x79 },
		{ SPA_KEY_F11,				0x7A },
		{ SPA_KEY_F12,				0x7B },
		{ SPA_KEY_F13,				0x7C },
		{ SPA_KEY_F14,				0x7D },
		{ SPA_KEY_F15,				0x7E },
		{ SPA_KEY_F16,				0x7F },
		{ SPA_KEY_F17,				0x80 },
		{ SPA_KEY_F18,				0x81 },
		{ SPA_KEY_F19,				0x82 },
		{ SPA_KEY_F20,				0x83 },
		{ SPA_KEY_F21,				0x84 },
		{ SPA_KEY_F22,				0x85 },
		{ SPA_KEY_F23,				0x86 },
		{ SPA_KEY_F24,				0x87 },
		//{ SPA_KEY_F25,				0x88 }, // TODO: Double check
		{ SPA_KEY_KP_0,				0x60 },
		{ SPA_KEY_KP_1,				0x61 },
		{ SPA_KEY_KP_2,				0x62 },
		{ SPA_KEY_KP_3,				0x63 },
		{ SPA_KEY_KP_4,				0x64 },
		{ SPA_KEY_KP_5, 			0x65 },
		{ SPA_KEY_KP_6, 			0x66 },
		{ SPA_KEY_KP_7, 			0x67 },
		{ SPA_KEY_KP_8, 			0x68 },
		{ SPA_KEY_KP_9, 			0x69 },
		{ SPA_KEY_KP_DECIMAL,		0x6E },
		{ SPA_KEY_KP_DIVIDE,		0x6F },
		{ SPA_KEY_KP_MULTIPLY,		0x6A },
		{ SPA_KEY_KP_SUBTRACT,		0x6D },
		{ SPA_KEY_KP_ADD,			0x6B },
		{ SPA_KEY_KP_ENTER,			0x0D },
		{ SPA_KEY_KP_EQUAL,			0xBB },
		{ SPA_KEY_LEFT_SHIFT,		0xA0 },
		{ SPA_KEY_LEFT_CONTROL,		0xA2 },
		{ SPA_KEY_LEFT_ALT,			0xA4 },
		{ SPA_KEY_LEFT_SUPER,		0x5B },
		{ SPA_KEY_RIGHT_SHIFT,		0xA1 },
		{ SPA_KEY_RIGHT_CONTROL,	0xA3 },
		{ SPA_KEY_RIGHT_ALT,		0xA5 },
		{ SPA_KEY_RIGHT_SUPER,		0x5C },
		{ SPA_KEY_MENU,				0x5E }
	};
}