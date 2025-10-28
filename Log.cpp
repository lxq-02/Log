#include "Log.h"


Log::Log()
{
    // 创建日志目录，如果不存在，就创建
    std::filesystem::create_directory("Log");
}

Log::~Log() 
{
    CloseAll();
}

void Log::CloseAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_logFiles)
    {
        if (pair.second.is_open())
        {
            pair.second.close();
        }
    }
    m_logFiles.clear();
}

std::string Log::GenerateLogFileName(const std::string& modulename)
{
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);

    char timeStr[64];
    tm localTime;
    localtime_s(&localTime, &nowTime);
    std::strftime(timeStr, sizeof(timeStr), "Log_%Y年%m月%d日%H时%M分%S秒", &localTime);

    std::string fileName = modulename + "_" + std::string(timeStr) + ".log";
    return fileName;
}

// 写入日志
//void Log::Write(const std::string& message) 
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if (m_ofs.is_open()) 
//    {
//        m_ofs << message << std::endl;
//    }
//}

void Log::WriteImpl(const std::string& moduleName, const std::string& message, const char* file, const char* function, int line)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string fileName = GenerateLogFileName(moduleName);

    if (m_logFiles.find(moduleName) == m_logFiles.end())
    {
        std::string fullPath = "Log/" + fileName;

        // 先以 trunc 模式清空文件（若文件不存在则创建）
        std::ofstream(fullPath, std::ios::out | std::ios::trunc).close();

        // 以追加模式打开文件
        m_logFiles[moduleName] = std::ofstream(fullPath, std::ios::app);
        if (!m_logFiles[moduleName].is_open())
        {
            // DEBUG_LOG("无法打开日志文件：" + fullPath);
        }
    }

    if (m_logFiles[moduleName].is_open())
    {
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);

        // 格式化时间
        char timeStr[64];
		tm localTime;
		localtime_s(&localTime, &nowTime);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &localTime);

        // 获取毫秒
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
        // 写入日志
        m_logFiles[moduleName] << "[" << timeStr << "."
            << std::setfill('0') << std::setw(3) << ms.count() << "] "
            << "[" << (file ? file : "unknown") << ":" << line << "]["
            << (function ? function : "unknown") << "] "
            << message << std::endl;
    }
}

//void Log::Write(const std::string& fileName, const std::string& message, const char* file, const char* function, int line)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//
//    if (m_logFiles.find(fileName) == m_logFiles.end())
//    {
//        std::string fullPath = "Log/" + fileName;
//
//        // 先以 trunc 模式清空文件（若文件不存在则创建）
//        std::ofstream(fullPath, std::ios::out | std::ios::trunc).close();
//
//        // 以追加模式打开文件
//        m_logFiles[fileName] = std::ofstream(fullPath, std::ios::app);
//        if (!m_logFiles[fileName].is_open())
//        {
//            DEBUG_LOG("无法打开日志文件：" + fullPath);
//        }
//    }
//
//    if (m_logFiles[fileName].is_open())
//    {
//        // 获取当前时间
//        auto now = std::chrono::system_clock::now();
//        auto nowTime = std::chrono::system_clock::to_time_t(now);
//
//        // 格式化时间
//        char timeStr[64];
//        tm localTime;
//        localtime_s(&localTime, &nowTime);
//        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &localTime);
//
//        // 获取毫秒
//        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
//
//        // 写入日志
//        m_logFiles[fileName] << "[" << timeStr << "." << std::setfill('0') << std::setw(3) << ms.count() << "] "
//            << "[" << file << ":" << line << "][" << function << "] "
//            << message << std::endl;
//    }
//}
