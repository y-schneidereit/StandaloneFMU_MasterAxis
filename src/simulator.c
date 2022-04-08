#include <stdio.h>
#include <Windows.h>

#include "fmi2Functions.h"

// model specific constants
#define GUID "{5beffd4c-c998-a7d1-125b-f08aec717ec5}"
#define RESOURCE_LOCATION "file:///C:/Users/schyan01/github/StandaloneFMU_MasterAxis/MasterAxis" // absolut path to the unziped fmu

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

int main(int argc, char *argv[]) 
{
	HMODULE libraryHandle = LoadLibraryA("C:\\Users\\schyan01\\github\\StandaloneFMU_MasterAxis\\MasterAxis\\binaries\\win64\\MasterAxis.dll");

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
	
	fmi2Component c = InstantiatePtr("instance1", fmi2CoSimulation, GUID, RESOURCE_LOCATION, &callbacks, fmi2False, fmi2False);
	
	if (!c)
	{
		return EXIT_FAILURE;
	}

	fmi2Real Time = 0;
	fmi2Real stepSize = 0.1;
	fmi2Real tolerance = 0.001;
	fmi2Real stopTime = 10;

	fmi2ValueReference Position_ref = 0;
	fmi2Real Position = 0;

	fmi2ValueReference Geschwindigkeit_ref = 1;
	fmi2Real Geschwindigkeit = 0;

	fmi2ValueReference Beschleunigung_ref = 2;
	fmi2Real Beschleunigung = 0;

	fmi2ValueReference Ruck_ref = 3;
	fmi2Real Ruck = 0;

	fmi2ValueReference PT1_ref = 4;
	fmi2Real PT1 = 0;

	fmi2ValueReference PT2_ref = 3;
	fmi2Real PT2 = 0;
	
	// Informs the FMU to setup the experiment. Must be called after fmi2Instantiate and befor fmi2EnterInitializationMode
	CHECK_STATUS(SetupExperimentPtr(c, fmi2False, tolerance, Time, fmi2False, stopTime));
	
	// Informs the FMU to enter Initialization Mode.
	CHECK_STATUS(EnterInitializationModePtr(c));
	
	CHECK_STATUS(SetRealPtr(c, &Position_ref, 1, &Position));

	CHECK_STATUS(ExitInitializationModePtr(c));
	
	printf("time, Position, Geschwindigkeit, Beschleunigung, Ruck, PT1, PT2\n");
	
	for (int nSteps = 0; nSteps <= 20; nSteps++)
	{
		Time = nSteps * stepSize;

		// set an input
		CHECK_STATUS(SetRealPtr(c, &Position_ref, 1, &Position));

		// perform a simulation step
		CHECK_STATUS(DoStepPtr(c, Time, stepSize, fmi2True));	//The computation of a time step is started./*

		// get an output
		CHECK_STATUS(GetRealPtr(c, &Geschwindigkeit_ref, 1, &Geschwindigkeit));
		CHECK_STATUS(GetRealPtr(c, &Beschleunigung_ref, 1, &Beschleunigung));
		CHECK_STATUS(GetRealPtr(c, &Ruck_ref, 1, &Ruck));
		CHECK_STATUS(GetRealPtr(c, &PT1_ref, 1, &PT1));
		CHECK_STATUS(GetRealPtr(c, &PT2_ref, 1, &PT2));

		printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", Time, Geschwindigkeit, Beschleunigung, Ruck, PT1, PT2);
	}
	TerminatePtr(c);

TERMINATE:
	// clean up
	if (status < fmi2Fatal) {
		FreeInstancePtr(c);
	}
	
	FreeLibrary(libraryHandle);

	return status;
}
