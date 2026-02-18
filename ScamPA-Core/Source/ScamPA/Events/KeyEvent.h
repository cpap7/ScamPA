#pragma once
#include "Event.h"
#include "ScamPA/Input/KeyCodes.h"

namespace SPA {
	class CKeyEvent : public IEvent {
	protected:
		EKeyCode_t m_keycode;

	protected:
		CKeyEvent(const EKeyCode_t a_keycode)
			: m_keycode(a_keycode) {

		}

	public:
		inline EKeyCode_t GetKeyCode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};

	class CKeyPressedEvent : public CKeyEvent {
	private:
		bool m_is_repeated;

	public:
		inline CKeyPressedEvent(const EKeyCode_t a_keycode, bool a_is_repeated = false)
			: CKeyEvent(a_keycode), m_is_repeated(a_is_repeated) {

		}

		inline bool IsRepeated() const { return m_is_repeated; }

		/* Repeat count tracks how many times a character is produced when the key is held down
		* i.e., holding down b = bbbbbbbbbbbbbbbbbbbb
		*/
		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keycode << " (Repeated = " << m_is_repeated << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class CKeyReleasedEvent : public CKeyEvent {
	public:
		inline CKeyReleasedEvent(const EKeyCode_t a_keycode)
			: CKeyEvent(a_keycode) {

		}

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class CKeyTypedEvent : public CKeyEvent {
	public:
		inline CKeyTypedEvent(const EKeyCode_t a_keycode)
			: CKeyEvent(a_keycode) {

		}

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped);
	};
}