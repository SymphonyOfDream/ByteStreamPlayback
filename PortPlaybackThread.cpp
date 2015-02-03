// PortPlaybackThread.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#include "PortPlaybackThread.h"
#include "DataStream.h"

#define R_BUFF_SIZE 4096
#define MS_PER_ST_UPDATE 10




extern "C" 
{




#if !defined(WINCE)
   #define DEFAULT_SERIALPORT_SPECIFICATION_TEMPLATE     L"\\\\.\\COM%d"
   #define DEFAULT_SERIALPORT_SPECIFICATION_TEMPLATE_A    "\\\\.\\COM%d"
#else
   #define DEFAULT_SERIALPORT_SPECIFICATION_TEMPLATE     L"COM%d:"
   #define DEFAULT_SERIALPORT_SPECIFICATION_TEMPLATE_A    "COM%d:"
#endif




bool m_bInitialized = false;


DataStream* m_apDataStreams[THREAD_MAX] = {0};
int m_iDataStreamCount = 0;


DWORD m_dwPlaybackStartMilliseconds = 0;




const char m_cHexNibbles[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

const char m_cHexNibblesUpperCase[] = {
	'A', 'B', 'C', 'D', 'E', 'F'
};




BOOL ReadCaptureLine(DataStream* pDataStream);
void HexStringToBytes(const char* pHex, int iHexLength, BYTE* pBinary);
int HexNibbleToNbr(char nibble);
BOOL WritePacket(DataStream* pDataStream, DataPacket* pDataPacket);
DWORD WINAPI ThreadProc( LPVOID lpParam );



DWORD m_dwMinimumScheculedMilliSecondToSend = 0xFFFFFFFFlu;

LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Initialize(void)
{
	for (int idx = 0; idx < m_iDataStreamCount; ++idx)
	{
		if (m_apDataStreams[idx] != NULL)
			delete m_apDataStreams[idx];
	}

	memset(m_apDataStreams, 0, sizeof(m_apDataStreams));
	m_iDataStreamCount = 0;

	m_bInitialized = true;
}



LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_DeInitialize(void)
{
	PortPlayback_Stop();

	for (int idx = 0; idx < m_iDataStreamCount; ++idx)
	{
		if (m_apDataStreams[idx] != NULL)
			delete m_apDataStreams[idx];
	}

	memset(m_apDataStreams, 0, sizeof(m_apDataStreams));
	m_iDataStreamCount = 0;

	m_bInitialized = false;
}



LIBRARY_BYTESTREAMPLAYBACK_API 
int PortPlayback_AddDataStream(const wchar_t wszFilename[], 
							   const int iComPortToPlayOn)
{
	if (m_iDataStreamCount >= THREAD_MAX)
		return -1;

	DataStream* pDataStream = new DataStream(wszFilename, iComPortToPlayOn);

	m_apDataStreams[m_iDataStreamCount] = pDataStream;



	// read the streams, compute the playback times and create the byte array to send

	// first, open the capture file
	HANDLE hFile
		= CreateFile(pDataStream->m_wszFilename,  
					 GENERIC_READ, 
					 0, 
					 0, 
					 OPEN_ALWAYS,
					 0,
					 0);

	int iResult = 0;
	bool bStartThreads = true;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// error opening file; abort, returning the port #
		DWORD dw = GetLastError(); 
		iResult = pDataStream->m_iComPortToPlayOn;
		bStartThreads = false;

		delete pDataStream;
	    m_apDataStreams[m_iDataStreamCount] = NULL;
		return -2;
	}

	pDataStream->m_hFile = hFile;

	// read the header
	int iFileIndex = -1;
	DWORD dwTotalBytesRead = 0;

	DWORD dwBytesRead;
	char strRecordInit[R_BUFF_SIZE];
	memset(strRecordInit, 0, sizeof(strRecordInit));

	char* acSubStrings[8];  
	memset(acSubStrings, 0, sizeof(acSubStrings));

	int token = -1;
	acSubStrings[++token] = strRecordInit;

	char c;

	bool bReadingHeader = true;
	while(bReadingHeader) 
	{
		if (!ReadFile(hFile, &c, 1, &dwBytesRead, 0))  // !!!
		{
			// error reading file; abort, returning the port # + 1100
			DWORD dwLastErr = GetLastError();
			iResult = (pDataStream->m_iComPortToPlayOn + 1100);
			bStartThreads = false;

			delete pDataStream;
			m_apDataStreams[m_iDataStreamCount] = NULL;

			::CloseHandle(hFile);

			return -3;
		}

		if (dwBytesRead == 0)
			break;

		switch (c)
		{
			default:
				strRecordInit[dwTotalBytesRead++] = c;
				break;

			case '\t':
				strRecordInit[dwTotalBytesRead++] = '\0';
				acSubStrings[++token] = &(strRecordInit[dwTotalBytesRead]);
				break;

			case '\r':
				strRecordInit[dwTotalBytesRead++] = '\0';
				acSubStrings[++token] = &(strRecordInit[dwTotalBytesRead]);

				if (!ReadFile(hFile, &c, 1, &dwBytesRead, 0)) 
				{
					// error reading from file; abort, returning the port # + 1300
					DWORD dwLastErr = GetLastError();
					iResult = (pDataStream->m_iComPortToPlayOn + 1300);
					bStartThreads = false;

					delete pDataStream;
					m_apDataStreams[m_iDataStreamCount] = NULL;

					::CloseHandle(hFile);

					return -4;
				}

				if (c == '\n') 
				{
					bReadingHeader = false;
				}
				else
				{
					// error reading from file; abort, returning the port # + 1300
					iResult = (pDataStream->m_iComPortToPlayOn + 1300);
					bStartThreads = false;

					delete pDataStream;
					m_apDataStreams[m_iDataStreamCount] = NULL;

					::CloseHandle(hFile);

					return -5;
				}

				break;
		}
	}  // while(true)

	// acSubStrings[0] contains "CAN PORT" || "SERIAL PORT"
	// acSubStrings[1] should contain pointer to COM port recording was made on during file creation
	// acSubStrings[2] : baud
	// acSubStrings[3] : data bits
	// acSubStrings[4] : parity
	// acSubStrings[5] : stop bits
	// acSubStrings[6] : flow control
   const bool bIsCanPort
      = !strcmp(acSubStrings[0], "CAN PORT");

   int iIndexToStartAt = 2;
   if (!bIsCanPort)
   {
      if (strcmp(acSubStrings[0], "SERIAL PORT"))
      {
         // So neither CAN PORT nor SERIAL PORT text find, an older
         // playback file format.

	      // acSubStrings[0] should contain pointer to COM port recording was made on during file creation
	      // acSubStrings[1] : baud
	      // acSubStrings[2] : data bits
	      // acSubStrings[3] : parity
	      // acSubStrings[4] : stop bits
	      // acSubStrings[5] : flow control

         iIndexToStartAt = 1;
      }
   }

	if (acSubStrings[iIndexToStartAt] == NULL) {
		iResult = (pDataStream->m_iComPortToPlayOn + 1300);
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);

		return -11;
	}

	const int iBaudRate = atoi(acSubStrings[iIndexToStartAt++]);

	if (acSubStrings[iIndexToStartAt] == NULL) {
		iResult = (pDataStream->m_iComPortToPlayOn + 1300);
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);

		return -11;
	}
	const int iDataBits = atoi(acSubStrings[iIndexToStartAt++]);

	if (acSubStrings[iIndexToStartAt] == NULL) {
		iResult = (pDataStream->m_iComPortToPlayOn + 1300);
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);

		return -11;
	}
	const int iParity = atoi(acSubStrings[iIndexToStartAt++]);

	if (acSubStrings[iIndexToStartAt] == NULL) {
		iResult = (pDataStream->m_iComPortToPlayOn + 1300);
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);

		return -11;
	}
	const int iStopBits = atoi(acSubStrings[iIndexToStartAt++]);

	if (acSubStrings[iIndexToStartAt] == NULL) {
		iResult = (pDataStream->m_iComPortToPlayOn + 1300);
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);

		return -11;
	}
	const int iFlowControl = atoi(acSubStrings[iIndexToStartAt++]);

