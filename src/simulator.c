#include <stdio.h>
#include <Windows.h>

#include "fmi2Functions.h"

// model specific constants
#define GUID "{Altair-MotionView: PickAndPlace.fmu: 1648708627.5544791}"
#define RESOURCE_LOCATION "file:///C:/Users/schyan01/github/StandaloneFMU_PickAndPlace/PickUndPlace/resources" // absolut path to the unziped fmu

// callback functions
static void cb_logMessage(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...) {
	printf("%s\n", message);
}

static void* cb_allocateMemory(size_t nobj, size_t size) {
	return calloc(nobj, size);
}

static void cb_freeMemory(void* obj) {
	free(obj);
}

#define CHECK_STATUS(S) { status = S; if (status != fmi2OK) goto TERMINATE; }

#include <strsafe.h>
void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

int main(int argc, char *argv[]) 
{
	HMODULE libraryHandle = LoadLibraryA("C:\\Users\\schyan01\\github\\StandaloneFMU_PickAndPlace\\PickUndPlace\\binaries\\win64\\msfmu.dll");

	if (!libraryHandle)
	{
		return EXIT_FAILURE;
	}

	fmi2InstantiateTYPE* InstantiatePtr = NULL;
	fmi2FreeInstanceTYPE* FreeInstancePtr = NULL;
	fmi2SetupExperimentTYPE* SetupExperimentPtr = NULL;
	fmi2EnterInitializationModeTYPE* EnterInitializationModePtr = NULL;
	fmi2ExitInitializationModeTYPE* ExitInitializationModePtr = NULL;
	fmi2TerminateTYPE* TerminatePtr = NULL;
	fmi2SetRealTYPE* SetRealPtr = NULL;
	fmi2GetRealTYPE* GetRealPtr = NULL;
	fmi2SetBooleanTYPE* SetBooleanPtr = NULL;
	fmi2GetBooleanTYPE* GetBooleanPtr = NULL;
	fmi2DoStepTYPE* DoStepPtr = NULL;
	fmi2GetTypesPlatformTYPE* GetTypesPlatform = NULL;
	fmi2GetVersionTYPE* GetVersion = NULL;

	InstantiatePtr = (fmi2InstantiateTYPE*) GetProcAddress(libraryHandle, "fmi2Instantiate");
	FreeInstancePtr = (fmi2FreeInstanceTYPE*) GetProcAddress(libraryHandle, "fmi2FreeInstance");
	SetupExperimentPtr =  (fmi2SetupExperimentTYPE*) GetProcAddress(libraryHandle, "fmi2SetupExperiment");
	EnterInitializationModePtr = (fmi2EnterInitializationModeTYPE*) GetProcAddress(libraryHandle, "fmi2EnterInitializationMode");
	ExitInitializationModePtr = (fmi2ExitInitializationModeTYPE*) GetProcAddress(libraryHandle, "fmi2ExitInitializationMode");
	TerminatePtr = (fmi2TerminateTYPE*) GetProcAddress(libraryHandle, "fmi2Terminate");
	SetRealPtr = (fmi2SetRealTYPE*) GetProcAddress(libraryHandle, "fmi2SetReal");
	GetRealPtr = (fmi2GetRealTYPE*) GetProcAddress(libraryHandle, "fmi2GetReal");
	SetBooleanPtr = (fmi2SetBooleanTYPE*) GetProcAddress(libraryHandle, "fmi2SetBoolean");
	GetBooleanPtr = (fmi2GetBooleanTYPE*) GetProcAddress(libraryHandle, "fmi2GetBoolean");
	DoStepPtr = (fmi2DoStepTYPE*) GetProcAddress(libraryHandle, "fmi2DoStep");
	GetTypesPlatform = (fmi2GetTypesPlatformTYPE*) GetProcAddress(libraryHandle, "fmi2GetTypesPlatform");
	GetVersion = (fmi2GetVersionTYPE*) GetProcAddress(libraryHandle, "fmi2GetVersion");

	if (NULL == InstantiatePtr ||
		NULL == FreeInstancePtr ||
		NULL == SetupExperimentPtr ||
		NULL == EnterInitializationModePtr ||
		NULL == ExitInitializationModePtr ||
		NULL == TerminatePtr ||
		NULL == SetRealPtr || 
		NULL == GetRealPtr || 
		NULL == SetBooleanPtr || 
		NULL == GetBooleanPtr || 
		NULL == DoStepPtr ||  
		NULL == GetTypesPlatform || 
		NULL == GetVersion)
	{
		return EXIT_FAILURE;
	}

	fmi2Status status = fmi2OK;

	fmi2CallbackFunctions callbacks = {cb_logMessage, cb_allocateMemory, cb_freeMemory, NULL, NULL};

	fmi2String platform = GetTypesPlatform();
	printf("%s\n", platform);

	fmi2String version = GetVersion();
	printf("%s\n", version);
	
	fmi2Component c = InstantiatePtr("instance1", fmi2CoSimulation, GUID, RESOURCE_LOCATION, &callbacks, fmi2False, fmi2True);
	
	if (!c)
	{
		return EXIT_FAILURE;
	}

	fmi2Real Time = 0;
	fmi2Real stepSize = 1;
	fmi2Real tolerance = 0.001;
	fmi2Real stopTime = 9;
	
	// Informs the FMU to setup the experiment. Must be called after fmi2Instantiate and befor fmi2EnterInitializationMode
	CHECK_STATUS(SetupExperimentPtr(c, fmi2False, tolerance, Time, fmi2False, stopTime));
	
	HRESULT hrReturnVal;
	hrReturnVal = EnterInitializationModePtr(c);
	// Informs the FMU to enter Initialization Mode.
	//CHECK_STATUS(EnterInitializationModePtr(c));
	//ErrorExit(TEXT("EnterInitializationModePtr"));
	fmi2ValueReference z_command_ref = 0;
	fmi2Real z_command = 0;

	fmi2ValueReference z_override_ref = 1;
	fmi2Real z_override = 0;

	fmi2ValueReference y_command_ref = 2;
	fmi2Real y_command = 0;
	
	fmi2ValueReference y_override_ref = 3;
	fmi2Real y_override = 0;

	CHECK_STATUS(SetRealPtr(c, &z_command_ref, 1, &z_command));
	CHECK_STATUS(SetRealPtr(c, &z_override_ref, 1, &z_override));
	CHECK_STATUS(SetRealPtr(c, &y_command_ref, 1, &y_command));
	CHECK_STATUS(SetRealPtr(c, &y_override_ref, 1, &y_override));

	//CHECK_STATUS(ExitInitializationModePtr(c));
	/*
	printf("time, u, T\n");

	//for (int nSteps = 0; nSteps <= 20; nSteps++)
	//{
		//Time = nSteps * stepSize;

		// set an input
		//CHECK_STATUS(fmi2SetBoolean(c, &u_ref, 1, &u));

		// perform a simulation step
		*///CHECK_STATUS(DoStepPtr(c, Time, stepSize, fmi2True));	//The computation of a time step is started./*
		
		// get an output
		//CHECK_STATUS(fmi2GetBoolean(c, &T_ref, 1, &T));
		
		//printf("%.2f, %d, %d\n", Time, u, T);
	//}
	//TerminatePtr(c);*/

TERMINATE:
	// clean up
	if (status < fmi2Fatal) {
		FreeInstancePtr(c);
	}
	
	FreeLibrary(libraryHandle);

	return status;
}
