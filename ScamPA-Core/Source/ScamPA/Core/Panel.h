#pragma once

namespace SPA {
	class IPanel {
	public:
		virtual ~IPanel() = default;

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUIRender() {}
	};
}