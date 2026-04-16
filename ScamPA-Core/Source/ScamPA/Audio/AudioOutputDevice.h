#pragma once
#include "AudioDevice.h"
#include "AudioIOResources.h"

#include <vector>
#include <mutex>

namespace SPA {
	class CAudioOutputDevice : public IAudioDevice {
	private:
		SAudioOutput m_audio_output;

	public:
		CAudioOutputDevice(const SAudioDeviceConfig& a_config);
		~CAudioOutputDevice();

		CAudioOutputDevice(const CAudioOutputDevice&) = delete;
		void operator=(const CAudioOutputDevice&) = delete;

		// Lifecycle
		virtual void Init(const SAudioDeviceConfig& a_config) override;
		virtual void Shutdown() override;
		virtual void Start() override;
		virtual void Stop() override;

		virtual std::vector<SAudioDeviceInfo> GetDeviceList() override;
		virtual void SetDeviceByIndex(int32_t a_index) override;

		// Submits pulse code modulation (PCM) samples for playback (appends to buffer)
		void SubmitSamples(const int16_t* a_samples, uint32_t a_count);
		void SubmitSamples(const std::vector<int16_t>& a_samples);

		// Clears pending audio
		void ClearBuffer();

		// Returns approx. remaining samples in buffer
		size_t GetBufferedSampleCount() const;

		// Internal helper which is called from miniaudio data callback function
		uint32_t OnDataRequested(int16_t* a_output, uint32_t a_sample_count, uint32_t a_threshold = 48000);

	private:
		// Compacts audio once it's fully consumed
		// or past a certain threshold (i.e., ~3s @ 16kHz)
		// Default threshold = 48000; since 3s * 16000 = 48000
		void CompactAudioSamples(uint32_t a_threshold = 48000); 		// Called internally via OnDataRequested()

	};
}

