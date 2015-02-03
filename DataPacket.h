#pragma once



extern "C" 
{



struct DataPacket 
{
public:
	DWORD m_dwScheculedMilliSecondToSend;

	BYTE* m_pbyteData;
	DWORD m_dwDataLength;

	DataPacket* m_pNext;


	DataPacket(void);
	DataPacket(const DataPacket* pDataPacket);

	~DataPacket(void);

	void AddData(const BYTE* pBuffer, int iBufferLen);
};


};