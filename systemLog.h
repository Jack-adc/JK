#pragma once

#include <string>
#define LOGPATH "./log"

typedef enum  LogLevel
{
    E_LOG_DEBUG = 0X00,//������־
    E_LOG_INFO = 0X01,//��ͨ��־
    E_LOG_WARN = 0X02,//������־
    E_LOG_ERROR = 0X03,//������־
    E_LOG_FATAL = 0X04,//���ش�����־
    E_LOG_ALL = 0X05,//��ʾȫ����־
    E_LOG_OFF = 0X06,//�ر���־
}; LogLevel;


typedef enum LogInfo
{
    E_TIME = 0,//��־ʱ��
    E_LEVEL = 1,//��־����
    E_INFO = 2,//��־��Ϣ
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