#pragma once
#include <ScamPA/Core/Layer.h>

namespace SPA {
	class CAppLayer : public ILayer {
	public:
		CAppLayer();
		~CAppLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float a_timestep) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(IEvent& a_event) override;


	};
}