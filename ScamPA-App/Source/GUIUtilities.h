#pragma once
#include <cstdint>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>

namespace SPA {
	
	namespace GUI {

		static bool BeginTreeNode(const char* a_name, bool a_default_open_flag = true) {
			ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			if (a_default_open_flag) {
				tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			return ImGui::TreeNodeEx(a_name, tree_node_flags);
		}

		static void EndTreeNode() {
			ImGui::TreePop();
		}
	}
}