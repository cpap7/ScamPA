#pragma once
#include "Event.h"

namespace SPA {
	class CWindowResizedEvent : public IEvent {
	private:
		uint32_t m_width;
		uint32_t m_height;

	public:
		inline CWindowResizedEvent(uint32_t a_width, uint32_t a_height)
			: m_width(a_width), m_height(a_height) {

		}

		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }

		inline std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class CWindowClosedEvent : public IEvent {
	public:
		inline CWindowClosedEvent() {
		
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class CWindowFocusEvent : public IEvent {
	public:
		inline CWindowFocusEvent() {
		
		}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class CWindowLostFocusEvent : public IEvent {
	public:
		inline CWindowLostFocusEvent() {

		}

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	// File drag & drop events 
	class CWindowDropEvent : public IEvent {
	private:
		std::vector<std::filesystem::path> m_paths;

	public:
		inline CWindowDropEvent(const std::vector<std::filesystem::path>& a_paths)
			: m_paths(a_paths) {

		}

		inline CWindowDropEvent(std::vector<std::filesystem::path>&& a_paths)
			: m_paths(std::move(a_paths)) {

		} // Move constructor

		const std::vector<std::filesystem::path>& GetPath() const { return m_paths; }

		EVENT_CLASS_TYPE(WindowDrop)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/* Placeholders - Might remove later*/
	class CAppTickEvent : public IEvent {
	public:
		inline CAppTickEvent() {

		}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class CAppUpdateEvent : public IEvent {
	public:
		inline CAppUpdateEvent() {

		}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class CAppRenderEvent : public IEvent {
	public:
		inline CAppRenderEvent() {

		}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}