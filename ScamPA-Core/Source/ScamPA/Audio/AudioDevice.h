#pragma once
#include <cstdint>
#include <vector>
#include <mutex>

// Forward declarations
struct ma_device; 

namespace SPA {

	enum class EAudioSampleFormat : uint8_t {
		Int16	= 0,
		Float32 = 1,

		Default = Int16
	};

	enum class EAudioDeviceType : uint8_t {
		Capture		= 0,	// Audio input from external device (i.e., microphone)
		Loopback	= 1,	// Audio input from system audio (i.e., speakers)
		Playback	= 2,	// Audio output to system audio (i.e., speakers)
		
		Default		= Capture
	};

	struct SAudioResource { // Thread-safe resource used by derived classes for capturing samples
		mutable std::mutex m_mutex;
		std::vector<int16_t> m_buffer;
	};

	// TODO: Implement function for getting input & output device lists
	struct SAudioDeviceInfo {
		std::string m_name = "";
		std::string m_device_id = "";	// Platform-specific ID
		bool m_is_default = false;
	};
	
	struct SAudioDeviceConfig {
		uint32_t m_sample_rate	= 16000; // 16kHz for whisper.cpp
		uint32_t m_channels		= 1;	 // Mono audio
		
		EAudioSampleFormat m_sample_format	= EAudioSampleFormat::Default;
		EAudioDeviceType m_device_type		= EAudioDeviceType::Default;
	};


	class IAudioDevice {
	protected:
		SAudioDeviceConfig m_config;
		ma_device* m_device = nullptr;
		bool m_is_active = false;

	public:
		virtual ~IAudioDevice() = default;

		// Lifecycle
		virtual void Init(const SAudioDeviceConfig& a_config) = 0;
		virtual void Shutdown() = 0;
		virtual void Start()	= 0;
		virtual void Stop()		= 0;

		//virtual std::vector<SAudioDeviceInfo> GetDeviceList() {}

		// Getters
		inline const SAudioDeviceConfig& GetConfig() const	{ return m_config;					}
		inline uint32_t GetSampleRate()	const				{ return m_config.m_sample_rate;	}
		inline uint32_t GetChannels() const					{ return m_config.m_channels;		}
		inline ma_device* GetDevice() const					{ return m_device;					}
		inline EAudioSampleFormat GetSampleFormat() const	{ return m_config.m_sample_format;	}
		inline EAudioDeviceType GetDeviceType()	const		{ return m_config.m_device_type;	}
		inline bool IsActive() const						{ return m_is_active;				}


		// Factory
		static std::unique_ptr<IAudioDevice> Create(const SAudioDeviceConfig a_config);
	};

}
