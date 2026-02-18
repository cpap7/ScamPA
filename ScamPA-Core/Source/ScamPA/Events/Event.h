#pragma once

#include "spapch.h"
#include "ScamPA/Core/Base.h"

namespace SPA {

	/* Events in ScamPA are currently blocking, meaning when an event occurs it
	 * immediately gets dispatched and must be dealt with right then and there.
	 * For the future, a better strategy might be to buffer events in an event
	 * bus and process them during the "event" part of the update stage.
	 */
	enum class EEventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowDrop,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EEventCategory {
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EEventType GetStaticType() { return EEventType::type; }		\
							   EEventType GetEventType() const override { return GetStaticType(); } \
							   const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) int GetCategoryFlags() const override { return category; }

	class IEvent {
	public:
		bool m_is_handled = false;

	private:
		friend class CEventDispatcher;

	public:
		~IEvent() = default;
		virtual EEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EEventCategory category) {
			return GetCategoryFlags() & category; // AND operation to widen the net (i.e., >1 category) 
		}
	};

	class CEventDispatcher {
	private:
		IEvent& m_event;

	private:
		template<typename T>
		using EventFunction = std::function<bool(T&)>;

	public:
		CEventDispatcher(IEvent& a_event)
			: m_event(a_event) {
		}

		template<typename T>
		bool Dispatch(EventFunction<T> a_function) {
			if (m_event.GetEventType() == T::GetStaticType()) {
				m_event.m_is_handled |= a_function(*(T*)&m_event);
				return true;
			}
			return false;
		}

	};

	// Overloaded operator << 
	// Makes it easier to log events by calling ToString
	inline std::ostream& operator<<(std::ostream& a_os, const IEvent& a_event) {
		return a_os << a_event.ToString();
	}

	// Specialize fmt::formatter for Event types outside the SAGE namespace
	template<typename T>
	struct fmt::formatter<T, std::enable_if_t<std::is_base_of<IEvent, T>::value, char>> : fmt::formatter<std::string> {
		auto format(const T& a_event, fmt::format_context& a_ctx) const {
			return fmt::format_to(a_ctx.out(), "{}", a_event.ToString());
		}
	};

	// Utility function for formatting strings with arguments
	template <typename... T>
	std::string StringFromArgs(fmt::format_string<T...> a_fmt, T&&... a_args) {
		return fmt::format(a_fmt, std::forward<T>(a_args)...);
	}
}