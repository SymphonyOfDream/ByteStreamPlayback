using System;
using System.Collections.Generic;
using System.Text;

using System.IO.Ports;




namespace SerialPortRecorder
{
   /// <summary>
   /// The SimpleSerialPort class encapsulates COM port names.
   /// </summary>
   public class SimpleSerialPort : IEquatable<SimpleSerialPort>, ICloneable
   {
      private int m_iValue;
      string m_strDisplay;


      static readonly SimpleSerialPort m_InvalidSimpleSerialPort = new SimpleSerialPort();


      static SortedList<int, SimpleSerialPort> m_SortedListComPortsObjects
               = new SortedList<int, SimpleSerialPort>();

      /// <summary>
      /// Baud Rates, since there's no associated enum within .NET/.NETCF
      /// </summary>
      public enum eBaudRate
      {
         Baud_110 = 110,
         Baud_300 = 300,
         Baud_1200 = 1200,
         Baud_2400 = 2400,
         Baud_4800 = 4800,
         Baud_9600 = 9600,
         Baud_19200 = 19200,
         Baud_38400 = 38400,
         Baud_57600 = 57600,
         Baud_115200 = 115200,
         Baud_230400 = 230400,
         Baud_460800 = 460800,
         Baud_921600 = 921600
      }


      #region CTORs/DTOR

      private SimpleSerialPort()
      {
         m_iValue = -1;
         m_strDisplay = "";
      }


      /// <summary>
      /// Creates a SimpleSerialPort object having the specified port number
      /// (i.e., 1 for "COM1") and display text (i.e., "COM1")
      /// </summary>
      /// <param name="iValue">Port number (i.e., 1 for "COM1")</param>
      /// <param name="strDisplay">Display text (i.e., "COM1")</param>
      public SimpleSerialPort(int iValue,
                              string strDisplay)
      {
         if (strDisplay == null)
         {
            m_iValue = -1;
            m_strDisplay = "";
         }
         else
         {
            strDisplay = strDisplay.Trim();
            if (strDisplay.Length < 1)
            {
               m_iValue = -1;
               m_strDisplay = "";
            }
            else
            {
               m_strDisplay = strDisplay;

               if (iValue < 1 || iValue > 99)
               {
                  m_iValue = -1;
                  m_strDisplay = "";
               }
               else
               {
                  m_iValue = iValue;
               }
            }
         }
      }


      /// <summary>
      /// Creates a SimpleSerialPort object from the string(s) returned by
      /// the .NET/.NETCF.
      /// </summary>
      /// <param name="strDotNetPortName">COM port name, as returned by
      /// .NET/.NETCF</param>
      public SimpleSerialPort(string strDotNetPortName)
      {
         if (strDotNetPortName == null)
         {
            m_iValue = -1;
            m_strDisplay = "";
         }
         else
         {
            if (strDotNetPortName.Length != 0)
            {
               strDotNetPortName = strDotNetPortName.Trim();
               if (strDotNetPortName.Length < 1)
               {
                  m_iValue = -1;
                  m_strDisplay = "";
               }
               else
               {

                  int iSerialPortNumber;
                  string strSerialPortNumber = GetTrailingNumber(strDotNetPortName);
                  if (strSerialPortNumber != null)
                  {
                     iSerialPortNumber = int.Parse(strSerialPortNumber);
                     if (iSerialPortNumber < 1 || iSerialPortNumber > 99)
                     {
                        m_iValue = -1;
                        m_strDisplay = "";
                     }
                     else
                     {
                        m_iValue = iSerialPortNumber;
                        m_strDisplay = strDotNetPortName;
                     }
                  }
                  else
                  {
                     m_iValue = -1;
                     m_strDisplay = "";
                  }
               }

            }
            else
            {
               // Source string is empty PRIOR to a .Trim().
               // This is a valid invalid object.
               m_iValue = -1;
               m_strDisplay = "";
            }
         }
      }


      /// <summary>
      /// Copy CTOR.
      /// </summary>
      /// <param name="source">Source object to make a copy of.</param>
      public SimpleSerialPort(SimpleSerialPort source)
      {
         m_iValue = source.m_iValue;
         m_strDisplay = string.Copy(source.m_strDisplay);
      }

      #endregion



