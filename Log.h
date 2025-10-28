#pragma once


// C++标准库
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <ctime>

// C++17文件系统（需特殊处理）
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

// Windows头文件（放在最后）
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _HAS_STD_BYTE 0
#include <windows.h>
#undef _HAS_STD_BYTE

#ifdef _DEBUG
#include <Windows.h>
#define DEBUG_LOG(message) { \
    std::string msg = "[DEBUG] " + std::string(message) + "\n"; \
    OutputDebugStringA(msg.c_str()); \
}
#endif


#define LOG_WRITE(module, msg) Log::GetInstance().Write(module, msg, __FILE__, __func__, __LINE__)
#define LOG_ERROR(msg) LOG_WRITE("ERROR", msg)
#define LOG_INFO(msg) LOG_WRITE("INFO", msg)
#define LOG_DEBUG(msg) LOG_WRITE("DEBUG", msg)
#define LOG_WARNING(msg) LOG_WRITE("WARNING", msg)
#define LOG_CGI(msg) LOG_WRITE("CGI", msg)
#define LOG_DB(msg) LOG_WRITE("DB", msg)
#define LOG_SPEED(msg) LOG_WRITE("SPEED", msg)
#define LOG_VISUAL(msg) LOG_WRITE("VISUAL", msg)

class Log
{
private:
    // 私有构造函数（禁止外部实例化）
    Log();
    ~Log();
    // 删除拷贝构造和赋值
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;    

    void CloseAll();

    std::string GenerateLogFileName(const std::string& modulename);
public:
    static Log& GetInstance() 
    {
        static Log instance;
        return instance;
    }

    // 写入日志
public:
    template<typename T>
    void Write(const std::string& moduleName,
        const T& message,
        const char* file,
        const char* function,
        int line) 
    {
        WriteImpl(moduleName, ToString(message), file, function, line);
    }

private:
    void WriteImpl(const std::string& moduleName,
        const std::string& message,
        const char* file,
        const char* function,
        int line);

    // 替换原有的to_string实现
    template<typename T>
    static std::string ToString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // 特化版本处理常见类型
    static std::string ToString(const std::string& str) { return str; }
    static std::string ToString(const char* str) { return str ? str : ""; }
    static std::string ToString(char* str) { return str ? str : ""; }
    static std::string ToString(bool value) { return value ? "true" : "false"; }

    // 数值类型的特化版本（避免使用std::to_string）
    static std::string ToString(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string ToString(double value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string ToString(float value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string ToString(long value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string ToString(unsigned int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

private:
    std::ofstream m_ofs;
    std::mutex m_mutex;  // 保证线程安全
    std::unordered_map<std::string, std::ofstream> m_logFiles;
};

