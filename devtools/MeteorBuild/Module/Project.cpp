/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Project.h"
#include <Core/Utils.h>
#include <Platform/FileManager.h>
#include <Platform/File.h>

#include "Module.h"
#include <Core/Application.h>

LOG_ADDCATEGORY(ProjectScriptParser);

bool Project::Finalize(String* output)
{
	if (BuildSystemApplication* app = GetApplication<BuildSystemApplication>())
	{
		const Array<Module>* modules = app->GetBuildSystem().GetModules();

		String project;
		for (auto& mdl : *modules)
		{
			String actualProject = String::Format("\t\t<Project Path = \"%ls\" Id=\"%ls\"/>\n", *mdl.generatedProjectFile, *mdl.identification);
			project = String::Format("%ls%ls", *project, *actualProject);
		}

		*output = String::Format("<!-- This file is generated with MeteorBuild(R) -->\n"
			"<Solution>\n"
			"\t<Configurations>\n"
			"\t\t<BuildType Name=\"Debug\" />\n"
			"\t\t<BuildType Name=\"Shipping\" />\n"
			"\t\t<Platform Name = \"x64\" />\n"
			"\t</Configurations>\n"
			"\t<Folder Name = \"/Modules/\">\n"
			"%ls"
			"\t</Folder>\n"
			"</Solution>", 
			*project);

		return true;
	}

	return false;
}
