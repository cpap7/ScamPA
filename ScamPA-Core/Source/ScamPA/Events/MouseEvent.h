#pragma once

#include "Event.h"
#include "ScamPA/Input/MouseCodes.h"

namespace SPA {
	class CMouseMovedEvent : public IEvent {
	private:
		float m_mouse_x, m_mouse_y;

	public:
		inline CMouseMovedEvent(float a_x, float a_y)
			: m_mouse_x(a_x), m_mouse_y(a_y) {

		}

		inline float GetX() const { return m_mouse_x; }
		inline float GetY() const { return m_mouse_y; }

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << " [" << m_mouse_x << ", " << m_mouse_y << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	};

	class CMouseScrolledEvent : public IEvent {
	private:
		float m_x_offset, m_y_offset;

	public:
		inline CMouseScrolledEvent(float a_x_offset, float a_y_offset)
			: m_x_offset(a_x_offset), m_y_offset(a_y_offset) {

		}

		inline float GetXOffset() const { return m_x_offset; }
		inline float GetYOffset() const { return m_y_offset; }

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << " [" << GetXOffset() << ", " << GetYOffset() << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	};

	class CMouseButtonEvent : public IEvent {
	protected:
		EMouseButton_t m_button;

	public:
		inline EMouseButton_t GetMouseButton() const { return m_button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		inline CMouseButtonEvent(const EMouseButton_t a_button)
			: m_button(a_button) {

		}
	};

	class CMouseButtonPressedEvent : public CMouseButtonEvent {
	public:
		inline CMouseButtonPressedEvent(const EMouseButton_t a_button)
			: CMouseButtonEvent(a_button) {

		}

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class CMouseButtonReleasedEvent : public CMouseButtonEvent {
	public:
		inline CMouseButtonReleasedEvent(const EMouseButton_t a_button)
			: CMouseButtonEvent(a_button) {

		}

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}
