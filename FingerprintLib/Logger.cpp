#include "Logger.hpp"

#include <QtCore/QDebug>

#include <iostream>

namespace
{
    std::string_view CaretReset = "\r\n";
}

class Logger::LoggerImpl
{

public:
    void logString(std::string_view _toLog) const
    {
        const std::string& toLog = _toLog.data();
        qDebug() << QString::fromStdString(toLog);
    }
};



Logger::Logger() = default;

Logger::~Logger() = default;

Logger& Logger::Instance()
{
    static Logger intance;
    return intance;
}

void Logger::logDebugEndl( std::string_view _toLog )
{
    while ( m_loggerReady.test_and_set( std::memory_order_acquire ) )
    {
        m_pLoggerImpl->logString( _toLog );
        //m_pLoggerImpl->logString( CaretReset );

        m_loggerReady.clear( std::memory_order_release );
    }
}

void Logger::logDebug( std::string_view _toLog )
{
    while ( m_loggerReady.test_and_set( std::memory_order_acquire ) )
    {
        m_pLoggerImpl->logString( _toLog );
        m_loggerReady.clear( std::memory_order_release );
    }
}
