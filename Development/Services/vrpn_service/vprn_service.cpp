////////////////////////////////////////////////////////////////////// 
// NT Service Stub Code (For XYROOT )
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <winsvc.h>
#include <process.h>
#include <FCNTL.H>
#include <io.h>
#include <vrpn/vrpn_connection.h>
#include "flock.h"


const int nBufferSize = 500;
char pServiceName[nBufferSize+1];
char pExeFile[nBufferSize+1];
char pInitFile[nBufferSize+1];
char pLogFile[nBufferSize+1];
bool running = false;

SERVICE_STATUS          serviceStatus; 
SERVICE_STATUS_HANDLE   hServiceStatusHandle; 

VOID WINAPI VRPNServiceMain( DWORD dwArgc, LPTSTR *lpszArgv );
VOID WINAPI VRPNServiceHandler( DWORD fdwControl );

CRITICAL_SECTION myCS;

void WriteLog(char* pMsg)
{
	// write error or other information into log file
	::EnterCriticalSection(&myCS);
	try
	{
		SYSTEMTIME oT;
		::GetLocalTime(&oT);
		FILE* pLog = fopen(pLogFile,"a");
		fprintf(pLog,"%02d/%02d/%04d, %02d:%02d:%02d\n    %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,pMsg); 
		fprintf(stderr,"%02d/%02d/%04d, %02d:%02d:%02d\n    %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,pMsg); 
		fclose(pLog);
	} catch(...) {}
	::LeaveCriticalSection(&myCS);
}
void StartLog()
{
	// write error or other information into log file
	::EnterCriticalSection(&myCS);
	try
	{
		SYSTEMTIME oT;
		::GetLocalTime(&oT);
		FILE* pLog = fopen(pLogFile,"w");
		fprintf(pLog,"%02d/%02d/%04d, %02d:%02d:%02d\n    %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,"Starting log..."); 
		fclose(pLog);
	} catch(...) {}
	::LeaveCriticalSection(&myCS);
}
////////////////////////////////////////////////////////////////////// 
//
// Configuration Data and Tables
//

SERVICE_TABLE_ENTRY   DispatchTable[] = 
{ 
	{pServiceName, VRPNServiceMain}, 
	{NULL, NULL}
};


BOOL KillService(char* pName) 
{ 
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d", nError);
		WriteLog(pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "OpenService failed, error code = %d", nError);
			WriteLog(pTemp);
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[121];
				sprintf(pTemp, "ControlService failed, error code = %d", nError);
				WriteLog(pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL RunService(char* pName, int nArg, char** pArg) 
{ 
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d", nError);
		WriteLog(pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "OpenService failed, error code = %d", nError);
			WriteLog(pTemp);
		}
		else
		{
			// call StartService to run the service
			if(StartService(schService,nArg,(const char**)pArg))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[121];
				sprintf(pTemp, "StartService failed, error code = %d", nError);
				WriteLog(pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}


DWORD WINAPI consoleLogger(void* param)
{
	HANDLE hReadPipe = (HANDLE)param;
	DWORD dwBytesRead = 0;
	char cNextChar[1024];
	while(running)
	{
		if(ReadFile( hReadPipe, cNextChar, 1024, &dwBytesRead, NULL ))
		{
			cNextChar[dwBytesRead] = 0;
			WriteLog(cNextChar);
		}
	}
	return 0;
}



void serviceMain()
{
	WSADATA wsaData; 
	WSAStartup(MAKEWORD(1,1), &wsaData);
	vrpn_Connection* connection = vrpn_create_server_connection();
	Flock* flock = new Flock("test_tracker", connection);
	
	running = true;
	while(running)
	{
		flock->loop();
		connection->mainloop();
		vrpn_SleepMsecs(1);
	}
	WriteLog("Stopping service...");
	delete flock;
	delete connection;
	WriteLog("Stopped service...");
}

////////////////////////////////////////////////////////////////////// 
//
// This routine gets used to start your service
//
VOID WINAPI VRPNServiceMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
	DWORD   status = 0; 
    DWORD   specificError = 0xfffffff; 
 
    serviceStatus.dwServiceType        = SERVICE_WIN32; 
    serviceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    serviceStatus.dwWin32ExitCode      = 0; 
    serviceStatus.dwServiceSpecificExitCode = 0; 
    serviceStatus.dwCheckPoint         = 0; 
    serviceStatus.dwWaitHint           = 0; 
 
    hServiceStatusHandle = RegisterServiceCtrlHandler(pServiceName, VRPNServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "RegisterServiceCtrlHandler failed, error code = %d", nError);
		WriteLog(pTemp);
        return; 
    } 
 
    // Initialization complete - report running status 
    serviceStatus.dwCurrentState       = SERVICE_RUNNING; 
    serviceStatus.dwCheckPoint         = 0; 
    serviceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &serviceStatus)) 
    { 
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "SetServiceStatus failed, error code = %d", nError);
		WriteLog(pTemp);
    } 
	StartLog();

	/*HANDLE m_hReadPipe;
	HANDLE m_hWritePipe;
	CreatePipe( &m_hReadPipe, &m_hWritePipe, NULL, 0 );
	stderr->_file = _open_osfhandle( (intptr_t)m_hWritePipe, _O_TEXT);
	setvbuf( stderr, NULL, _IONBF, 0 );

	DWORD dwThreadId;
	HANDLE threadHandle = CreateThread(NULL, 0, consoleLogger, m_hReadPipe, 0, &dwThreadId);*/

	serviceMain();
//	TerminateThread(threadHandle,0);

}

////////////////////////////////////////////////////////////////////// 
//
// This routine responds to events concerning your service, like start/stop
//
VOID WINAPI VRPNServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			serviceStatus.dwWin32ExitCode = 0; 
			serviceStatus.dwCurrentState  = SERVICE_STOPPED; 
			serviceStatus.dwCheckPoint    = 0; 
			serviceStatus.dwWaitHint      = 0;
			// terminate all processes started by this service before shutdown
			{
				WriteLog("Service is being asked to stop...");
				running = false;
				Sleep(1000); // even het andere process de tijd geven om te stoppen
				if (!SetServiceStatus(hServiceStatusHandle, &serviceStatus))
				{ 
					long nError = GetLastError();
					char pTemp[121];
					sprintf(pTemp, "SetServiceStatus failed, error code = %d", nError);
					WriteLog(pTemp);
				}
			}
			return; 
		case SERVICE_CONTROL_PAUSE:
			serviceStatus.dwCurrentState = SERVICE_PAUSED; 
			break;
		case SERVICE_CONTROL_CONTINUE:
			serviceStatus.dwCurrentState = SERVICE_RUNNING; 
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		default: 
			break;
	};
    if (!SetServiceStatus(hServiceStatusHandle,  &serviceStatus)) 
	{ 
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "SetServiceStatus failed, error code = %d", nError);
		WriteLog(pTemp);
    } 
}


