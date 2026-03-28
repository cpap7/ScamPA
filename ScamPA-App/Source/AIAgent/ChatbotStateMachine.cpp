#include "ChatbotStateMachine.h"
#include <ScamPA/Core/Logger.h>
#include <ScamPA/Audio/AudioInputDevice.h>
#include <ScamPA/Audio/AudioOutputDevice.h>

namespace SPA {
	
	namespace Utilities {

		static bool IsEndOfSentence(char a_char) {
			return a_char == '.' || a_char == '?' || a_char == '!' || a_char == '\n';
		}

		static std::string ChatbotStateToString(EChatbotState a_state) {
			std::string state_str = "";
			switch (a_state) {
				case EChatbotState::Idle:				state_str = "IDLE";			break;
				case EChatbotState::Listening:			state_str = "LISTENING";	break;
				case EChatbotState::Transcribing:		state_str = "TRANSCRIBING";	break;
				case EChatbotState::Inferring:			state_str = "INFERRING";	break;
				case EChatbotState::Speaking:			state_str = "SPEAKING";		break;
				case EChatbotState::Error:				state_str = "ERROR";		break;
				default:								return "UNKNOWN";
			}
 			return state_str.empty() ? "UNKNOWN" : state_str;
		}

		static std::vector<float> ConvertSamples(const std::vector<int16_t>& a_raw_samples) {
			std::vector<float> float_samples(a_raw_samples.size());

			for (size_t i{}; i < a_raw_samples.size(); ++i) {
				float_samples[i] = static_cast<float>(a_raw_samples[i] / 32768.0f);
			}

			return float_samples;
		}
	}

	CChatbotStateMachine::CChatbotStateMachine(CAIEngineManager& a_manager)
		: m_manager(a_manager) {
		Init();
	}

	CChatbotStateMachine::~CChatbotStateMachine() {
		Shutdown();
	}

	void CChatbotStateMachine::Init() {
		// Setup audio input device (STT / whisper.cpp)
		SAudioDeviceConfig input_cfg;
		input_cfg.m_sample_rate		= 16000; // whisper.cpp default
		input_cfg.m_channels		= 1;
		input_cfg.m_sample_format	= EAudioSampleFormat::Int16;
		input_cfg.m_device_type		= EAudioDeviceType::Loopback; // Default = loopback (audio from speakers)
		m_audio_input_device		= IAudioDevice::Create(input_cfg);

		// Setup audio output device (TTS / piper)
		SAudioDeviceConfig output_cfg;
		output_cfg.m_sample_rate	= 22050; // piper default
		output_cfg.m_channels		= 1;
		output_cfg.m_sample_format	= EAudioSampleFormat::Int16;
		output_cfg.m_device_type	= EAudioDeviceType::Playback; // Play to speakers
		m_audio_output_device		= IAudioDevice::Create(output_cfg);

	}

	void CChatbotStateMachine::Shutdown() {
		CancelAll();
		m_audio_input_device.reset();
		m_audio_output_device.reset();
	}

	void CChatbotStateMachine::OnUpdate() {
		// Listening -> Inferring
		if (m_state == EChatbotState::Listening) {
			auto* input_device = static_cast<CAudioInputDevice*>(m_audio_input_device.get());
			if (!input_device || input_device->GetBufferedSampleCount() == 0) {
				return;
			}

			// ~300 ms window @ 16 kHz
			float rms_energy = input_device->GetRecentRMSEnergy(4800);
			if (rms_energy > m_silence_threshold) {
				m_speech_detected = true;
				m_silence_timer.Reset();
			}
			else if (m_speech_detected && m_silence_timer.GetTimeElapsed() >= m_silence_duration) {
				SPA_CORE_INFO("(Chatbot FSM) Silence detected after speech. Auto-inferring...");
				m_speech_detected = false;
				OnEvent(EChatbotEvent::Infer);
			}
			return;
		}

		// Speaking -> Listening 
		if (m_state == EChatbotState::Speaking) {
			auto* output_device = static_cast<CAudioOutputDevice*>(m_audio_output_device.get());
			if (output_device->GetBufferedSampleCount() == 0) { // Check if the audio samples are drained
				OnEvent(EChatbotEvent::PlaybackDrained);
			}
			
		}

	}

