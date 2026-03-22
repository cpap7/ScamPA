#include "spapch.h"
#include "AudioOutputDevice.h"

#include <cstring>
#include <miniaudio.h>

namespace SPA {

	namespace Utilities {
		static void PlaybackDataCallback(ma_device* a_device, void* a_output, const void* a_input, ma_uint32 a_frame_count) {
			(void)a_input; // Not needed for playback

			auto* playback_device = static_cast<CAudioOutputDevice*>(a_device->pUserData);
			if (!playback_device || !a_output) {
				return;
			}

			auto* output = static_cast<int16_t*>(a_output);
			uint32_t total_samples = a_frame_count * a_device->playback.channels;
			uint32_t written_samples = playback_device->OnDataRequested(output, total_samples);

			// Zero-fill remainder (avoid noise)
			if (written_samples < total_samples) {
				std::memset(output + written_samples, 0, (total_samples - written_samples) * sizeof(int16_t));
			}
		}
	}

	CAudioOutputDevice::CAudioOutputDevice(const SAudioDeviceConfig& a_config) {
		Init(a_config);
	}

	CAudioOutputDevice::~CAudioOutputDevice() {
		Shutdown();
	}

	void CAudioOutputDevice::Init(const SAudioDeviceConfig& a_config) {
		if (m_device) {
			SPA_CORE_WARN("(Audio Output Device) Device already initialized!");
			return;
		}
		m_config = a_config;
		m_config.m_device_type = a_config.m_device_type;

		m_device = new ma_device();
		ma_device_config ma_device_cfg	= ma_device_config_init(ma_device_type_playback);
		ma_device_cfg.playback.format	= ma_format_s16;
		ma_device_cfg.playback.channels = a_config.m_channels;
		ma_device_cfg.sampleRate		= a_config.m_sample_rate;
		ma_device_cfg.dataCallback		= Utilities::PlaybackDataCallback;
		ma_device_cfg.pUserData			= this;

		ma_result result = ma_device_init(nullptr, &ma_device_cfg, m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Output Device) Failed to initialize device! Error Code: {0}", static_cast<int>(result));
			delete m_device;
			m_device = nullptr;
			return;
		}

		SPA_CORE_INFO("(Audio Output Device) Device initialized (sample rate = {0}, channels = {1})", m_device->sampleRate, m_device->playback.channels);
	}
	
	void CAudioOutputDevice::Shutdown() {
		Stop();
		
		if (m_device) {
			ma_device_uninit(m_device);
			delete m_device;
			m_device = nullptr;
		}
		
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_audio_resource.m_buffer.clear();
		m_read_cursor = 0;
	}

	void CAudioOutputDevice::Start() {
		if (!m_device) {
			SPA_CORE_ERROR("(Audio Output Device) Cannot start since there is no audio device!");
			return;
		}

		if (m_is_active) {
			return;
		}

		ma_result result = ma_device_start(m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Output Device) Failed to start audio device! Error Code: {0}", static_cast<int>(result));
			return;
		}

		m_is_active = true;
		SPA_CORE_INFO("(Audio Output Device) Audio playback started");
	}

	void CAudioOutputDevice::Stop() {
		if (!m_device || !m_is_active) {
			return;
		}

		ma_result result = ma_device_stop(m_device);
		if (result != MA_SUCCESS) {
			SPA_CORE_ERROR("(Audio Output Device) Failed to stop audio device! Error Code: {0}", static_cast<int>(result));
			return;
		}

		m_is_active = false;
		SPA_CORE_INFO("(Audio Output Device) Audio playback stopped");
	}

	void CAudioOutputDevice::SubmitSamples(const int16_t* a_samples, uint32_t a_count) {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_audio_resource.m_buffer.insert(m_audio_resource.m_buffer.end(), a_samples, a_samples + a_count);
	}

	void CAudioOutputDevice::SubmitSamples(const std::vector<int16_t>& a_samples) {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_audio_resource.m_buffer.insert(m_audio_resource.m_buffer.end(), a_samples.begin(), a_samples.end());
	}

	void CAudioOutputDevice::ClearBuffer() {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);
		m_audio_resource.m_buffer.clear();
		m_read_cursor = 0;
	}

	size_t CAudioOutputDevice::GetBufferedSampleCount() const {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);

		// Return difference of the current buffer size & read cursor
		return (m_audio_resource.m_buffer.size() > m_read_cursor) ? (m_audio_resource.m_buffer.size() - m_read_cursor) : 0;
	}

	uint32_t CAudioOutputDevice::OnDataRequested(int16_t* a_output, uint32_t a_sample_count) {
		std::lock_guard<std::mutex> lock(m_audio_resource.m_mutex);

		size_t available = (m_audio_resource.m_buffer.size() > m_read_cursor) ? (m_audio_resource.m_buffer.size() - m_read_cursor) : 0;
		uint32_t to_copy = static_cast<uint32_t>(std::min<size_t>(available, a_sample_count));
		
		if (to_copy > 0) {
			std::memcpy(a_output, m_audio_resource.m_buffer.data() + m_read_cursor, to_copy * sizeof(int16_t));
			m_read_cursor += to_copy;
		}

		// Compact once it's fully consumed
		// or past a certain threshold (i.e., ~3s @ 16kHz)
		if (m_read_cursor > 0 && m_read_cursor >= m_audio_resource.m_buffer.size()) {
			m_audio_resource.m_buffer.clear();
			m_read_cursor = 0;
		}
		else if (m_read_cursor > 48000) {
			m_audio_resource.m_buffer.erase(m_audio_resource.m_buffer.begin(), m_audio_resource.m_buffer.begin() + m_read_cursor);
			m_read_cursor = 0;
		}

		return to_copy;
	}
}