/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Parser.h"
#include <Module/Module.h>
#include <Module/Project.h>

#include <Platform/FileManager.h>
#include <Platform/File.h>
#include <Types/Pointers.h>

#include <PathCch.h>
#include <rpc.h>
#include <Objbase.h>

LOG_ADDCATEGORY(Parser);

static bool AddVerbDetail(Module* moduleToWrite, const String& verb, const String& value)
{
	if (verb == "IncludePath" || verb == "IncludePaths") { moduleToWrite->includePaths.Add(value); return true; }
	if (verb == "Dependencies") { moduleToWrite->requires.Add(value); return true; }

	MR_LOG(LogParser, Error, "Unknown verb: %ls", *verb);
	return false;
}

static bool AddVerbDetail(Project* projectToWrite, const String& verb, const String& value)
{
	if (verb == "GlobalDefine" || verb == "GlobalDefines") { projectToWrite->globalDefines.Add(value); return true; }
	if (verb == "Executable") { projectToWrite->launcher = value; return true; }

	MR_LOG(LogParser, Error, "Unknown verb: %ls", *verb);
	return false;
}

Module* Parser::ParseModuleScript(String* moduleName)
{
	bool bFailed = false;

	ScopedPtr<IFile> module = FileManager::CreateFileOperation(moduleName, FileAccessMode::OPENMODE_READ, FileShareMode::SHAREMODE_READ, OVERRIDERULE_OPEN_ONLY_IF_EXISTS);
	if (module != nullptr)
	{
		module->Read();

		const char* buffer = module->GetBuffer();
		if (GetWord(buffer, false) == "Module")
		{
			PathCchRemoveFileSpec(moduleName->Data(), moduleName->Length());
			moduleName->Refresh();

			MR_LOG(LogParser, Verbose, "Opening %ls as ModuleScript!", *module->GetName());
			Module* newModule = new Module();
			if (!newModule)
				return nullptr;

			SkipWord(buffer);  // Skip "Module"

			newModule->moduleName = GetWord(buffer, true);

			if (GetCharacterType(buffer) == Colon)
			{
				SkipCharacterType(buffer, Colon);
				newModule->dependsOn = GetWord(buffer, true);

				if (GetCharacterType(buffer) == OpenBrace)
				{
					SkipCharacterType(buffer, OpenBrace);

					while (*buffer != '\0')
					{
						while (GetCharacterType(buffer) != ClosedBrace
							&& GetCharacterType(buffer) != None)
						{
							const String flagWord = GetWord(buffer, true);
							if (flagWord && GetCharacterType(buffer) == Colon)
							{
								SkipCharacterType(buffer, Colon);

								if (GetCharacterType(buffer) == OpenBrace)
								{
									SkipCharacterType(buffer, OpenBrace);
									while (GetCharacterType(buffer) != ClosedBrace)
									{
										const String value = GetWord(buffer, true);
										if (value)
										{
											if (AddVerbDetail(newModule, flagWord, value))
												MR_LOG(LogParser, Verbose, "Added %ls property to %ls", *value, *flagWord);
										}

										if (GetCharacterType(buffer) == Comma)
											SkipCharacterType(buffer, Comma);
									}
								}
							}
							else if (GetCharacterType(buffer) != Colon)
							{
								MR_LOG(LogParser, Fatal, "Missing colon after word %ls!", flagWord.Chr());
							}
							else
							{
								MR_LOG(LogParser, Fatal, "Unknown error!");
							}
						}

						SkipCharacterType(buffer, ClosedBrace);
					}

					if (*buffer == '\0')
					{
						GUID id;
						if (SUCCEEDED(CoCreateGuid(&id)))
						{
							wchar_t buffer[64];
							if (!StringFromGUID2(id, buffer, 64))
							{
								module->Close();
								return nullptr;
							}

							newModule->identification = buffer;
							MR_LOG(LogParser, Verbose, "Successfully generated GUID, for module %ls!", *newModule->moduleName);
						}
					}

					module->Close();
					newModule->SetIsParsed(true);

					newModule->defineName = *newModule->moduleName;
					for (wchar_t* ptr = newModule->defineName.Data(); *ptr; ptr++)
						*ptr = towupper(*ptr);

					return newModule;
				}
				else
				{
					bFailed = true;
				}
			}
			else
			{
				bFailed = true;
			}
		}
		else
		{
			bFailed = true;
		}

		return nullptr;
	}

	return nullptr;
}

