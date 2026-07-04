/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
//#include <Types/Delegate.h>
#include <HAL/DataTypes.h>

#ifdef MR_DEBUG
constexpr inline const bool bIsRunningDebugMode = true;
#else
constexpr inline const bool bIsRunningDebugMode = false;
#endif // MR_DEBUG

class String;
struct LogDescriptor;
enum LogFormatting;
struct LogAssertion;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

//CREATE_DELEGATE(LoggerInitialize);

struct LogEntry {};

// Logger definition is split exclusively into seperate .cpp OS specific files
//

// * awd
class CORE_API Logger
{
	static constexpr const u32 MAX_LOG_ENTRIES = 1024;

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

	virtual u32 FormatLogMessage(char* buffer, LogFormatting format, LogDescriptor* descriptor);
	virtual void TransmitAssertion(const LogAssertion* Info);
	virtual void SendToOutputBuffer(char* buffer, const u32 count);
	virtual void HandleFatal(LogDescriptor* Descriptor);


protected:
	bool bIsInitialized = false;

private:
	static inline Logger* instance = nullptr;
};

#include "LogMacros.h"

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

