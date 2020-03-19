#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

#define PROFILING 1
#ifdef PROFILING
    #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION()  PROFILE_SCOPE(__FUNCTION__)
#else
    #define PROFILE_SCOPE(name)
#endif

struct ProfileResult
{
    const std::string name;
    long long start, end;
    uint32_t threadID;
};

class Instrumentor
{
    std::string     m_sessionName   = "None";
    std::ofstream   m_outputStream;
    int             m_profileCount  = 0;
    std::mutex      m_lock;
    bool            m_activeSession = false;

    Instrumentor() { }

public:

    static Instrumentor& Instance()
    {
        static Instrumentor instance;
        return instance;
    }

    ~Instrumentor()
    {
        endSession();
    }

    void beginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        if (m_activeSession) { endSession(); }
        m_activeSession = true;
        m_outputStream.open(filepath);
        writeHeader();
        m_sessionName = name;
    }

    void endSession()
    {
        if (!m_activeSession) { return; }
        m_activeSession = false;
        writeFooter();
        m_outputStream.close();
        m_profileCount = 0;
    }

    void writeProfile(const ProfileResult& result)
    {
        std::lock_guard<std::mutex> lock(m_lock);

        if (m_profileCount++ > 0) { m_outputStream << ","; }

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << "{";
        m_outputStream << "\"cat\":\"function\",";
        m_outputStream << "\"dur\":" << (result.end - result.start) << ',';
        m_outputStream << "\"name\":\"" << name << "\",";
        m_outputStream << "\"ph\":\"X\",";
        m_outputStream << "\"pid\":0,";
        m_outputStream << "\"tid\":" << result.threadID << ",";
        m_outputStream << "\"ts\":" << result.start;
        m_outputStream << "}";
    }

    void writeHeader()
    {
        m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
    }

    void writeFooter()
    {
        m_outputStream << "]}";
    }
};

class InstrumentationTimer
{
    ProfileResult m_result;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_stopped;

public:

    InstrumentationTimer(const std::string & name)
        : m_result({ name, 0, 0, 0 })
        , m_stopped(false)
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_stopped) { stop(); }
    }

    void stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        m_result.end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        m_result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Instance().writeProfile(m_result);

        m_stopped = true;
    }
};