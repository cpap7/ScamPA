// config_site.h — minimal config 
#define PJ_HAS_SSL_SOCK 0       // Disable for now, enable later with OpenSSL
#define PJMEDIA_HAS_VIDEO 0     // Audio only
#define PJMEDIA_HAS_FFMPEG 0
#define PJMEDIA_AUDIO_DEV_HAS_PORTAUDIO 0
#define PJMEDIA_AUDIO_DEV_HAS_WMME 1   // Windows audio (required even with custom port)
