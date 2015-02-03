#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PORTPLAYBACKTHREAD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBRARY_BYTESTREAMPLAYBACK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BYTESTREAMPLAYBACK_EXPORTS
   #define LIBRARY_BYTESTREAMPLAYBACK_API __declspec(dllexport)
#else
   #define LIBRARY_BYTESTREAMPLAYBACK_API __declspec(dllimport)
#endif




// carried over from the capture app
#define THREAD_MAX 15


extern "C" 
{


LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Initialize(void);

LIBRARY_BYTESTREAMPLAYBACK_API 
int PortPlayback_AddDataStream(const wchar_t wszFilename[], 
							          const int iComPortToPlayOn);



LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_DeInitialize(void);

LIBRARY_BYTESTREAMPLAYBACK_API 
DWORD PortRecorder_GetTotalBytesPlayed(int iPortPlaybackObjectID);

LIBRARY_BYTESTREAMPLAYBACK_API 
int PortPlayback_Play(volatile long* pThreadPlayingBackCount);


LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Pause(void);


LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Stop(void);


};
