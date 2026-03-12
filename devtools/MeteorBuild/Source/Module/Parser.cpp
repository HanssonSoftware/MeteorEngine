/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Parser.h"
#include <Module/Module.h>
#include <Module/Project.h>

#include <Types/Pointers.h>

#include <Platform/Winapi.h>
#include <PathCch.h>
//#include <rpc.h>
#include <Objbase.h>

LOG_ADDCATEGORY(Parser);

static bool AddVerbDetail(Module* moduleToWrite, const String& verb, const String& value)
{
	//if (verb == "IncludePath" || verb == "IncludePaths") { moduleToWrite->includePaths.Add(value); return true; }
	//if (verb == "Dependencies") { moduleToWrite->requires.Add(value); return true; }

	//MR_LOG(LogParser, Error, "Unknown verb: %ls", *verb);
	return false;
}

static bool AddVerbDetail(Project* projectToWrite, const String& verb, const String& value)
{
	//if (verb == "GlobalDefine" || verb == "GlobalDefines") { projectToWrite->globalDefines.Add(value); return true; }
	//if (verb == "Executable") { projectToWrite->launcher = value; return true; }

	//MR_LOG(LogParser, Error, "Unknown verb: %ls", *verb);
	return false;
}


	//ScopedPtr<IFile> module = FileManager::CreateFileOperation(moduleName, FileAccessMode::OPENMODE_READ, FileShareMode::SHAREMODE_READ, OVERRIDERULE_OPEN_ONLY_IF_EXISTS);
	//if (module != nullptr)
	//{
	//	module->Read();

	//	const wchar_t* buffer = module->GetBuffer();
	//	if (GetWord(buffer, false) == "Module")
	//	{
	//		PathCchRemoveFileSpec(moduleName->Data(), moduleName->Length());
	//		moduleName->Refresh();

	//		MR_LOG(LogParser, Log, "Opening %ls as ModuleScript!", *module->GetName());
	//		Module* newModule = new Module();
	//		if (!newModule)
	//			return nullptr;

	//		SkipWord(buffer);  // Skip "Module"

	//		newModule->moduleName = GetWord(buffer, true);

	//		if (Getwchar_tacterType(buffer) == Colon)
	//		{
	//			Skipwchar_tacterType(buffer, Colon);
	//			newModule->dependsOn = GetWord(buffer, true);

	//			if (Getwchar_tacterType(buffer) == OpenBrace)
	//			{
	//				Skipwchar_tacterType(buffer, OpenBrace);

	//				while (*buffer != '\0')
	//				{
	//					while (Getwchar_tacterType(buffer) != ClosedBrace
	//						&& Getwchar_tacterType(buffer) != None)
	//					{
	//						const String flagWord = GetWord(buffer, true);
	//						if (flagWord && Getwchar_tacterType(buffer) == Colon)
	//						{
	//							Skipwchar_tacterType(buffer, Colon);

	//							if (Getwchar_tacterType(buffer) == OpenBrace)
	//							{
	//								Skipwchar_tacterType(buffer, OpenBrace);
	//								while (Getwchar_tacterType(buffer) != ClosedBrace)
	//								{
	//									const String value = GetWord(buffer, true);
	//									if (value)
	//									{
	//										if (AddVerbDetail(newModule, flagWord, value))
	//											MR_LOG(LogParser, Log, "Added %ls property to %ls", *value, *flagWord);
	//									}

	//									if (Getwchar_tacterType(buffer) == Comma)
	//										Skipwchar_tacterType(buffer, Comma);
	//								}
	//							}
	//						}
	//						else if (Getwchar_tacterType(buffer) != Colon)
	//						{
	//							MR_LOG(LogParser, Fatal, "Missing colon after word %ls!", flagWord.Chr());
	//						}
	//						else
	//						{
	//							MR_LOG(LogParser, Fatal, "Unknown error!");
	//						}
	//					}

	//					Skipwchar_tacterType(buffer, ClosedBrace);
	//				}

	//				if (*buffer == '\0')
	//				{
	//					GUID id;
	//					if (SUCCEEDED(CoCreateGuid(&id)))
	//					{
	//						wwchar_t_t buffer[64];
	//						if (!StringFromGUID2(id, buffer, 64))
	//						{
	//							module->Close();
	//							return nullptr;
	//						}

	//						newModule->identification = buffer;
	//						MR_LOG(LogParser, Log, "Successfully generated GUID, for module %ls!", *newModule->moduleName);
	//					}
	//				}

	//				module->Close();
	//				newModule->SetIsParsed(true);

	//				newModule->defineName = *newModule->moduleName;
	//				for (wwchar_t_t* ptr = newModule->defineName.Data(); *ptr; ptr++)
	//					*ptr = towupper(*ptr);

	//				return newModule;
	//			}
	//			else
	//			{
	//				bFailed = true;
	//			}
	//		}
	//		else
	//		{
	//			bFailed = true;
	//		}
	//	}
	//	else
	//	{
	//		bFailed = true;
	//	}

	//	return nullptr;
	//}



	//ScopedPtr<IFile> module = FileManager::CreateFileOperation(projectPath, FileAccessMode::OPENMODE_READ, FileShareMode::SHAREMODE_READ, OVERRIDERULE_OPEN_ONLY_IF_EXISTS);
	//if (module != nullptr)
	//{
	//	PathCchRemoveFileSpec(projectPath->Data(), projectPath->Length());
	//	projectPath->Refresh();

	//	module->Read();

	//	const wchar_t* buffer = module->GetBuffer();

	//	bool bHasBeenParsedOneWordAtLeast = false;
	//	if (GetWord(buffer, false) == "Project")
	//	{
	//		SkipWord(buffer);  // Skip "Project"
	//		Project* newProject = new Project();
	//		if (!newProject)
	//			return nullptr;

	//		MR_LOG(LogParser, Log, "Opening %ls as ProjectScript!", *module->GetName());

	//		newProject->projectName = GetWord(buffer, true);

	//		if (Getwchar_tacterType(buffer) == OpenBrace)
	//		{
	//			Skipwchar_tacterType(buffer, OpenBrace);

	//			while (*buffer != '\0')
	//			{
	//				while (Getwchar_tacterType(buffer) != ClosedBrace
	//					&& Getwchar_tacterType(buffer) != None)
	//				{
	//					const String flagWord = GetWord(buffer, true);
	//					if (flagWord && Getwchar_tacterType(buffer) == Colon)
	//					{
	//						Skipwchar_tacterType(buffer, Colon);

	//						if (Getwchar_tacterType(buffer) == OpenBrace)
	//						{
	//							Skipwchar_tacterType(buffer, OpenBrace);
	//							while (Getwchar_tacterType(buffer) != ClosedBrace)
	//							{
	//								const String value = GetWord(buffer, true);
	//								if (value)
	//								{
	//									if (!bHasBeenParsedOneWordAtLeast) bHasBeenParsedOneWordAtLeast = true;

	//									AddVerbDetail(newProject, flagWord, value);
	//									MR_LOG(LogParser, Log, "Adding %ls property to %ls", *value, *flagWord);
	//								}

	//								if (Getwchar_tacterType(buffer) == Comma)
	//									Skipwchar_tacterType(buffer, Comma);
	//							}
	//						}
	//					}
	//					else if (Getwchar_tacterType(buffer) != Colon)
	//					{
	//						MR_LOG(LogParser, Fatal, "Missing colon after word! %ls", flagWord.Chr());
	//					}
	//					else
	//					{
	//						MR_LOG(LogParser, Fatal, "Unknown error!");
	//					}
	//				}

	//				Skipwchar_tacterType(buffer, ClosedBrace);
	//			}
	//		}

	//		module->Close();
	//		return newProject;
	//	}

	//	module->Close();
	//	return nullptr;
	//}

