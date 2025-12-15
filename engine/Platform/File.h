/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "FileHelpers.h"

#include <Types/String.h>
#include <Logging/Log.h>
#include <Types/Pointers.h>

//#include "Platform.proxy.h"

LOG_ADDCATEGORY(FileSystem);

class /*PLATFORM_API*/ IFile
{
public:
	IFile() = default;

	virtual ~IFile() noexcept = default;

	virtual void* GetFileHandle() = 0;

	virtual void Write(const String* buffer) const = 0;

	virtual void Read() = 0;

	virtual void Close() = 0;

	String GetName() const { return fileName; };

	virtual void SetName(const String& content) { fileName = content; };
	 
	uint64_t GetSize() const { return size; }

	bool IsValid() const
	{
		return bWasInitSucceded;
	}
		
	/** Returns true, if the file was "created" from the hard disk. */
	bool IsPersistent() const
	{
		return !bWasCreatedByProgramatically;
	}

	virtual void Delete() = 0;

	const char* GetBuffer() const noexcept
	{
		return buffer;
	}

	operator bool() const
	{
		return buffer != nullptr && bWasInitSucceded;
	}

protected:
	bool bWasCreatedByProgramatically = false;

	bool bWasInitSucceded = false;

	uint32_t size = 0;

	char* buffer;

	String fullPath;

	String fileName;
};
