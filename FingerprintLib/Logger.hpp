#pragma once
#include <string_view>
#include <memory>
#include <charconv>
#include <atomic>

#include "FastPimpl.hpp"
#include "Platform.hpp"

class Logger
{

public:

    static Logger& Instance();

    void logDebugEndl( std::string_view _toLog );

    void logDebug( std::string_view _toLog );

    template< typename TToLog >
    static constexpr bool IsStringType( TToLog&& _toLog )
    {
        static_cast<void>(_toLog);

        using TClearType = typename std::decay<TToLog>::type;
        constexpr bool IsStringType = std::is_same_v<TClearType,std::string >
                ||  std::is_same_v<TClearType,std::string_view >
                ||  std::is_same_v<TClearType,const char *>;
        return IsStringType;
    }

    template< typename TToLog >
    void logDebugEndl( TToLog&& _toLog )
    {
        if constexpr (!IsStringType(_toLog))
        {
            std::array<char, sizeof (TToLog)* 8> str{};
            if( auto [p, ec] = std::to_chars(str.data(), str.data() + str.size(),_toLog); ec == std::errc() )
            {
                logDebugEndl( std::string_view( str.data(), p - str.data() ) );
            }
        }
        else{
            logDebugEndl( static_cast<std::string_view>(_toLog) );
        }

    }

    template< typename TToLog>
    void logDebug( TToLog&& _toLog )
    {
        if constexpr (!IsStringType(_toLog))
        {
            std::array<char, sizeof (TToLog)* 8> str{};
            if( auto [p, ec] = std::to_chars(str.data(), str.data() + str.size(),_toLog); ec == std::errc() )
            {
                logDebug( std::string_view( str.data(), p - str.data() ) );
            }
        }
        else{
            logDebug( static_cast<std::string_view>(_toLog) );
        }
    }


private:

    Logger();
    ~Logger();

private:

    static constexpr inline  std::size_t kImplSize =  Platform::LogerImplSize;
    static constexpr inline  std::size_t kImplAlignment = Platform::LogerImplAlignment;

private:

    mutable std::atomic_flag m_loggerReady;

    class LoggerImpl;
    Utils::FastPimpl<LoggerImpl,kImplSize,kImplAlignment> m_pLoggerImpl;
};
