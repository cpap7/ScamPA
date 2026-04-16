#pragma once
#include "AudioDevice.h"
#include "AudioIOResources.h"

#include <vector>
#include <mutex>
#include <functional>

namespace SPA {

	using AudioCaptureCallbackFn = std::function<void(const int16_t*, uint32_t)>; // args = a_samples, a_sample_count

	class CAudioInputDevice : public IAudioDevice {
	private:
		SAudioInput m_audio_input;
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
		virtual std::vector<int16_t> ConsumeBuffer() override; 		// Drains audio buffer & returns all buffered samples

		void SetCaptureCallback(const AudioCaptureCallbackFn& a_callback);

		float GetRecentRMSEnergy(size_t a_sample_count = 4800) const; 		// Non-consuming peek helper - gets RMS energy of last set of samples
		size_t GetBufferedSampleCount() const; 								

		// Internal helper called from miniaudio data callback
		void OnDataReceived(const int16_t* a_samples, uint32_t a_sample_count);
	};
}

