#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include<ShObjIdl_core.h>


#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#ifdef _DEBUG
	//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

//ASK IMPORTANT: Is vector iteration faster with iterators or via index?
int main(int argc, char** argv)
{
	LOG(LogType::L_NORMAL, "Starting game '%s'...", TITLE);

	//This compiles the solution, this is kinda cool, talk about this with the teacher
	//UINT ret = WinExec("dotnet build I:/Git_Projecte/Diamond-Engine/Diamond Engine/Project Folder/CSTest/ConsoleApp1.sln --configuration Debug", SW_SHOW);
	//system("msbuild C:/Users/Mique/Desktop/wetransfer-c6d5c3/CSTest/ConsoleApp1/ConsoleApp1.sln /p:Configuration=Debug");

//This works BUT i dont like to hardcode the cmd.exe path, i think this will cause some issues with different OS or system paths
#pragma region CreateProcess
	//STARTUPINFO si;
	//SecureZeroMemory(&si, sizeof(STARTUPINFO));

	//si.cb = sizeof(STARTUPINFO);


	//PROCESS_INFORMATION pi;

	//BOOL result = CreateProcess(
	//	"c:\\windows\\system32\\cmd.exe",
	//	"/c dotnet build CSTest/ConsoleApp1.sln --configuration Debug",
	//	NULL,
	//	NULL,
	//	FALSE,
	//	0,
	//	NULL,
	//	NULL,
	//	&si,
	//	&pi);

	//if (result)
	//{
	//	WaitForSingleObject(pi.hProcess, INFINITE);
	//	printf("Process with ID: %i has exited.\n", GetProcessId(pi.hProcess));
	//	CloseHandle(pi.hProcess);
	//}
#pragma endregion

	
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Application* App = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG(LogType::L_NORMAL, "-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG(LogType::L_NORMAL, "-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG(LogType::L_ERROR, "Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG(LogType::L_NORMAL, "-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG(LogType::L_ERROR, "Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP || App->GetExitState())
				state = MAIN_FINISH;
		}
		break;

		case MAIN_FINISH:

			LOG(LogType::L_NORMAL, "-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG(LogType::L_ERROR, "Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	EngineExternal = nullptr;
	RELEASE(App);

#ifdef _DEBUG
	int leaks = MAX(0, m_getMemoryStatistics().totalAllocUnitCount - 23);
	LOG(LogType::L_NORMAL, "With %d memory leaks!\n", (leaks > 0) ? leaks : 0);
#endif

	LOG(LogType::L_NORMAL, "Exiting game '%s'...\n", TITLE);
	return main_return;
}