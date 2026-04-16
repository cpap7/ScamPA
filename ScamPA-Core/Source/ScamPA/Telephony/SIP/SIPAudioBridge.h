#pragma once
#include "../../Audio/AudioIOResources.h"

#include <pjsua2.hpp>

#include <vector>
#include <cstdint>

namespace SPA {

	/* Custom audio media port which bridges PJSIP RTP audio <-> STT/TTS audio pipeline
	*  Mimics functionality from CAudioInputDevice & CAudioOutputDevice (minus the coupling to miniaudio)
	* 
	*  Inbound (caller --> STT)
	*	- onFrameReceived() is called via PJSIP with PCM samples from the caller
	*	- Samples are then buffered so the STT can consume the audio buffer (via ConsumeBuffer())
	*  
	*  Outbound (TTS --> caller)
	*	- onFrameRequested() is called via PJSIP when it needs PCM to send
	*	- Samples are supplied from TTS output buffer
	*/

	class CSIPAudioBridge : public pj::AudioMediaPort {
	private:
		SAudioOutput m_outbound_audio;
		SAudioInput m_inbound_audio;

	public: // PJSIP overrides 
		// Reinterprets PJSIP byte frame (byte vector) as int16_t*, and appends to inbound buffer under lock
		virtual void onFrameReceived(pj::MediaFrame& a_frame) override;		// Caller's audio arrives here

		// Copies samples from cursor position into the frame, zero-fills remainder (as silence) 
		// and compacts buffer using silence threshold strategy ( 
		virtual void onFrameRequested(pj::MediaFrame& a_frame) override;	// Frame is filled w/ TTS audio to be sent to caller

	public:
		// Registers custom media port w/ PJSIP via createPort(),
		// using a MediaAudioFormat descriptor (16-bit PCM, 16 kHz mono audio, 20 ms frames)
		void Create(uint32_t a_sample_rate = 16000, uint32_t a_channels = 1, uint32_t a_samples_per_frame = 320);

		/* STT pipeline */
		std::vector<int16_t> ConsumeInboundAudio(); // Move-drains inbound buffer atomically; STT-engine calls to pull accumulated caller audio

		/* TTS pipeline */
		void SubmitOutboundAudio(const int16_t* a_samples, uint32_t a_count);
		void SubmitOutboundAudio(const std::vector<int16_t>& a_samples);
		void ClearOutboundAudio();

	private: // Internal helpers
		void CompactAudioSamples(uint32_t a_threshold = 48000);
	};
}

