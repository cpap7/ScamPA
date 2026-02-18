#pragma once
#include "spapch.h"
#include "ScamPA/Events/Event.h"

namespace SPA {
	class ILayer {
	public:
		virtual ~ILayer() = default;

		// Methods are defined in derived classes (i.e., CImGuiLayer) or layer classes within clientside app
		virtual void OnAttach() {}									// Function for what happens when pushed onto a layer stack
		virtual void OnDetach() {}									// Function for what happens when popped off of a layer stack
		virtual void OnUpdate(float a_timestep) {}					// Function for what occurs w/ the layer every frame
		virtual void OnUIRender() {}								// Function for whats occurs when rendering ImGui windows
		virtual void OnEvent(IEvent& a_event) {}					// Function for handling events
	};

}