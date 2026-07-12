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

// * Logger the Core's beloved logging system, use MR_LOG() to log your wants.
class CORE_API Logger
{
	static constexpr const u32 MAX_LOG_ENTRIES = 2048;

	/* u8 why ? : category(enum : u8 also)	(reserved)
				  0							0000000		*/
	struct LogEntry
	{
		struct
		{
			//u8 
		} payload;

		//void* payload;
		u8 flag;
	};

	enum LogSeverity : u8
	{
		Log = 1,
		Info = Log,
		Verbose,
		Warn,
		Error,
		Fatal
	};

public:
	static Logger* Get();

	Logger() = default;
	Logger(Logger* newInstance);
	Logger(Logger&&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger& operator=(Logger&&) = delete;

	virtual ~Logger() noexcept;

	virtual void Init();
	virtual void Shutdown();

	void LogStandard(LogEntry* category, LogSeverity severity, const char* message, ...);
	void LogFatal(LogEntry* category, LogSeverity severity, const char* message, const u64 time, const char* function, const u32 line, const char* file, ...);
	void LogAssert();

	void SendToOutputBuffer(void* buffer, const u32 count);

protected:
	static bool PrepareLoggingSystem();

	bool bIsInitialized = false;

	static constexpr inline const char* FormatSeverity(LogSeverity Severity) noexcept
	{
		switch (Severity)
		{
		case Log:
			return "Log";
		case Warn:
			return "Warning";
		case Error:
			return "Error";
		case Fatal:
			return "Fatal";
		case Verbose:
			return "Verbose";
		}

		return "???";
	}
private:
	MemoryBlockArena* loggingArena;

	static inline Logger* instance = nullptr;
};

#include "LogMacros.h"