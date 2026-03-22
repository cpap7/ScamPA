#pragma once
#include <ScamPA/Audio/AudioDevice.h>

#include <vector>
#include <cstdint>

namespace SPA {
	struct SAudioDeviceSettings {
		std::vector<SAudioDeviceInfo> m_device_list{};
		int32_t m_selected_device_index = -1;
		bool m_device_list_dirty		= true; // Refresh on 1st frame
	};
}