	void CChatbotStateMachine::OnEvent(EChatbotEvent a_event) {
		// TODO: Consider using nested switch
		switch (m_state.load()) {
			case EChatbotState::Idle: {
				if (a_event == EChatbotEvent::Record)		{ ListenAndWait(); }
				break;
			}
			case EChatbotState::Listening: {
				if (a_event == EChatbotEvent::Infer)		{ StopAndInfer(); }
				else if (a_event == EChatbotEvent::Cancel)	{ CancelAll(); }
				break;
			}

			case EChatbotState::Transcribing:
			case EChatbotState::Inferring: {
				if (a_event == EChatbotEvent::Cancel)		{ CancelAll(); }
				break;
			}

			case EChatbotState::Speaking: {
				if (a_event == EChatbotEvent::PlaybackDrained) {
					SPA_CORE_INFO("(Chatbot FSM) Playback complete");
					if (m_chat_loop)						{ ListenAndWait(); }
				}
				else if (a_event == EChatbotEvent::Cancel)	{ CancelAll(); }
				break;
			}

			case EChatbotState::Error: { 
				// NOTE: Error logs will be handled & show up within the console 
				// so we can just quickly transition states to avoid hiccups
				if (a_event == EChatbotEvent::Record)		{ ListenAndWait(); }
				else if (a_event == EChatbotEvent::Cancel)	{ Transition(EChatbotState::Idle); }
				break;
			}
		}
 	}


	std::string CChatbotStateMachine::GetLastSTTTranscript() const {
		std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
		return m_fsm_results.m_last_stt_transcript;
	}

	std::string CChatbotStateMachine::GetLastLLMResponse() const {
		std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
		return m_fsm_results.m_last_llm_response;
	}

	std::string CChatbotStateMachine::GetLastError() const {
		std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
		return m_fsm_results.m_last_error;
	}

	void CChatbotStateMachine::ListenAndWait() {
		{ // Clear stale results from previous cycle
			std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
			m_fsm_results.m_last_stt_transcript.clear();
			m_fsm_results.m_last_llm_response.clear();
			m_fsm_results.m_last_error.clear();
		}

		auto* input_device = static_cast<CAudioInputDevice*>(m_audio_input_device.get());
		if (!input_device) {
			SPA_CORE_WARN("(Chatbot FSM) No input device available!");
			return;
		}

		m_cancel_requested.store(false);
		m_speech_detected = false;
		m_silence_timer.Reset();
		Transition(EChatbotState::Listening);
		input_device->Start();
	}

	void CChatbotStateMachine::StopAndInfer() {
		auto* input_device = static_cast<CAudioInputDevice*>(m_audio_input_device.get());
		if (!input_device) {
			SPA_CORE_WARN("(Chatbot FSM) No input device available!");
			return;
		}

		input_device->Stop();
		std::vector<int16_t> samples = input_device->ConsumeBuffer();
		if (samples.empty()) {
			SPA_CORE_WARN("(Chatbot FSM) No audio captured! Returning to idle state...");
			Transition(EChatbotState::Idle);
			return;
		}

		Transition(EChatbotState::Transcribing);
		if (m_worker.joinable()) {
			m_worker.join();
		}
		m_worker = std::thread(&CChatbotStateMachine::RunPipeline, this, std::move(samples));
	}

	void CChatbotStateMachine::RunPipeline(std::vector<int16_t> a_raw_audio) {
		auto* stt_engine = m_manager.GetSTTEngine();
		auto* llm_engine = m_manager.GetLLMEngine();
		auto* tts_engine = m_manager.GetTTSEngine();

		if (!stt_engine || !llm_engine || !tts_engine) {
			SetError("One or more AI engines are not loaded!");
			return;
		}

		// State: Transcribing
		// State should be set via OnEvent() at this stage
		std::vector<float> float_audio = Utilities::ConvertSamples(a_raw_audio);
		a_raw_audio.clear();

		VoxBox::STranscriptResult stt_result = stt_engine->Transcribe(float_audio);
		float_audio.clear();

		if (m_cancel_requested) {
			Transition(EChatbotState::Idle);
			return;
		}

		if (!stt_result.Success() || stt_result.Text().empty()) {
			SetError("STT Transcription failed");
			return;
		}

		std::string transcript = stt_result.Text();
		SPA_CORE_INFO("(Chatbot FSM) Transcript: {0}", transcript);
		{
			std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
			m_fsm_results.m_last_stt_transcript = transcript;
		}

		// State: Inferring
		Transition(EChatbotState::Inferring);
		{
			std::lock_guard<std::mutex> lock(m_fsm_token_buffer.m_mutex);
			m_fsm_token_buffer.m_current_sentence.clear();
			m_fsm_token_buffer.m_full_response.clear();
		}

		// Setup token callback for sentence-level TTS chunks
		llm_engine->SetTokenCallback([this](int /*a_id*/, const char* a_text, int a_type, const float* /*a_probs*/) -> int {
			if (m_cancel_requested) {
				return 0;
			}

			auto token_type = static_cast<VoxBox::ETokenType>(a_type);
			if (token_type == VoxBox::ETokenType::SampledNonEOGNonCtrl) {
				AccumulateToken(a_text ? a_text : "");
			}

			return 1;
		});

		VoxBox::SInferenceResult llm_result = llm_engine->Query(transcript);
		llm_engine->SetTokenCallback(nullptr);
		FlushTokens(); // Flush remaining partial sequence

		{
			std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
			std::lock_guard<std::mutex> token_lock(m_fsm_token_buffer.m_mutex);
			m_fsm_results.m_last_llm_response = m_fsm_token_buffer.m_full_response; // NOTE: Populated via AccumulateToken()
		}

		if (!llm_result.Success()) {
			SetError("LLM inference failed");
			return;
		}

		if (m_cancel_requested) {
			Transition(EChatbotState::Idle);
			return;
		}

		// State: Speaking
		Transition(EChatbotState::Speaking);
	}