/*
	char* pEndPoint;
	m_uiHPC = strtoul((strRecordInit + (tokens[5])), &pEndPoint, 10);
	m_uiMultiMsBlock = ((m_uiHPC/1000) * MS_PER_ST_UPDATE);
*/
	// open the com port


	wchar_t wszComPort[MAX_PATH+1] = {0};
	swprintf(wszComPort,
		     DEFAULT_SERIALPORT_SPECIFICATION_TEMPLATE,
			 pDataStream->m_iComPortToPlayOn);

	HANDLE hCommPort
		= CreateFile(wszComPort,  // filename
					 GENERIC_WRITE | GENERIC_READ,  // desired access
					 0, // share mode
					 NULL, // Security Attributes 
					 OPEN_EXISTING,  // Creation Disposition
					 FILE_ATTRIBUTE_NORMAL,  // Flags And Attributes 
					 NULL);  // Template File 

	if (hCommPort == INVALID_HANDLE_VALUE)
	{
		// error opening port; abort, returning the port #
		iResult = pDataStream->m_iComPortToPlayOn;
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;
		return -6;
	}

	// set up the com port parameters
	DCB commState;
	if (GetCommState(hCommPort, &commState))
	{
		commState.BaudRate = iBaudRate;
		commState.ByteSize = iDataBits;
		commState.Parity = iParity;

		commState.StopBits = iStopBits;

		if (iFlowControl == 0) 
		{
			// XON/XOFF
			commState.fOutX = TRUE;
			commState.fInX = TRUE;
			// hardware
			commState.fOutxCtsFlow = FALSE;
			commState.fOutxDsrFlow = FALSE;
			commState.fDtrControl = DTR_CONTROL_DISABLE;
			commState.fRtsControl = RTS_CONTROL_DISABLE;
		}
		else if (iFlowControl == 1)
		{
			// XON/XOFF
			commState.fOutX = FALSE;
			commState.fInX = FALSE;
			// hardware
			commState.fOutxCtsFlow = TRUE;
			commState.fOutxDsrFlow = TRUE;
			commState.fDtrControl = DTR_CONTROL_ENABLE;
			commState.fRtsControl = RTS_CONTROL_ENABLE;
		}
		else
		{
			// flow control == none

			// XON/XOFF
			commState.fOutX = FALSE;
			commState.fInX = FALSE;
			// hardware
			commState.fOutxCtsFlow = FALSE;
			commState.fOutxDsrFlow = FALSE;
			commState.fDtrControl = DTR_CONTROL_DISABLE;
			commState.fRtsControl = RTS_CONTROL_DISABLE;
		}

		if (!SetCommState(hCommPort, &commState))
		{
			// error opening port; abort, returning the port # + 100
			DWORD dw = GetLastError(); 
			iResult = pDataStream->m_iComPortToPlayOn + 100;
			bStartThreads = false;

			delete pDataStream;
			m_apDataStreams[m_iDataStreamCount] = NULL;

			::CloseHandle(hFile);
			::CloseHandle(hCommPort);

			return -7;
		}
	}
	else
	{
		// error opening port; abort, returning the port # + 200
		DWORD dw = GetLastError(); 
		iResult = pDataStream->m_iComPortToPlayOn + 200;
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);
		::CloseHandle(hCommPort);

		return -8;
	}


	// set up the com port timeouts
	COMMTIMEOUTS commTimeouts;
	if (GetCommTimeouts(hCommPort, &commTimeouts))
	{
		commTimeouts.WriteTotalTimeoutConstant = 0;
		commTimeouts.WriteTotalTimeoutMultiplier = 0;

		if (!SetCommTimeouts(hCommPort, &commTimeouts))
		{
			// error opening port; abort, returning the port # + 300
			DWORD dw = GetLastError(); 
			iResult = pDataStream->m_iComPortToPlayOn + 300;
			bStartThreads = false;

			delete pDataStream;
			m_apDataStreams[m_iDataStreamCount] = NULL;

			::CloseHandle(hFile);
			::CloseHandle(hCommPort);

			return -9;
		}
	}
	else
	{
		// error opening port; abort, returning the port # + 400
		DWORD dw = GetLastError(); 
		iResult = pDataStream->m_iComPortToPlayOn + 400;
		bStartThreads = false;

		delete pDataStream;
		m_apDataStreams[m_iDataStreamCount] = NULL;

		::CloseHandle(hFile);
		::CloseHandle(hCommPort);

		return -10;
	}

	pDataStream->m_hComPort = hCommPort;

	// Read in ALL data to play back
	while (ReadCaptureLine(pDataStream));

	return m_iDataStreamCount++;
}



