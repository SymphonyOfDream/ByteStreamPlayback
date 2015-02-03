#pragma once

extern "C" 
{



class PortSettings {
public:
   PortSettings(const int iComPort,
                const int iBaudRate,
                const int iDataBits,
                const int iParity,
                const int iStopBits,
                const int iFlowControl) : m_iComPort(iComPort),
                                          m_iBaudRate(iBaudRate),
                                          m_iDataBits(iDataBits),
                                          m_iParity(iParity),
                                          m_iStopBits(iStopBits),
                                          m_iFlowControl(iFlowControl)
   {;}

   int GetComPort() const { return m_iComPort; }
   int GetBaudRate() const { return m_iBaudRate; }
   int GetDataBits() const { return m_iDataBits; }
   int GetParity() const { return m_iParity; }
   int GetStopBits() const { return m_iStopBits; }
   int GetFlowControl() const { return m_iFlowControl; }

   BOOL GetThreadStop() const { return *m_bStopThread; }
   void SetThreadStop(BOOL* bStopThread) { m_bStopThread = bStopThread; }
   HANDLE GetCommPort() const { return m_hCommPort; }
   void SetCommPort(HANDLE hComm) { m_hCommPort = hComm; }
   int GetThreadReturn() const { return m_iThreadReturn; }
   void SetThreadReturn(int iThreadReturn)  { m_iThreadReturn = iThreadReturn; }
   DWORD GetThreadLastError() const { return m_iThreadLastError; }
   void SetThreadLastError(DWORD iThreadLastError)  { m_iThreadLastError = iThreadLastError; }

private:
   const int m_iComPort;
   const int m_iBaudRate;
   const int m_iDataBits;
   const int m_iParity;
   const int m_iStopBits;
   const int m_iFlowControl;
   BOOL* m_bStopThread;
   HANDLE m_hCommPort;
   int m_iThreadReturn;
   DWORD m_iThreadLastError;
};


};
