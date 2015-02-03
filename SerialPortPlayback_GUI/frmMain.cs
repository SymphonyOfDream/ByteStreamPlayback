using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using SerialPortRecorder;



namespace SerialPortPlayback
{

    public partial class frmMain : Form
    {

        #region Native Methods - DllImport's


#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_Initialize")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_Initialize")]
#endif
        private static extern void PortPlayback_Initialize();



#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_AddDataStream")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_AddDataStream")]
#endif
        private static extern int PortPlayback_AddDataStream(string strRecordedByteStreamFQFilename,
                                                             int iComPortToPlayOn);


        
#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_DeInitialize")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_DeInitialize")]
#endif
        private static extern void PortPlayback_DeInitialize();


        
#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortRecorder_GetTotalBytesPlayed")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortRecorder_GetTotalBytesPlayed")]
#endif
        private static extern UInt32 PortRecorder_GetTotalBytesPlayed(int iPortPlaybackObjectID);



#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_Play")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_Play")]
#endif
        private static extern int PortPlayback_Play();


        
#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_Pause")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_Pause")]
#endif
        private static extern void PortPlayback_Pause();


        
#if DEBUG
        [DllImport("ByteStreamPlaybackD.dll", EntryPoint = "PortPlayback_Stop")]
#else
        [DllImport("ByteStreamRecord.dll", EntryPoint = "PortPlayback_Stop")]
#endif
        private static extern void PortPlayback_Stop();


        #endregion


        bool m_bIsPlaying = false;



        class PlaybackInfo
        {
            public int PlaybackObjectID { get; set; }
            public string FQFilename { get; set; }
            public SimpleSerialPort ComPort { get; set; }

            public PlaybackInfo()
            {
                FQFilename = "n/a";
                ComPort = SimpleSerialPort.CreateInvalid();
                PlaybackObjectID = -1;
            }

            public PlaybackInfo(string strFQFilename,
                                SimpleSerialPort comPort)
            {
                FQFilename = strFQFilename;
                ComPort = comPort;
                PlaybackObjectID = -1;
            }

            public override string ToString()
            {
                return string.Format("File: {0}, Playback Port: {1}",
                                     Path.GetFileName(FQFilename),
                                     ComPort);
            }
        }



        public frmMain()
        {
            PortPlayback_Initialize();

            InitializeComponent();
        }


        private void cmdExit_Click(object sender, EventArgs e)
        {
            this.Hide();
            this.Close();
        }


        void VerifyPlayability()
        {
            cmdPlay.Enabled = lstSelectedPlaybackFiles.Items.Count > 0;
        }

        private void lstSelectedPlaybackFiles_SelectedIndexChanged(object sender, EventArgs e)
        {
            cmdRemovePlaybackFile.Enabled = lstSelectedPlaybackFiles.SelectedIndex >= 0;
        }

        private void cmdAddPlaybackFile_Click(object sender, EventArgs e)
        {
            openFileDialog.ShowDialog(this);
            VerifyPlayability();
        }
    
        private void cmdRemovePlaybackFile_Click(object sender, EventArgs e)
        {
            int iDoomedIdx = lstSelectedPlaybackFiles.SelectedIndex;

            lstSelectedPlaybackFiles.Items.RemoveAt(iDoomedIdx);
            lstBytesPlayed.Items.RemoveAt(iDoomedIdx);

            VerifyPlayability();
        }



        private void OpenFileDialog_FileOk(object sender, CancelEventArgs e)
        {
            Application.DoEvents();

            string strFQFilename = openFileDialog.FileName;
            if (File.Exists(strFQFilename))
            {
                using (frmChooseComPort chooseComPortForm = new frmChooseComPort(strFQFilename))
                {
                    DialogResult rc = chooseComPortForm.ShowDialog(this);
                    if (rc == DialogResult.OK)
                    {
                        if (chooseComPortForm.SelectedPort.IsValid)
                        {
                            lstSelectedPlaybackFiles
                            .Items
                            .Add(new PlaybackInfo(strFQFilename,
                                                  chooseComPortForm.SelectedPort));

                            lstBytesPlayed.Items.Add(0);
                        }
                    }
                }
            }
        }



        private void cmdPlay_Click(object sender, EventArgs e)
        {
            if (!m_bIsPlaying)
            {
                int iStreamCount = 0;
                foreach (object obj in lstSelectedPlaybackFiles.Items)
                {
                    PlaybackInfo playbackInfo = (PlaybackInfo)obj;

                    if (!playbackInfo.ComPort.IsValid)
                        continue;

                    playbackInfo.PlaybackObjectID
                        = PortPlayback_AddDataStream(playbackInfo.FQFilename,
                                                     playbackInfo.ComPort.Value);

                    if (playbackInfo.PlaybackObjectID >= 0)
                        iStreamCount++;
                }

                if (iStreamCount > 0)
                {
                    PortPlayback_Play();

                    m_bIsPlaying = true;
                    cmdPlay.Text = "STOP";

                    timer1.Enabled = true;
                }
            }
            else
            {
                timer1.Enabled = false;

                PortPlayback_Stop();
                PortPlayback_DeInitialize();

                cmdPlay.Text = "Play";
                m_bIsPlaying = false;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            for (int idx = 0; idx < lstSelectedPlaybackFiles.Items.Count; ++idx)
            {
                PlaybackInfo playbackInfo = (PlaybackInfo)lstSelectedPlaybackFiles.Items[idx];

                lstBytesPlayed.Items[idx] = PortRecorder_GetTotalBytesPlayed(playbackInfo.PlaybackObjectID);
            }
        }


    }  // public partial class frmMain : Form

}  // namespace SerialPortPlayback
