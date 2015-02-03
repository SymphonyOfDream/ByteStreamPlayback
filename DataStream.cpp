#include "StdAfx.h"
#include "DataStream.h"


extern "C" 
{



DataStream::DataStream(const wchar_t wszFilename[], const int iComPortToPlayOn) 
: m_iComPortToPlayOn(iComPortToPlayOn)
, m_hFile(INVALID_HANDLE_VALUE)
, m_hComPort(INVALID_HANDLE_VALUE)
, m_bStopThread(FALSE)
, m_dwTotalBytesPlayed(0)
, m_pDataPacketRootNode(NULL)
, m_pDataPacketTailNode(NULL)
, m_dwDataPacketCount(0)
{
	memset(m_wszFilename, 0, sizeof(m_wszFilename));
	wcscpy(m_wszFilename, wszFilename);
}




DataStream::~DataStream(void)
{
	if (m_pDataPacketRootNode != NULL)
	{
		while (m_pDataPacketRootNode->m_pNext != NULL)
		{
			DataPacket* pDataPacket = m_pDataPacketRootNode;
			m_pDataPacketRootNode = m_pDataPacketRootNode->m_pNext;
			delete pDataPacket;
		}

		delete m_pDataPacketRootNode;
	}
}




void DataStream::AddDataPacket(const DataPacket* pDataPacket)
{
	DataPacket* pNewDataPacket = new DataPacket(pDataPacket);

	if (m_pDataPacketRootNode == NULL)
	{
		m_pDataPacketRootNode = m_pDataPacketTailNode = pNewDataPacket;
	} 
	else 
	{
		m_pDataPacketTailNode->m_pNext = pNewDataPacket;
		m_pDataPacketTailNode = pNewDataPacket;
	}

	++m_dwDataPacketCount;
}


};
