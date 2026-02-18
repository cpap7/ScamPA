#pragma once

#include "ScamPA/Core/Layer.h"

namespace SPA {
	class CImGuiLayer : public ILayer {
	private:
		bool m_block_events = true;

	public:
		CImGuiLayer();
		~CImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float a_timestep) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(IEvent& a_event) override;

		void Begin();
		void End();

		void SetDarkThemeColors();
		
		uint32_t GetActiveWidgetID() const;

		inline bool IsBlockingEvents()							{ return m_block_events;				}
		inline void SetBlockEvents(bool a_block_events_flag)	{ m_block_events = a_block_events_flag; }

	};
}

