#pragma once
#include "Layer.h"

namespace SPA {
	class CLayerStack {
	private:
		std::vector<std::shared_ptr<ILayer>> m_layers;
		uint32_t m_layer_insert_index = 0;

	public:
		CLayerStack();
		~CLayerStack();

		template<typename T>
		void PushLayer() {
			static_assert(std::is_base_of<ILayer, T>::value, "Pushed type is not subclass of Layer!");
			m_layers.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushLayer(const std::shared_ptr<ILayer>& a_layer);
		
		void PopLayer(const std::shared_ptr<ILayer>& a_target_layer);

		// Regular & reverse iterated vectors
		std::vector<std::shared_ptr<ILayer>>::iterator begin()						{ return m_layers.begin(); }
		std::vector<std::shared_ptr<ILayer>>::iterator end()						{ return m_layers.end(); }
		std::vector<std::shared_ptr<ILayer>>::reverse_iterator rbegin()				{ return m_layers.rbegin(); }
		std::vector<std::shared_ptr<ILayer>>::reverse_iterator rend()				{ return m_layers.rend(); }

		// Const iterated vectors
		std::vector<std::shared_ptr<ILayer>>::const_iterator begin() const			{ return m_layers.begin(); }
		std::vector<std::shared_ptr<ILayer>>::const_iterator end() const			{ return m_layers.end(); }
		std::vector<std::shared_ptr<ILayer>>::const_reverse_iterator rbegin() const { return m_layers.rbegin(); }
		std::vector<std::shared_ptr<ILayer>>::const_reverse_iterator rend() const	{ return m_layers.rend(); }
	};
}