Ewchar_tacterType GetType(wchar_t*& str)
{
	if (!str) return None;

	while (isspace(*str))
		str++;

	const wchar_t* constStr = str;
	if (iswalpha(*constStr))
	{
		while (!iswspace(*constStr))
			constStr++;

		return Word;
	}

	if (*constStr == TEXT('{'))
	{
		return OpenBrace;
	}
	else if (*constStr == TEXT('}'))
	{
		return ClosedBrace;
	}
	else if (*constStr == TEXT(','))
	{
		return Comma;
	}
	else if (*constStr == ';')
	{
		return SemiColon;
	}
	else if (*constStr == TEXT(':'))
	{
		return Colon;
	}
	else if (*constStr == TEXT('\0'))
	{
		return EndOfFile;
	}

	return None;
}

bool SkipType(wchar_t*& str, Ewchar_tacterType type)
{
	if (str)
	{
		switch (type)
		{
		case Word:
		{
			if (isalpha(*str))
			{
				while (!isspace(*str))
					str++;

				return true;
			}
			break;
		}
		default:
			if (GetType(str) == type)
			{
				str++;
				return true;
			}
			break;
		}
	}

	return false;
}

String GetWord(wchar_t*& in, bool bStep)
{
	while (isspace(*in))
		in++;

	wchar_t* begin = in;
	wchar_t* end = nullptr;
	uint32_t wchar_ts = 0;

	//if (*begin == '"')
	//{
	//	begin++; // "
	//	end = begin;

	//	while (*end && /*isalpha(*end)*/ *end != '"')
	//	{
	//		wchar_ts++;
	//		end++;
	//	}

	//	end++;
	//}
	if (*begin == '$')
	{
		end = begin;

		end++;
		if (isspace(*end))
		{
			while (*end && !isspace(*end)) end++;
			wchar_ts = (uint32_t)(end - begin);
		}
		else
		{

		}
	}
	else
	{
		end = in;
		while (*end && !isspace(*end) && !ispunct(*end)) end++;

		wchar_ts = (uint32_t)(end - begin);
	}

	//String returned(begin, wchar_ts);
	if (bStep) in = end;

	return "returned";
}

String GetQuotedWord(wchar_t*& in, bool bStep)
{
	while (isspace(*in))
		in++;

	in++; // "

	wchar_t* begin = in;
	wchar_t* end = nullptr;
	uint32_t wchar_ts = 0;

	if (/**begin == '$'*/false)
	{
 	//	end = begin;
		//end++;

		//while (*end != ')' && isalpha(*end)) end++;
		//wchar_ts = (uint32_t)(end - begin);
	}
	else
	{
		end = begin;

		while (*end != '"') end++;
		wchar_ts = (uint32_t)(end - begin);
	}

	end++; // "

	//String returned(begin, wchar_ts);
	if (bStep) in = end;

	return "returned";
}

bool SkipWord(wchar_t*& in, int& line, int& wchar_tacter)
{
	if (in)
	{
		while (*in && !isspace(*in))
		{
			in++;
		}
		
		return true;
	}

	return false;
}
