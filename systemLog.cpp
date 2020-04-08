#include "systemLog.h"
#include <windows.h>
#include <io.h>
#include <direct.h>

systemLog* systemLog::m_log = nullptr;

systemLog::systemLog()
{

}

systemLog::~systemLog()
{

}

std::string systemLog::GetCurrentTimeString()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    char szTime[64] = { 0 };
    sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, 
        st.wDay, st.wHour + 8, st.wMinute, st.wSecond, st.wMilliseconds);
    return szTime;
}

systemLog* systemLog::GetInstance()
{

    if (m_log == NULL)
    {
        m_log = new systemLog;
    }
    return m_log;
}

void systemLog::WriteLog(std::string &strLog, LogLevel eLevel)
{
    if (_access(LOGPATH, 0) != 0)
    {
        // ´´½¨Ä¿Â¼
        mkdir(LOGPATH);
    }

}