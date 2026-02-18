#pragma once
#include "spapch.h"
#include "ScamPA/Events/Event.h"

namespace SPA {
	class ILayer {
	public:
		virtual ~ILayer() = default;

		// Defined clientside, as well as in derived classes (i.e., CImGuiLayer)
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float a_timestep) {}
		virtual void OnUIRender() {}
		virtual void OnEvent(IEvent& a_event) {}

	};

}