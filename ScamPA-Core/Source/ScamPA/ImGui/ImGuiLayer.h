#pragma once

#include "ScamPA/Core/Layer.h"
#include "ScamPA/Renderer/VulkanImage.h"

namespace SPA {

	// Handles majority of the ImGui setup for Vulkan
	class CImGuiLayer : public ILayer {
	private:
		// UI callbacks
		std::function<void()> m_menubar_callback		= nullptr;
		
		// Icon resources
		std::shared_ptr<CVulkanImage> m_app_header_icon = nullptr;
		std::shared_ptr<CVulkanImage> m_icon_close		= nullptr;
		std::shared_ptr<CVulkanImage> m_icon_minimize	= nullptr;
		std::shared_ptr<CVulkanImage> m_icon_maximize	= nullptr;
		std::shared_ptr<CVulkanImage> m_icon_restore	= nullptr;

		// Flags
		bool m_is_blocking_events	= true;
		bool m_is_titlebar_hovered	= false;
		bool m_use_custom_titlebar	= false;

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

	private: // Helpers
		void RenderCustomMenubar();
		void RenderCustomTitlebar(float& a_out_titlebar_height);

	public:
		// Getters
		uint32_t GetActiveWidgetID() const;
		inline bool IsBlockingEvents() const							{ return m_is_blocking_events;	}
		inline bool IsTitlebarHovered() const							{ return m_is_titlebar_hovered;	}
		inline const std::function<void()>& GetMenubarCallback() const	{ return m_menubar_callback;	}

		// Setters
		inline void SetBlockEvents(bool a_block_events_flag)							{ m_is_blocking_events = a_block_events_flag;		}
		inline void SetTitlebarHovered(bool a_titlebar_hovered_flag)					{ m_is_titlebar_hovered = a_titlebar_hovered_flag;	}
		inline void SetMenubarCallback(const std::function<void()>& a_menubar_callback)	{ m_menubar_callback = a_menubar_callback;			}
	};
}