      /// <summary>
      /// Only method that will return an invalid SimpleSerialPort object (whose ToString() 
      /// method will return an empty string).
      /// </summary>
      /// <returns>Invalid SimpleSerialPort object.</returns>
      static public SimpleSerialPort CreateInvalid()
      {
         return SimpleSerialPort.m_InvalidSimpleSerialPort;
      }


      /// <summary>
      /// Returns a list of all Baud Rates.
      /// </summary>
      static public List<eBaudRate> AllBaudRates
      {
         get
         {
            List<eBaudRate> lstAllBaudRates = new List<eBaudRate>();
            lstAllBaudRates.Add(eBaudRate.Baud_110);
            lstAllBaudRates.Add(eBaudRate.Baud_300);
            lstAllBaudRates.Add(eBaudRate.Baud_1200);
            lstAllBaudRates.Add(eBaudRate.Baud_2400);
            lstAllBaudRates.Add(eBaudRate.Baud_4800);
            lstAllBaudRates.Add(eBaudRate.Baud_9600);
            lstAllBaudRates.Add(eBaudRate.Baud_19200);
            lstAllBaudRates.Add(eBaudRate.Baud_38400);
            lstAllBaudRates.Add(eBaudRate.Baud_57600);
            lstAllBaudRates.Add(eBaudRate.Baud_115200);
            lstAllBaudRates.Add(eBaudRate.Baud_230400);
            lstAllBaudRates.Add(eBaudRate.Baud_460800);
            lstAllBaudRates.Add(eBaudRate.Baud_921600);
            return lstAllBaudRates;
         }
      }


      static void UpdateSortedPortObjects()
      {
         lock (SimpleSerialPort.m_SortedListComPortsObjects)
         {
            SimpleSerialPort.m_SortedListComPortsObjects.Clear();

            string[] astrSerialPorts = SerialPort.GetPortNames();

            foreach (string strSerialPort in astrSerialPorts)
            {
               if (strSerialPort.IndexOf("COM") < 0 || strSerialPort == "COM0")
               {
                  continue;
               }

               SimpleSerialPort portObject = new SimpleSerialPort(strSerialPort);

               if (portObject == null || portObject.IsValid == false)
               {
                  continue;
               }

               SimpleSerialPort.m_SortedListComPortsObjects.Add(portObject.Value,
                                                                portObject);
            } // foreach (string strSerialPort in astrSerialPorts)
         }
      }


      /// <summary>
      /// Returns a list of all SimpleSerialPort objects available
      /// on your system (all available COM ports are contained).
      /// </summary>
      /// <returns>List of all SimpleSerialPort objects available
      /// on your system (all available COM ports are contained).</returns>
      static public List<SimpleSerialPort> GetSortedPortObjects()
      {
         UpdateSortedPortObjects();

         List<SimpleSerialPort> sortedList;
         lock (SimpleSerialPort.m_SortedListComPortsObjects)
         {
            sortedList = new List<SimpleSerialPort>();

            foreach (KeyValuePair<int, SimpleSerialPort> pair in SimpleSerialPort.m_SortedListComPortsObjects)
            {
               sortedList.Add(pair.Value);
            }
         }

         return sortedList;
      }


      /// <summary>
      /// Returns an array of all SimpleSerialPort objects available
      /// on your system (all available COM ports are contained).
      /// </summary>
      /// <returns>Array of all SimpleSerialPort objects available
      /// on your system (all available COM ports are contained).</returns>
      static public SimpleSerialPort[] GetSortedPortObjectsArray()
      {

         UpdateSortedPortObjects();

         SimpleSerialPort[] aSimpleSerialPort;

         lock (SimpleSerialPort.m_SortedListComPortsObjects)
         {
            aSimpleSerialPort
               = new SimpleSerialPort[SimpleSerialPort.m_SortedListComPortsObjects.Count];

            int idx = 0;
            foreach (KeyValuePair<int, SimpleSerialPort> pair in SimpleSerialPort.m_SortedListComPortsObjects)
            {
               aSimpleSerialPort[idx++] = pair.Value;
            }
         }
         return aSimpleSerialPort;
      }


      /// <summary>
      /// Returns true iif SimpleSerialPort has a valid Value and display string.
      /// </summary>
      public bool IsValid
      {
         get
         {
            return (m_iValue > 0 && m_iValue < 100)
                   && (m_strDisplay != null && m_strDisplay.Trim().Length > 0);
         }
      }


