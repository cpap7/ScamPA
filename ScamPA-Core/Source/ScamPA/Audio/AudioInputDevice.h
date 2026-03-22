#pragma once
#include "AudioDevice.h"

#include <vector>
#include <mutex>
#include <functional>

namespace SPA {

	using AudioCaptureCallbackFn = std::function<void(const int16_t*, uint32_t)>; // args = a_samples, a_sample_count

	class CAudioInputDevice : public IAudioDevice {
	private:
		SAudioResource m_audio_resource;
		AudioCaptureCallbackFn m_callback;		

	public:
		CAudioInputDevice(const SAudioDeviceConfig& a_config);
		~CAudioInputDevice();

		// No need for copies
		CAudioInputDevice(const CAudioInputDevice&) = delete;
		void operator=(const CAudioInputDevice&) = delete;

		// Lifecycle
		virtual void Init(const SAudioDeviceConfig& a_config) override;
		virtual void Shutdown() override;
		virtual void Start() override;
		virtual void Stop() override;

		virtual std::vector<SAudioDeviceInfo> GetDeviceList() override;
		virtual void SetDeviceByIndex(int32_t a_index) override;

		void SetCaptureCallback(const AudioCaptureCallbackFn& a_callback);

		// Drains audio buffer & returns all buffered samples
		// NOTE: Should be thread-safe
		std::vector<int16_t> ConsumeBuffer();
		
		// Internal helper called from miniaudio data callback
		void OnDataReceived(const int16_t* a_samples, uint32_t a_sample_count);
	};
}

