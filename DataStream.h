#pragma once



#include "DataPacket.h"


extern "C" 
{



struct DataStream 
{
public:

	wchar_t m_wszFilename[MAX_PATH + 1];

	int m_iComPortToPlayOn;

	HANDLE m_hFile;
	HANDLE m_hComPort;
    BOOL m_bStopThread;

    DWORD m_dwTotalBytesPlayed;

	DWORD m_dwDataPacketCount;

	DataPacket* m_pDataPacketRootNode;
	DataPacket* m_pDataPacketTailNode;

	DataStream(const wchar_t wszFilename[], const int iComPortToPlayOn);
	~DataStream(void);


	void AddDataPacket(const DataPacket* pDataPacket);
};


};
