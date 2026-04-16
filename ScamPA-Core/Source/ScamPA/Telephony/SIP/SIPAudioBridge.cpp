#include "spapch.h"
#include "SIPAudioBridge.h"

namespace SPA {

	void CSIPAudioBridge::onFrameReceived(pj::MediaFrame& a_frame) {
		if (a_frame.type != PJMEDIA_FRAME_TYPE_AUDIO || a_frame.buf.size() == 0) {
			return;
		}

		// Caller's RTP audio arrives here as raw bytes (via PJ's bytevector)
		// So, they need to be reinterpret_cast as PCM int16 samples which we currently use
		const int16_t* frame_samples = reinterpret_cast<const int16_t*>(a_frame.buf.data());
		size_t frame_sample_count = a_frame.buf.size() / sizeof(int16_t);

		std::lock_guard<std::mutex> lock(m_inbound_audio.m_mutex);
		m_inbound_audio.m_buffer.insert(m_inbound_audio.m_buffer.end(), frame_samples, frame_samples + frame_sample_count);

	}

	void CSIPAudioBridge::onFrameRequested(pj::MediaFrame& a_frame) {
		std::lock_guard<std::mutex> lock(m_outbound_audio.m_mutex);

		int16_t* output_buffer = reinterpret_cast<int16_t*>(a_frame.buf.data());
		size_t frame_sample_count = a_frame.buf.size() / sizeof(int16_t);

		size_t available = (m_outbound_audio.m_buffer.size() > m_outbound_audio.m_read_cursor) ? (m_outbound_audio.m_buffer.size() - m_outbound_audio.m_read_cursor) : 0;
		size_t to_copy = std::min(available, frame_sample_count);

		if (to_copy > 0) {
			// Copy up to samples per frame from the cursor position into the buffer
			std::memcpy(output_buffer, m_outbound_audio.m_buffer.data() + m_outbound_audio.m_read_cursor, to_copy * sizeof(int16_t));
			m_outbound_audio.m_read_cursor += to_copy; // Advance cursor position
		}

		// Zero-fill remainder (silence)
		if (to_copy < frame_sample_count) {
			std::memset(output_buffer + to_copy, 0, (frame_sample_count - to_copy) * sizeof(int16_t));
		}

		CompactAudioSamples(48000); // 3s * 16000 Hz = 48000

		a_frame.type = PJMEDIA_FRAME_TYPE_AUDIO;
	}

	void CSIPAudioBridge::Create(uint32_t a_sample_rate, uint32_t a_channels, uint32_t a_samples_per_frame) {
		pj::MediaFormatAudio audio_fmt;
		audio_fmt.type			= PJMEDIA_TYPE_AUDIO;
		audio_fmt.clockRate		= a_sample_rate;
		audio_fmt.channelCount	= a_channels;
		audio_fmt.bitsPerSample = 16;
		audio_fmt.frameTimeUsec = (a_samples_per_frame * 1000000) / a_sample_rate;

		createPort("SIPAudioBridge", audio_fmt);
	}

	std::vector<int16_t> CSIPAudioBridge::ConsumeInboundAudio() {
		std::lock_guard<std::mutex> lock(m_inbound_audio.m_mutex);

		std::vector<int16_t> consumed_audio = std::move(m_inbound_audio.m_buffer);
		m_inbound_audio.m_buffer.clear();

		return consumed_audio;
	}

	void CSIPAudioBridge::SubmitOutboundAudio(const int16_t* a_samples, uint32_t a_count) {
		std::lock_guard<std::mutex> lock(m_outbound_audio.m_mutex);
		m_outbound_audio.m_buffer.insert(m_outbound_audio.m_buffer.end(), a_samples, a_samples + a_count);
	}

	void CSIPAudioBridge::SubmitOutboundAudio(const std::vector<int16_t>& a_samples) {
		std::lock_guard<std::mutex> lock(m_outbound_audio.m_mutex);
		m_outbound_audio.m_buffer.insert(m_outbound_audio.m_buffer.end(), a_samples.begin(), a_samples.end());
	}

	void CSIPAudioBridge::ClearOutboundAudio() {
		std::lock_guard<std::mutex> lock(m_outbound_audio.m_mutex);
		m_outbound_audio.m_buffer.clear();
		m_outbound_audio.m_read_cursor = 0;
	}

	void CSIPAudioBridge::CompactAudioSamples(uint32_t a_threshold) {
		// Compacts audio once its either:
		// - Fully consumed 
		// - Or past ~3s silence threshold @ 16 kHz (default value: 48000 = 3s * 16000 Hz)
		if (m_outbound_audio.m_read_cursor > 0 && m_outbound_audio.m_read_cursor >= m_outbound_audio.m_buffer.size()) {
			m_outbound_audio.m_buffer.clear();
			m_outbound_audio.m_read_cursor = 0;
		}
		else if (m_outbound_audio.m_read_cursor > a_threshold) {
			m_outbound_audio.m_buffer.erase(m_outbound_audio.m_buffer.begin(), m_outbound_audio.m_buffer.begin() + m_outbound_audio.m_read_cursor);
			m_outbound_audio.m_read_cursor = 0;
		}
	}

	
}