/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "DataTypes.h"
#include <Types/StringView.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

//* Accessing mode: During execution, if you access the current from somewhere it behaves as the selected option.
enum CORE_API Access : u8
{
    None = 0,
    Read = 1 << 0,
    Write = 1 << 1
};

enum CORE_API Share : u8
{
    Nothing = 0,
    Reading = 1 << 0,
    Writing = 1 << 1,
    Delete = 1 << 2
};

enum CORE_API OpenAs : u8
{
    AlwaysCreate,
    CreateIfNotExist,
    JustOpen,
    OpenIfExist,
    Truncate
};

enum class CORE_API Origin : u8
{
    Begin,
    Current,
    End
};

class CORE_API File
{
public:

	File() = default;
	virtual ~File() noexcept = default;

    /** Creates a file or opens if exist, define accessing mode with 'access', define sharing mode with 'share'.*/
    bool Open(const StringView& name, const OpenAs openAs, const Access access, const Share share);

    void Read(char* returnedBuffer);

    u64 GetFileSize() const { return fileSize; };

protected:
	union
	{
		void* windowsAPIHandle = nullptr;

        struct
        {
            u32 linuxHandle;
            u32 DUMMY_PADDING;
        };
	};

    u64 fileSize = 0;
};