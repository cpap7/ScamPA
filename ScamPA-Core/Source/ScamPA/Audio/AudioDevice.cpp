#include "spapch.h"
#include "AudioDevice.h"

#include "AudioInputDevice.h"
#include "AudioOutputDevice.h"

namespace SPA {
	std::unique_ptr<IAudioDevice> IAudioDevice::Create(const SAudioDeviceConfig a_config) {
		switch (a_config.m_device_type) {
			case EAudioDeviceType::Capture:		
			case EAudioDeviceType::Loopback:	return std::make_unique<CAudioInputDevice>(a_config);
			case EAudioDeviceType::Playback:	return std::make_unique<CAudioOutputDevice>(a_config);
		}
		
		SPA_CORE_ERROR("(Audio Device) Unknown device type!");
		return nullptr;
	}
}