LIBRARY_BYTESTREAMPLAYBACK_API 
DWORD PortRecorder_GetTotalBytesPlayed(int iPortPlaybackObjectID)
{
	if (iPortPlaybackObjectID < 0 || iPortPlaybackObjectID >= m_iDataStreamCount)
		return 0;

	return m_apDataStreams[iPortPlaybackObjectID]->m_dwTotalBytesPlayed;
}



volatile long* m_pThreadPlayingBackCount;

LIBRARY_BYTESTREAMPLAYBACK_API 
int PortPlayback_Play(volatile long* pThreadPlayingBackCount)
{
	*pThreadPlayingBackCount = 0;
	m_pThreadPlayingBackCount = pThreadPlayingBackCount;

    if (!m_bInitialized)
        return -1;

	if (m_iDataStreamCount < 1)
		return -2;

    HANDLE  ahThreadArray[THREAD_MAX]; 
    memset(ahThreadArray, NULL, sizeof(ahThreadArray));

    DWORD   adwThreadIdArray[THREAD_MAX];
    memset(adwThreadIdArray, 0, sizeof(adwThreadIdArray));

	for (int threadIdx = 0; threadIdx < m_iDataStreamCount; ++threadIdx)
	{
	   DataPacket* pDataPacket = m_apDataStreams[threadIdx]->m_pDataPacketRootNode;
	   while (pDataPacket != NULL)
      {
         pDataPacket->m_dwScheculedMilliSecondToSend -= m_dwMinimumScheculedMilliSecondToSend;
   		pDataPacket = pDataPacket->m_pNext;
      }


        ahThreadArray[threadIdx] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ThreadProc,       // thread function name
            m_apDataStreams[threadIdx],          // argument to thread function 
            CREATE_SUSPENDED,    // we'll check for creation 
            &(adwThreadIdArray[threadIdx]));   // returns the thread identifier 

      if (ahThreadArray[threadIdx] != NULL) 
      {
         // It's very important that our threads are NOT delayed by
         // the client's GUI thread for whatever reason.
         ::SetThreadPriority(ahThreadArray[threadIdx], THREAD_PRIORITY_HIGHEST);
      }
	}

   m_dwPlaybackStartMilliseconds = ::GetTickCount();

	for (int threadIdx = 0; threadIdx < m_iDataStreamCount; ++threadIdx)
	{
		DWORD dwRC = ResumeThread(ahThreadArray[threadIdx]);
		if (dwRC == (DWORD) -1)
		{
			DWORD dwErr = GetLastError();
		}
	}
}




LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Pause(void)
{
	if (!m_bInitialized)
		return;

	for (int idx = 0; idx < m_iDataStreamCount; ++idx)
		m_apDataStreams[idx]->m_bStopThread = TRUE;
}




LIBRARY_BYTESTREAMPLAYBACK_API 
void PortPlayback_Stop(void)
{
	if (!m_bInitialized)
		return;

	for (int idx = 0; idx < m_iDataStreamCount; ++idx) {
		m_apDataStreams[idx]->m_bStopThread = TRUE;
		::Sleep(500);
	}
}



BOOL ReadCaptureLine(DataStream* pDataStream)
{
	char* pszSubStrings[7]; 
	memset(pszSubStrings, 0, sizeof(pszSubStrings));
	
	char strRecord[R_BUFF_SIZE];
	memset(strRecord, 0, sizeof(strRecord));

	int iTokenIdx = -1;
	pszSubStrings[++iTokenIdx] = strRecord;

	BOOL bFailed = false;
	DWORD dwBytesRead;

	DWORD dwTotalBytesRead = 0;

	char c;

	int iResult = 0;

	bool bReadingRecord = true;
	while (bReadingRecord) 
	{
		if (!ReadFile(pDataStream->m_hFile, &c, 1, &dwBytesRead, 0))
		{
			// error reading from file; abort, returning the port # + 1100
			DWORD dwLastErr = GetLastError();
			iResult = (pDataStream->m_iComPortToPlayOn + 1100);
			bFailed = true;
			bReadingRecord = false;
			continue;
		}

		if (dwBytesRead == 0)
		{
			bReadingRecord = false;
			continue;
		}

		switch (c)
		{
			default:
				strRecord[dwTotalBytesRead++] = c;
				break;

			case '\t':
				strRecord[dwTotalBytesRead++] = '\0';
				pszSubStrings[++iTokenIdx] = &(strRecord[dwTotalBytesRead]);
				break;

			case '\r':
				strRecord[dwTotalBytesRead++] = '\0';
				pszSubStrings[++iTokenIdx] = &(strRecord[dwTotalBytesRead]);

				// Read trailing \n
				if ((!ReadFile(pDataStream->m_hFile, &c, 1, &dwBytesRead, 0)) || (dwBytesRead == 0))
				{
					// error writing to file; abort, returning the port # + 1300
					DWORD dwLastErr = GetLastError();
					iResult = (pDataStream->m_iComPortToPlayOn + 1300);
					bFailed = TRUE;
				}

				if (c != '\n') 
				{
					// error writing to file; abort, returning the port # + 1300
					iResult = (pDataStream->m_iComPortToPlayOn + 1300);
					bFailed = TRUE;
				}

				bReadingRecord = false;
				break;
		}
	}  // while (true)


	// if not a full line 
	//          "[milliseconds]\t[bytes read]\r\n"
	// just return

	if (iTokenIdx != 2)
		return false;

	if (bFailed)
		return false;

	// "01234\ta8fe84b5234e\r\n"  << Data will ALWAYS have 2-digit hex values (byte), never odd # of hex values (eg, 0xa should be impossible)
	// pszSubStrings[0] = "012345"
	// pszSubStrings[1] = "a8fe84b5234e"
	DataPacket dataPacket;

	dataPacket.m_dwScheculedMilliSecondToSend = (DWORD)_atoi64(strRecord);
   if (m_dwMinimumScheculedMilliSecondToSend > dataPacket.m_dwScheculedMilliSecondToSend)
   {
      m_dwMinimumScheculedMilliSecondToSend = dataPacket.m_dwScheculedMilliSecondToSend;
   }

	const DWORD dwTotalDataHexDigits = strlen(pszSubStrings[1]);
	const DWORD dwByteCount = dwTotalDataHexDigits/2;

	BYTE* pbyteData = new BYTE[dwByteCount];

	HexStringToBytes(pszSubStrings[1], dwTotalDataHexDigits, pbyteData);

	dataPacket.AddData(pbyteData, dwByteCount);

	delete [] pbyteData;

	pDataStream->AddDataPacket(&dataPacket);

	return true;
}




