#include "spapch.h"
#include "AudioInputDevice.h"

#include <miniaudio.h>

namespace SPA {

	namespace Utilities {
		static void CaptureDataCallback(ma_device* a_device, void* a_output, const void* a_input, ma_uint32 a_frame_count) {
			(void)a_output; // Unused for capture
			
			auto* capture_device = static_cast<CAudioInputDevice*>(a_device->pUserData);
			if (!capture_device || !a_input) {
				return;
			}

			const auto* samples = static_cast<const int16_t*>(a_input);
			uint32_t total_samples = a_frame_count * a_device->capture.channels;
			capture_device->OnDataReceived(samples, total_samples);
		}
	}


	CAudioInputDevice::CAudioInputDevice(const SAudioDeviceConfig& a_config) {
		Init(a_config);
	}

	CAudioInputDevice::~CAudioInputDevice() {
		Shutdown();
	}

	void CAudioInputDevice::Init(const SAudioDeviceConfig& a_config) {
		if (m_device) {
			SPA_CORE_WARN("(Audio Input Device) Device already initialized!");
			return;
		}

		m_config = a_config;
		m_config.m_device_type = a_config.m_device_type;
		
		m_device = new ma_device();
		ma_device_config ma_device_cfg;
		if (a_config.m_device_type == EAudioDeviceType::Loopback) {
			ma_device_cfg = ma_device_config_init(ma_device_type_loopback);
			SPA_CORE_INFO("(Audio Input Device) Configured to loopback (speaker) capture");
		}
		else { // Microphone input
			ma_device_cfg = ma_device_config_init(ma_device_type_capture);
			SPA_CORE_INFO("(Audio Input Device) Configured to microphone capture");
		}

		ma_device_cfg.capture.format	= ma_format_s16;
		ma_device_cfg.capture.channels	= a_config.m_channels;
		ma_device_cfg.sampleRate		= a_config.m_sample_rate;
		ma_device_cfg.dataCallback		= Utilities::CaptureDataCallback;
		ma_device_cfg.pUserData			= this;

		ma_result result = ma_device_init(nullptr, &ma_device_cfg, m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Input Device) Failed to initialize device! Error Code: {0}", static_cast<int>(result));
			delete m_device;
			m_device = nullptr;
			return;
		}

		SPA_CORE_INFO("(Audio Input Device) Device initialized (sample rate = {0}, channels = {1})", m_device->sampleRate, m_device->capture.channels);
	}

	void CAudioInputDevice::Shutdown() {
		Stop();

		if (m_device) {
			ma_device_uninit(m_device);
			delete m_device;
			m_device = nullptr;
		}

		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_audio_resource.m_buffer.clear();
	}

	void CAudioInputDevice::Start() {
		if (!m_device) {
			SPA_CORE_ERROR("(Audio Input Device) Cannot start since there is no audio device!");
			return;
		}

		if (m_is_active) {
			return;
		}
		
		ma_result result = ma_device_start(m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Input Device) Failed to start audio device! Error Code: {0}", static_cast<int>(result));
			return;
		}

		m_is_active = true;
		SPA_CORE_INFO("(Audio Input Device) Audio capture started");
	}

	void CAudioInputDevice::Stop() {
		if (!m_device || !m_is_active) {
			return;
		}

		ma_result result = ma_device_stop(m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Input Device) Failed to stop audio device! Error Code: {0}", static_cast<int>(result));
			return;
		}

		m_is_active = false;
		SPA_CORE_INFO("(Audio Input Device) Audio capture stopped");
	}

	void CAudioInputDevice::SetCaptureCallback(const AudioCaptureCallbackFn& a_callback) {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_callback = a_callback;
	}

	std::vector<int16_t> CAudioInputDevice::ConsumeBuffer() {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		
		std::vector<int16_t> consumed = std::move(m_audio_resource.m_buffer);
		m_audio_resource.m_buffer.clear();
		
		return consumed;
	}

	void CAudioInputDevice::OnDataReceived(const int16_t* a_samples, uint32_t a_sample_count) {
		{ // Separate block to prevent potential deadlock
			std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
			m_audio_resource.m_buffer.insert(m_audio_resource.m_buffer.end(), a_samples, a_samples + a_sample_count);
		}

		if (m_callback) {
			m_callback(a_samples, a_sample_count);
		}
	}
}