////////////////////////////////////////////////////////////////////// 
//
// Uninstall
//
VOID UnInstall(char* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d", nError);
		WriteLog(pTemp);
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "OpenService failed, error code = %d", nError);
			WriteLog(pTemp);
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				char pTemp[121];
				sprintf(pTemp, "Failed to delete service %s", pName);
				WriteLog(pTemp);
			}
			else 
			{
				char pTemp[121];
				sprintf(pTemp, "Service %s removed",pName);
				WriteLog(pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}
}

////////////////////////////////////////////////////////////////////// 
//
// Install
//
VOID Install(char* pPath, char* pName) 
{  
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d", nError);
		WriteLog(pTemp);
	}
	else
	{
		SC_HANDLE schService = CreateService
		( 
			schSCManager,	/* SCManager database      */ 
			pName,			/* name of service         */ 
			pName,			/* service name to display */ 
			SERVICE_ALL_ACCESS,        /* desired access          */ 
			SERVICE_WIN32_OWN_PROCESS, /* service type            */ 
			SERVICE_AUTO_START,      /* start type              */ 
			SERVICE_ERROR_NORMAL,      /* error control type      */ 
			pPath,			/* service's binary        */ 
			NULL,                      /* no load ordering group  */ 
			NULL,                      /* no tag identifier       */ 
			NULL,                      /* no dependencies         */ 
			NULL,                      /* LocalSystem account     */ 
			NULL
		);                     /* no password             */ 
		if (schService==0) 
		{
			long nError =  GetLastError();
			char pTemp[121];
			sprintf(pTemp, "Failed to create service %s, error code = %d", pName, nError);
			WriteLog(pTemp);
		}
		else
		{
			char pTemp[121];
			sprintf(pTemp, "Service %s installed", pName);
			WriteLog(pTemp);
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}


////////////////////////////////////////////////////////////////////// 
//
// Standard C Main
//
void main(int argc, char *argv[] )
{
	// initialize global critical section
	::InitializeCriticalSection(&myCS);
	// initialize variables for .exe, .ini, and .log file names
	char pModuleFile[nBufferSize+1];
	DWORD dwSize = GetModuleFileName(NULL,pModuleFile,nBufferSize);
	pModuleFile[dwSize] = 0;
	if(dwSize>4&&pModuleFile[dwSize-4]=='.')
	{
		sprintf(pExeFile,"%s",pModuleFile);
		pModuleFile[dwSize-4] = 0;
		sprintf(pInitFile,"%s.ini",pModuleFile);
		sprintf(pLogFile,"%s.log",pModuleFile);
	}
	else
	{
		printf("Invalid module file name: %s\r\n", pModuleFile);
		return;
	}
	WriteLog(pExeFile);
	WriteLog(pInitFile);
	WriteLog(pLogFile);
	// read service name from .ini file
	strcpy(pServiceName, "VRPN Server Service");
	WriteLog(pServiceName);
	// uninstall service if switch is "-u"
	if(argc==2&&_stricmp("-u",argv[1])==0)
	{
		UnInstall(pServiceName);
	}
	// install service if switch is "-i"
	else if(argc==2&&_stricmp("-i",argv[1])==0)
	{			
		Install(pExeFile, pServiceName);
	}
	// bounce service if switch is "-b"
	else if(argc==2&&_stricmp("-b",argv[1])==0)
	{			
		KillService(pServiceName);
		RunService(pServiceName,0,NULL);
	}
	else if(argc==2&&_stricmp("-d",argv[1])==0)
	{
		serviceMain();
	}
	// assume user is starting this service 
	else 
	{
		// pass dispatch table to service controller
		if(!StartServiceCtrlDispatcher(DispatchTable))
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "StartServiceCtrlDispatcher failed, error code = %d", nError);
			WriteLog(pTemp);
		}
		// you don't get here unless the service is shutdown
	}
	::DeleteCriticalSection(&myCS);
}

