/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
//#include <Types/Delegate.h>
#include <HAL/DataTypes.h>
#include <Memory/MemoryBlockArena.h>

#ifdef MR_DEBUG
constexpr inline const bool bIsRunningDebugMode = true;
#else
constexpr inline const bool bIsRunningDebugMode = false;
#endif // MR_DEBUG

class MemoryBlockPool;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

//CREATE_DELEGATE(LoggerInitialize);

struct LogEntry {};

enum LogSeverity : u8
{
	Log = 1,
	Info = Log,
	Verbose,
	Warn,
	Error,
	Fatal
};

// * Logger the Core's beloved logging system, use MR_LOG() to log your wants.
class CORE_API Logger
{
	static constexpr const u32 MAX_LOG_ENTRIES = 2048;

	/* u8 why ? : category(enum : u8 also)	(reserved)
				  0							0000000		*/
	struct LogEntry
	{
		u16 severity : 3;

		const char* message;
	};

public:
	static Logger* Get();
	static u16 AddLogCategory(const char* name);

	Logger() = default;
	Logger(Logger* newInstance);
	Logger(Logger&&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger& operator=(Logger&&) = delete;

	virtual ~Logger() noexcept;

	void Init();
	virtual void Shutdown();

	void LogStandard(const u16& category, LogSeverity severity, const void* message, ...);
	void LogFatal(const u16& category, LogSeverity severity, const void* message, const void* function, const u32 line, const void* file, ...);
	void LogAssert();

	void SendToOutputBuffer(void* buffer, const u32 count, LogSeverity severity);

protected:
	static bool PrepareLoggingSystem();

	const char* FindLogCategory(const u16& name);

	static constexpr inline const char* FormatSeverity(LogSeverity Severity) noexcept
	{
		switch (Severity)
		{
		case LogSeverity::Log:
			return "Log";
		case LogSeverity::Warn:
			return "Warning";
		case LogSeverity::Error:
			return "Error";
		case LogSeverity::Fatal:
			return "Fatal";
		case LogSeverity::Verbose:
			return "Verbose";
		}

		return "???";
	}

private:
	MemoryBlockArena* loggingArena;

	static inline Logger* instance = nullptr;
};



#include "LogMacros.h"