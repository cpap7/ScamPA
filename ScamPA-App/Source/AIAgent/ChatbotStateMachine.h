#pragma once
#include "AIEngineManager.h"

#include <ScamPA/Audio/AudioDevice.h>
#include <ScamPA/Utilities/Timer.h>

#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <functional>


namespace SPA {

	/*  Chatbot Finite State Machine — Transition Table
		--------------------------------------------------------------
		State        | + Event           | -> Next State
		--------------------------------------------------------------
		Idle         | + Record          | -> Listening
		Recording    | + Infer           | -> Transcribing
		Recording    | + Cancel          | -> Idle
		Transcribing | + (STT done)      | -> Inferring       [worker]
		Transcribing | + (STT failed)    | -> Error           [worker]
		Transcribing | + Cancel          | -> Idle
		Inferring    | + (LLM done)      | -> Speaking        [worker]
		Inferring    | + (LLM failed)    | -> Error           [worker]
		Inferring    | + Cancel          | -> Idle
		Speaking     | + PlaybackDrained | -> Listening (chat loop)	
		Speaking     | + Cancel          | -> Idle
		Error        | + Record          | -> Listening
		--------------------------------------------------------------
	*/

	enum class EChatbotState : uint8_t {
		Idle = 0,			// Default initial state / reset state
		
		Listening,			// Capturing audio input
		Transcribing,		// STT in progress
		Inferring,			// LLM generating a response
		Speaking,			// TTS playback draining

		Error
	};

	enum class EChatbotEvent : uint8_t {
		Record,				// Begin audio capture
		Infer,				// Stop listening, run STT -> LLM -> TTS
		Cancel,				// Abort current operation
		PlaybackDrained		// TTS output buffer empty
	};

	struct SFSMResult {
		mutable std::mutex m_mutex;
		std::string m_last_stt_transcript;
		std::string m_last_llm_response;
		std::string m_last_error;
	};

	struct SFSMTokenBuffer {
		std::mutex m_mutex;
		std::string m_current_sentence;
		std::string m_full_response;
	};
	
	class CChatbotStateMachine {
	private:
		SFSMResult m_fsm_results;
		SFSMTokenBuffer m_fsm_token_buffer;

		std::thread m_worker;

		std::unique_ptr<IAudioDevice> m_audio_input_device;
		std::unique_ptr<IAudioDevice> m_audio_output_device;

		CAIEngineManager& m_manager;

		// TODO: silence = end of input
		CTimer m_silence_timer;
		float m_silence_threshold	= 0.005f; // RMS energy floor
		float m_silence_duration	= 2.0f; // Seconds of quiet after speech before auto-infer
		bool m_speech_detected		= false;

		std::atomic<EChatbotState> m_state{ EChatbotState::Idle };
		std::atomic<bool> m_cancel_requested{ false };
		std::atomic<bool> m_chat_loop{ true }; // Tracks if the chat should loop back to recording state after speaking state is done
	
	public:
		explicit CChatbotStateMachine(CAIEngineManager& a_manager);
		~CChatbotStateMachine();

		void Init();
		void Shutdown();

		void OnUpdate(); // Called each frame by the panel
		void OnEvent(EChatbotEvent a_event); // Transition table

		inline EChatbotState GetState() const		{ return m_state.load();	}
		inline CAIEngineManager& GetEngineManager()	{ return m_manager;			}

		// Pollable results
		// NOTE: We return copies here since the worker thread may be writing to m_fsm_token_buffer strings
		std::string GetLastSTTTranscript() const;
		std::string GetLastLLMResponse() const;
		std::string GetLastError() const;

		inline void SetChatLoop(bool a_toggle)	{ m_chat_loop = a_toggle;		}
		inline bool IsChatLooping() const		{ return m_chat_loop.load();	}

	private:
		
		/* Pipeline functions: STT --> LLM --> TTS */
		
		void ListenAndWait();
		void StopAndInfer();
		void RunPipeline(std::vector<int16_t> a_raw_audio);
		void CancelAll();

		// Helpers
		void Transition(EChatbotState a_next);
		void SetError(const std::string& a_message);

		// TTS streaming
		void AccumulateToken(const std::string& a_token);
		void FlushTokens();

	};
}