Project* Parser::ParseProjectScript(String* projectPath)
{
	ScopedPtr<IFile> module = FileManager::CreateFileOperation(projectPath, FileAccessMode::OPENMODE_READ, FileShareMode::SHAREMODE_READ, OVERRIDERULE_OPEN_ONLY_IF_EXISTS);
	if (module != nullptr)
	{
		PathCchRemoveFileSpec(projectPath->Data(), projectPath->Length());
		projectPath->Refresh();

		module->Read();

		const char* buffer = module->GetBuffer();

		bool bHasBeenParsedOneWordAtLeast = false;
		if (GetWord(buffer, false) == "Project")
		{
			SkipWord(buffer);  // Skip "Project"
			Project* newProject = new Project();
			if (!newProject)
				return nullptr;

			MR_LOG(LogParser, Verbose, "Opening %ls as ProjectScript!", *module->GetName());

			newProject->projectName = GetWord(buffer, true);

			if (GetCharacterType(buffer) == OpenBrace)
			{
				SkipCharacterType(buffer, OpenBrace);

				while (*buffer != '\0')
				{
					while (GetCharacterType(buffer) != ClosedBrace
						&& GetCharacterType(buffer) != None)
					{
						const String flagWord = GetWord(buffer, true);
						if (flagWord && GetCharacterType(buffer) == Colon)
						{
							SkipCharacterType(buffer, Colon);

							if (GetCharacterType(buffer) == OpenBrace)
							{
								SkipCharacterType(buffer, OpenBrace);
								while (GetCharacterType(buffer) != ClosedBrace)
								{
									const String value = GetWord(buffer, true);
									if (value)
									{
										if (!bHasBeenParsedOneWordAtLeast) bHasBeenParsedOneWordAtLeast = true;

										AddVerbDetail(newProject, flagWord, value);
										MR_LOG(LogParser, Verbose, "Adding %ls property to %ls", *value, *flagWord);
									}

									if (GetCharacterType(buffer) == Comma)
										SkipCharacterType(buffer, Comma);
								}
							}
						}
						else if (GetCharacterType(buffer) != Colon)
						{
							MR_LOG(LogParser, Fatal, "Missing colon after word! %ls", flagWord.Chr());
						}
						else
						{
							MR_LOG(LogParser, Fatal, "Unknown error!");
						}
					}

					SkipCharacterType(buffer, ClosedBrace);
				}
			}

			module->Close();
			return newProject;
		}

		module->Close();
		return nullptr;
	}

	return nullptr;
}

ECharacterType Parser::GetCharacterType(const char*& str)
{
	if (!str) return None;

	while (isspace(*str))
		str++;

	const char* constStr = str;
	if (isalpha(*constStr))
	{
		while (!isspace(*constStr))
			constStr++;

		return Word;
	}

	if (*constStr == '{')
	{
		return OpenBrace;
	}
	else if (*constStr == '}')
	{
		return ClosedBrace;
	}
	else if (*constStr == ',')
	{
		return Comma;
	}
	else if (*constStr == ';')
	{
		return SemiColon;
	}
	else if (*constStr == ':')
	{
		return Colon;
	}

	return None;
}

void Parser::SkipCharacterType(const char*& str, ECharacterType type)
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

				return;
			}
			break;
		}
		default:
			if (GetCharacterType(str) == type)
				str++;

			break;
		}
	}
}

String Parser::GetWord(const char*& in, bool bStep)
{
	while (isspace(*in))
		in++;

	const char* begin = in;
	const char* end = nullptr;
	uint32_t chars = 0;

	if (*begin == '"')
	{
		begin++; // "
		end = begin;

		while (*end && /*isalpha(*end)*/ *end != '"')
		{
			chars++;
			end++;
		}

		end++;
	}
	else if (*begin == '$')
	{
		end = begin;

		end++;
		if (isspace(*end))
		{
			while (*end && !isspace(*end)) end++;
			chars = (uint32_t)(end - begin);
		}
		else
		{

		}
	}
	else
	{
		end = in;
		while (*end && !isspace(*end) && !ispunct(*end)) end++;

		chars = (uint32_t)(end - begin);
	}

	String returned(begin, chars);
	if (bStep) in = end;

	return returned;
}

void Parser::SkipWord(const char*& in)
{
	if (in)
	{
		while (*in && !isspace(*in))
			in++;
	}
}

bool Parser::IsSpace(const char* buffer)
{
	return *buffer == ' ' ? true : false;
}