int HexNibbleToNbr(char nibble)
{
	int i = 0;
	for (i = 15; i >= 0; --i)
	{
		if (m_cHexNibbles[i] == nibble)
			return i;
	}

	// the hex values in the capture file started out as lower case but just in case...
	for (i = 5; i >= 0; --i)
	{
		if (m_cHexNibblesUpperCase[i] == nibble)
			return i + 10;
	}

	return i;
}




void HexStringToBytes(const char* pHex, int iHexLength, BYTE* pBinary)
{
	// Need the 'iHexLength - 1' because each iteration
	// uses pHex[i] AND pHex[i+1]
	for (int i = 0, j = 0; i < iHexLength - 1; ++j)
	{
		pBinary[j] = HexNibbleToNbr(pHex[i++]) << 4;
		pBinary[j] += HexNibbleToNbr(pHex[i++]);
	}
}





BOOL WritePacket(DataStream* pDataStream, 
				 DataPacket* pDataPacket)
{
	DWORD dwTotalBytesWritten = 0;
	DWORD dwBytesWritten = 0;
	BOOL bSuccess;

	BOOL bResult = TRUE;
	
	DWORD dwStartTicks = GetTickCount();

	while (   dwTotalBytesWritten < pDataPacket->m_dwDataLength
		   && (GetTickCount() - dwStartTicks) < pDataPacket->m_dwDataLength*50)
	{
		bResult = WriteFile(pDataStream->m_hComPort, 
		                    pDataPacket->m_pbyteData + dwTotalBytesWritten, 
							pDataPacket->m_dwDataLength - dwTotalBytesWritten, 
							&dwBytesWritten, 
							NULL);
    
		// Check for a problem or pending operation. 
		if (bResult) 
		{ 
			dwTotalBytesWritten += dwBytesWritten;
			pDataStream->m_dwTotalBytesPlayed += dwBytesWritten;
		}
	}

	return true;
}



DWORD WINAPI ThreadProc( LPVOID lpParam )
{
	DataStream* pDataStream = (DataStream*)lpParam;
	if (pDataStream == NULL)
		return -1;

	if (pDataStream->m_pDataPacketRootNode == NULL)
		return -2;

	::InterlockedIncrement(m_pThreadPlayingBackCount);

	DataPacket* pDataPacket = pDataStream->m_pDataPacketRootNode;

	// play the stream
	DWORD dwNow;

	while (   (!pDataStream->m_bStopThread) 
		   && pDataPacket != NULL)
	{
		dwNow = ::GetTickCount();

		if (dwNow < m_dwPlaybackStartMilliseconds)
		{
			// GetTickCount() rollover occured.
			// 0     dwNow   m_dwPlaybackStartMilliseconds   UINT_MAX
			// |     |       |                               |
			// +-----+-------+-------------------------------+
			dwNow = (UINT_MAX - m_dwPlaybackStartMilliseconds) + dwNow;
		}
		else 
		{
			// 0   m_dwPlaybackStartMilliseconds     dwNow   UINT_MAX
			// |   |                                |        |
			// +---+--------------------------------+--------+
			dwNow = dwNow - m_dwPlaybackStartMilliseconds;
		}

		int iDiff = int((__int64)pDataPacket->m_dwScheculedMilliSecondToSend - (__int64)dwNow);

		if (iDiff > 0)
			Sleep(iDiff);

		WritePacket(pDataStream, pDataPacket);

		pDataPacket = pDataPacket->m_pNext;
	}

	::InterlockedDecrement(m_pThreadPlayingBackCount);

 	return 0;
}







};