      /// <summary>
      /// Returns true iif SimpleSerialPort has a valid Value and display string,
      /// AND the COM port specified actually exists in the system.
      /// </summary>
      public bool Exists
      {
         get
         {
            if (!this.IsValid)
            {
               return false;
            }

            SimpleSerialPort.UpdateSortedPortObjects();

            return SimpleSerialPort.m_SortedListComPortsObjects.ContainsKey(m_iValue);
         }
      }


      /// <summary>
      /// Returns actual port # (i.e., 1 for "COM1")
      /// </summary>
      public int Value
      {
         get { return m_iValue; }
      }


      /// <summary>
      /// Returns the text used by .NET Serial Port class to open ports with.
      /// </summary>
      public string Display
      {
         get { return m_strDisplay; }
      }


      /// <summary>
      /// Creates a SimpleSerialPort from a string.
      /// </summary>
      /// <param name="strSource">String to parse.</param>
      /// <returns>SimpleSerialPort object (an invalid one of string
      /// could not be properly parsed).</returns>
      public static SimpleSerialPort Parse(string strSource)
      {
         SimpleSerialPort ssp = null;
         try
         {
            ssp = new SimpleSerialPort(strSource);
         }
         catch
         {
            ssp = SimpleSerialPort.CreateInvalid();
         }
         return ssp;
      }


      /// <summary>
      /// Helper function that reverses a string.
      /// </summary>
      /// <param name="strSource">String to be reversed.</param>
      /// <returns>Reversed copy of the string parameter.</returns>
      public string Reverse(string strSource)
      {
         if (string.IsNullOrEmpty(strSource))
            return strSource;

         if (strSource.Trim(null).Length < 1)
            return strSource;

         char[] acIndividualCharacters = strSource.ToCharArray();
         Array.Reverse(acIndividualCharacters);

         return new String(acIndividualCharacters);
      }


      /// <summary>
      /// Returns the number that is at the end of a string.
      /// </summary>
      /// <param name="strSource">String that potentially contains a number at the
      /// end of it.</param>
      /// <returns>The string representation of the number at the end of the source
      /// string; null if no number exists in source string.</returns>
      public string GetTrailingNumber(string strSource)
      {
         if (string.IsNullOrEmpty(strSource))
            return null;

         if (strSource.Trim(null).Length < 1)
            return null;

         string strCollectedNonAlphas = "";
         bool bFoundFirstNumber = false;
         for (int idx = strSource.Length - 1; idx >= 0; --idx)
         {
            if (char.IsNumber(strSource[idx]) == false)
            {
               if (bFoundFirstNumber)
               {
                  break;
               }
               else
               {
                  continue;
               }
            }

            bFoundFirstNumber = true;

            strCollectedNonAlphas += strSource.Substring(idx, 1);
         }

         strCollectedNonAlphas = strCollectedNonAlphas.Trim(null);
         if (strCollectedNonAlphas.Length < 1)
            return null;

         strCollectedNonAlphas = Reverse(strCollectedNonAlphas);

         int iFoundTrailingNumber;
         try
         {
            iFoundTrailingNumber = System.Convert.ToInt32(strCollectedNonAlphas);
         }
         catch
         {
            return null;
         }
         return iFoundTrailingNumber.ToString();
      }


      /// <summary>
      /// Returns the text used by .NET Serial Port class to open ports with.
      /// </summary>
      /// <returns>Text used by .NET Serial Port class to open ports with.</returns>
      public override string ToString()
      {
         return m_strDisplay;
      }


      public override int GetHashCode()
      {
         return m_strDisplay.GetHashCode();
      }


      public override bool Equals(object obj)
      {
         if (obj == null)
            return false;

         return this.Equals(obj as SimpleSerialPort);
      }


      #region IEquatable<SimpleSerialPort> Members

      public bool Equals(SimpleSerialPort other)
      {
          if (other == null)
              return false;

          return m_strDisplay == other.m_strDisplay;
      }

      public bool Equals(string other)
      {
          if (other == null)
              return false;

          return m_strDisplay == other;
      }

      #endregion


      #region ICloneable Members

      public object Clone()
      {
         return new SimpleSerialPort(this);
      }

      #endregion

   }  // class SimpleSerialPort

}  // namespace BadgerMeter.Library
