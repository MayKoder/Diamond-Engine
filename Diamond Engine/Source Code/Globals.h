#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }


enum class LogType
{
	L_NORMAL, L_WARNING, L_ERROR
};

#define LOG(_type, format, ...) log(__FILE__, __LINE__, _type, format, __VA_ARGS__);

void log(const char file[], int line, LogType _type, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI
#define PI 3.14159265358979323846

#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)
//static float LerpNum(float a, float b, float t) 
//{
//	return a + (b - a) * t;
//}

typedef unsigned int uint;
typedef unsigned __int64 uint64;

//enum update_status
//{
//	UPDATE_CONTINUE = 1,
//	UPDATE_STOP,
//	UPDATE_ERROR
//};

static void CMDCompileCS() 
{
#pragma region ShellExecute
	SHELLEXECUTEINFO execInfo = { 0 };
	execInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	execInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	execInfo.hwnd = NULL;
	execInfo.lpVerb = NULL;
	execInfo.lpFile = "cmd";
	//ShExecInfo.lpParameters = "/K dotnet build Assembly-CSharp.sln --configuration Release";
	execInfo.lpParameters = "/C cd mono-runtime/MSBuild & msbuild ../../Assembly-CSharp.sln /p:Configuration=Release"; //Should include msbuild to the editor folder to make sure this will work? /p:Configuration=Release
	execInfo.lpDirectory = NULL;
	execInfo.nShow = SW_SHOW; /*SW_SHOW  SW_HIDE*/
	execInfo.hInstApp = NULL;

	ShellExecuteEx(&execInfo);

	if (execInfo.hProcess != NULL) {
		WaitForSingleObject(execInfo.hProcess, INFINITE);
		CloseHandle(execInfo.hProcess);
	}

#pragma endregion
}

// Configuration ----------- ASK: Should we load this from an XML?
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define MIN_WIDTH 1024
#define MIN_HEIGHT 768

#define SCREEN_SIZE 1
#define TITLE "Diamond Engine v"