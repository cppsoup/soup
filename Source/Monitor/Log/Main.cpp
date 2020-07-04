import Monitor.Shared;
import Detours;
import Opal;

using namespace Opal;

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#pragma warning(push)
#if _MSC_VER > 1400
#pragma warning(disable:6102 6103)
#endif
#include <strsafe.h>
#pragma warning(pop)

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "DetourCallbackLogger.h"

DWORD main(int argc, char **argv)
{
	if (argc <= 1)
	{
		std::cout << "Missing executable parameter." << std::endl;
		return 1;
	}

	System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());
	System::IProcessManager::Register(std::make_shared<System::WindowsProcessManager>());

	auto application = Path(argv[1]);
	std::vector<std::string> args;
	for (int i = 2; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	auto file = std::fstream("Access.txt", std::fstream::out);

	auto callback = Monitor::DetourCallbackLogger(file);
	auto process = Monitor::DetouredProcess(callback); 
	auto result = process.RunProcess(application, args);

	return result;
}