#pragma once

#include "ScamPA/Core/Layer.h"

namespace SPA {

	// Handles majority of the ImGui setup for Vulkan
	class CImGuiLayer : public ILayer {
	private:
		bool m_is_blocking_events = true;

		// UI callbacks
		std::function<void()> m_menubar_callback;

	public:
		CImGuiLayer();
		~CImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float a_timestep) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(IEvent& a_event) override;

		void BeginFrame();
		void EndFrame();
		void RenderDockspace();

		// Getters
		uint32_t GetActiveWidgetID() const;
		inline bool IsBlockingEvents() const							{ return m_is_blocking_events;	}
		inline const std::function<void()>& GetMenubarCallback() const	{ return m_menubar_callback;	}

		// Setters
		void SetDarkThemeColors();
		inline void SetBlockEvents(bool a_block_events_flag)							{ m_is_blocking_events = a_block_events_flag;	}
		inline void SetMenubarCallback(const std::function<void()>& a_menubar_callback)	{ m_menubar_callback = a_menubar_callback;		}
	};
}

