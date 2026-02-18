#include "spapch.h"
#include "LayerStack.h"

namespace SPA {
	CLayerStack::CLayerStack() {

	}
	
	CLayerStack::~CLayerStack() {
		for (auto& layer : m_layers) {
			layer->OnDetach();
		}
		m_layers.clear();
	}

	

	void CLayerStack::PushLayer(const std::shared_ptr<ILayer>& a_layer) {
		m_layers.emplace_back(a_layer);
		a_layer->OnAttach();
	}
	 
	void CLayerStack::PopLayer(const std::shared_ptr<ILayer>& a_target_layer) {
		auto target = std::find(m_layers.begin(), m_layers.end(), a_target_layer);

		if (target != m_layers.end()) { // If found
			a_target_layer->OnDetach();
			m_layers.erase(target);

			// Decrement if removed before the insert index
			if (target < m_layers.begin() + m_layer_insert_index) {
				m_layer_insert_index--;
			}
		}
	}
}