	void CChatbotStateMachine::CancelAll() {
		m_cancel_requested = true;

		// Stop audio devices
		if (m_audio_input_device && m_audio_input_device->IsActive()) {
			m_audio_input_device->Stop();
		}
		if (m_audio_output_device && m_audio_output_device->IsActive()) {
			m_audio_output_device->Stop();
		}

		// Stop STT
		auto* stt_engine = m_manager.GetSTTEngine();
		if (stt_engine) {
			stt_engine->Cancel();
		}

		if (m_worker.joinable()) {
			m_worker.join();
		}

		{ // Clear stale results, so UI doesn't poll old data
			//std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
			//m_fsm_results.m_last_stt_transcript.clear();
			//m_fsm_results.m_last_llm_response.clear();
			//m_fsm_results.m_last_error.clear();
		}

		Transition(EChatbotState::Idle);
	}

	void CChatbotStateMachine::Transition(EChatbotState a_next) {
		std::string current_state = Utilities::ChatbotStateToString(m_state);
		std::string next_state = Utilities::ChatbotStateToString(a_next);
		SPA_CORE_TRACE("(Chatbot FSM) Transitioning state: {0} -> {1}", current_state, next_state);
		m_state = a_next;
	}
 
	void CChatbotStateMachine::SetError(const std::string& a_message) {
		{
			std::lock_guard<std::mutex> lock(m_fsm_results.m_mutex);
			m_fsm_results.m_last_error = a_message;
		}
		SPA_CORE_WARN("(Chatbot FSM) Error: {0}", a_message);
		m_state = EChatbotState::Error;
	}

	void CChatbotStateMachine::AccumulateToken(const std::string& a_token) {
		std::lock_guard<std::mutex> lock(m_fsm_token_buffer.m_mutex);

		m_fsm_token_buffer.m_current_sentence += a_token;
		m_fsm_token_buffer.m_full_response += a_token;

		// Synthesize audio, then queue audio after an ending punctuation is found within the sentence
		if (!m_fsm_token_buffer.m_current_sentence.empty() && Utilities::IsEndOfSentence(m_fsm_token_buffer.m_current_sentence.back())) {
			auto* tts_engine = m_manager.GetTTSEngine();
			auto* output_device = static_cast<CAudioOutputDevice*>(m_audio_output_device.get());

			if (tts_engine && output_device) {
				VoxBox::SAudioResult audio = tts_engine->Synthesize(m_fsm_token_buffer.m_current_sentence.c_str());
				
				if (audio.Success()) {
					SPA_CORE_INFO("(Chatbot FSM) Generated response: {0}", m_fsm_token_buffer.m_current_sentence);
					output_device->SubmitSamples(audio.SampleData(), audio.SampleCount());

					if (!output_device->IsActive()) {
						output_device->Start(); // Force start
					}
					
				}
			}
			m_fsm_token_buffer.m_current_sentence.clear();
		}
	}

	void CChatbotStateMachine::FlushTokens() {
		std::lock_guard<std::mutex> lock(m_fsm_token_buffer.m_mutex);
		
		if (m_fsm_token_buffer.m_current_sentence.empty()) {
			return;
		}
		
		// Synthesize & queue audio for the rest of the response before clearing
		auto* tts_engine = m_manager.GetTTSEngine();
		auto* output_device = static_cast<CAudioOutputDevice*>(m_audio_output_device.get());
		if (tts_engine && output_device) {
			VoxBox::SAudioResult audio = tts_engine->Synthesize(m_fsm_token_buffer.m_current_sentence);
			
			if (audio.Success()) {
				output_device->SubmitSamples(audio.SampleData(), audio.SampleCount());
				
				if (!output_device->IsActive()) {
					output_device->Start(); // Force start
				}
			}
		}
		m_fsm_token_buffer.m_current_sentence.clear();
		
	}
}