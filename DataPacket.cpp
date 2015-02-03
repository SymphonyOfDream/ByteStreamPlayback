#include "StdAfx.h"
#include "DataPacket.h"


extern "C" 
{



DataPacket::DataPacket(void)
: m_dwScheculedMilliSecondToSend(0)
, m_dwDataLength(0)
, m_pNext(NULL)
, m_pbyteData(NULL)
{
}



DataPacket::DataPacket(const DataPacket* pDataPacket)
: m_dwScheculedMilliSecondToSend(pDataPacket->m_dwScheculedMilliSecondToSend)
, m_dwDataLength(pDataPacket->m_dwDataLength)
, m_pNext(NULL)
{
	m_pbyteData = new BYTE[m_dwDataLength];
	memcpy(m_pbyteData, pDataPacket->m_pbyteData, m_dwDataLength);
}



DataPacket::~DataPacket(void)
{
	if (m_pbyteData != NULL)
		delete [] m_pbyteData;
}



void DataPacket::AddData(const BYTE* pBuffer, int iBufferLen)
{
	m_pbyteData = new BYTE[iBufferLen];
	m_dwDataLength = iBufferLen;
	memcpy(m_pbyteData, pBuffer, iBufferLen);
}


};

