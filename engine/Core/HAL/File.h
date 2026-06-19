/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "DataTypes.h"

class File
{
public:
    /** Accessing mode: During execution, if you access the current from somewhere it behaves as the selected option. */
    enum class Access : u8
    {
        Read,
        Write,
        ReadWrite
    };

    enum class Share : u8
    {
        None,
        Read,
        Write,
        All
    };

    enum class Origin : u8
    {
        Begin,
        Current,
        End
    };

	File() = default;
	virtual ~File() noexcept = default;

    /** Creates a file or opens if exist, define accessing mode with 'access', define sharing mode with 'share'.*/
	bool Open(const char* name, const Access access, const Share share);

    u32 GetFileSize() const { return fileSize; };

protected:
	union
	{
		void* windowsAPIHandle = nullptr;

        struct
        {
            u32 linuxHandle;
            const u32 DUMMY_PADDING;
        };
	};

    u32 fileSize = 0;
};