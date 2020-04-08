#pragma once

#include <string>
#define LOGPATH "./log"

typedef enum  LogLevel
{
    E_LOG_DEBUG = 0X00,//调试日志
    E_LOG_INFO = 0X01,//普通日志
    E_LOG_WARN = 0X02,//警告日志
    E_LOG_ERROR = 0X03,//错误日志
    E_LOG_FATAL = 0X04,//严重错误日志
    E_LOG_ALL = 0X05,//显示全部日志
    E_LOG_OFF = 0X06,//关闭日志
}; LogLevel;


typedef enum LogInfo
{
    E_TIME = 0,//日志时间
    E_LEVEL = 1,//日志级别
    E_INFO = 2,//日志信息
}; LogInfo;

class systemLog
{
public:
    void WriteLog(std::string &strLog, LogLevel eLevel = E_LOG_DEBUG);
    static systemLog * GetInstance();

private:
    systemLog();
    ~systemLog();
    static systemLog *m_log;
    std::string GetCurrentTimeString();
};