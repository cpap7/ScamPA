// Adapted from https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e
// Tweaked the style and naming conventions in order to fit the rest of the codebase, alongside some of the logic to fit the needs of this project
// NOTE: All .json files are outputted to the project folder w/ the source code (i.e., "Sandbox/")
// The files can be viewed via the Google Chrome browser -> chrome://tracing 

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//

#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>
#include <iomanip>

namespace SPA {
    struct SProfileResult {
        std::string m_name;

        std::chrono::duration<double, std::micro> m_start_time; // Floating point time duration in microseconds
        std::chrono::microseconds m_time_elapsed;

        std::thread::id m_thread_id;
    };

    struct SInstrumentationSession {
        std::string m_name;
    };

    class CInstrumentor {
    private:
        std::ofstream m_output_stream;
        std::mutex m_mutex;
        SInstrumentationSession* m_current_session;
        //int m_profile_count;        

    public:
        // Copy constructors
        CInstrumentor(const CInstrumentor& other) = delete;
        CInstrumentor(CInstrumentor&& other) = delete;

        void BeginSession(const std::string& new_name, const std::string& new_file_path = "results.json") {
            std::lock_guard lock(m_mutex);
            if (m_current_session) {
                // If there's already a session, it must be closed before beginning a new one
                // Profiling output for the original session will be in the new opened session instead
                // which is better than having a poorly formatted profiling output
            }
            if (CLogger::GetCoreLogger()) { // Handle edge case: BeginSession() before CLogger::Init()
                SPA_CORE_ERROR("CInstrumentor::BeginSession({0}) called when session '{1}' was already open!", new_name, m_current_session->m_name);
            }

            m_output_stream.open(new_file_path);

            if (m_output_stream.is_open()) {
                m_current_session = new SInstrumentationSession{ new_name };
                WriteHeader();
            }

            else {
                if (CLogger::GetCoreLogger()) { // Handle edge case: BeginSession() before CLogger::Init()
                    SPA_CORE_ERROR("CInstrumentor could not open results file '{0}'", new_file_path);
                }
            }

        }

        void EndSession() {
            std::lock_guard lock(m_mutex);
            InternalEndSession();
        }

        void WriteProfile(const SProfileResult& new_result) {
            // if (m_profile_count++ > 0) { m_output_stream << ","; }

            //std::string name = new_result.name;
            //std::replace(name.begin(), name.end(), '"', '\'');

            // Format the profile result as a json file (output)
            std::stringstream json_output;
            json_output << std::setprecision(3) << std::fixed;
            json_output << ",{";
            json_output << "\"cat\":\"function\",";
            json_output << "\"dur\":" << (new_result.m_time_elapsed.count()) << ',';
            json_output << "\"name\":\"" << new_result.m_name << "\",";
            json_output << "\"ph\":\"X\",";
            json_output << "\"pid\":0,";
            json_output << "\"tid\":" << new_result.m_thread_id << ",";
            json_output << "\"ts\":" << new_result.m_start_time.count();
            json_output << "}";

            // Lock mutex
            std::lock_guard lock(m_mutex);
            if (m_current_session) {
                m_output_stream << json_output.str();
                m_output_stream.flush();
            }
        }

        static CInstrumentor& Get() {
            static CInstrumentor s_instance;
            return s_instance;
        }

    private:
        CInstrumentor() : m_current_session(nullptr) {} //, m_profile_count(0) 
        ~CInstrumentor() { EndSession(); }

        void WriteHeader() {
            m_output_stream << "{\"otherData\": {},\"traceEvents\":[";
            m_output_stream.flush();
        }

        void WriteFooter() {
            m_output_stream << "]}";
            m_output_stream.flush();
        }

        // Note: must already own lock on m_mutex before calling this function
        void InternalEndSession() {
            WriteFooter();
            m_output_stream.close();
            delete m_current_session;
            //m_current_session = nullptr;
        }
    };

    class CInstrumentationTimer {
    private:
        std::chrono::time_point<std::chrono::steady_clock> m_start_timepoint;
        const char* m_name;

        bool m_stopped;

    public:
        CInstrumentationTimer(const char* new_name) : m_name(new_name), m_stopped(false) {
            m_start_timepoint = std::chrono::steady_clock::now();
        }

        ~CInstrumentationTimer() {
            if (!m_stopped) { Stop(); }
        }

        void Stop() {
            std::chrono::steady_clock::time_point end_timepoint = std::chrono::steady_clock::now();

            auto high_resolution_start = std::chrono::duration<double, std::micro>{ m_start_timepoint.time_since_epoch() };
            auto elapsed_time = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_start_timepoint).time_since_epoch();

            CInstrumentor::Get().WriteProfile({ m_name, high_resolution_start, elapsed_time, std::this_thread::get_id() });

            m_stopped = true;
        }

    };
}

namespace Utilities {

    template <size_t T>
    struct SChangeResult {
        char m_data[T];
    };

    template <size_t T, size_t U>
    constexpr auto CleanupOutputString(const char(&expr)[T], const char(&remove)[U]) {
        SChangeResult<T> result = {};

        size_t src_index = 0;
        size_t dst_index = 0;

        while (src_index < T) {
            size_t match_index = 0;
            while (match_index < U - 1 && src_index + match_index < T - 1 && expr[src_index + match_index] == remove[match_index])
                match_index++;
            if (match_index == U - 1)
                src_index += match_index;
            result.m_data[dst_index++] = expr[src_index] == '"' ? '\'' : expr[src_index];
            src_index++;
        }
        return result;
    }
}

#define SPA_PROFILE 0
#if SPA_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts. The syntax highlighting
// could mark the wrong one in the editor
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #define SPA_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
    #define SPA_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
    #define SPA_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
    #define SPA_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
    #define SPA_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define SPA_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
    #define SPA_FUNC_SIG __func__
#else
    #define SPA_FUNC_SIG "SPA_FUNC_SIG unknown!"
#endif
#define SPA_PROFILE_BEGIN_SESSION(new_name, new_filepath) ::SPA::CInstrumentor::Get().BeginSession(new_name, new_filepath)
#define SPA_PROFILE_END_SESSION() ::SPA::CInstrumentor::Get().EndSession()
#define SPA_PROFILE_SCOPE_LINE2(new_name, new_line) constexpr auto fixed_name##line = ::SPA::Utilities::CleanupOutputString(name, "__cdecl ");\
                                            											 ::SPA::CInstrumentationTimer timer##line(fixed_name##line.m_data)
#define SPA_PROFILE_SCOPE_LINE(new_name, new_line) SPA_PROFILE_SCOPE_LINE2(new_name, new_line)
#define SPA_PROFILE_SCOPE(new_name) SPA_PROFILE_SCOPE_LINE(new_name, __LINE__)
#define SPA_PROFILE_FUNCTION() SPA_PROFILE_SCOPE(SPA_FUNC_SIG)

#else
    #define SPA_PROFILE_BEGIN_SESSION(name, filepath)
    #define SPA_PROFILE_END_SESSION()
    #define SPA_PROFILE_SCOPE(name)
    #define SPA_PROFILE_FUNCTION()
#endif