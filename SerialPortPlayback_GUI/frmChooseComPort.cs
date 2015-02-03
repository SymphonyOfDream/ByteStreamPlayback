using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SerialPortRecorder;
using System.IO;



namespace SerialPortPlayback
{

    public partial class frmChooseComPort : Form
    {

        public SimpleSerialPort SelectedPort { get; private set; }


        public frmChooseComPort(string strSelectedFQFilename)
        {
            InitializeComponent();

            txtSelectedFile.Text = Path.GetFileName(strSelectedFQFilename);
        }


        private void frmChooseComPort_Load(object sender, EventArgs e)
        {
            cboComPorts.Items.Add(SimpleSerialPort.CreateInvalid());
            cboComPorts.Items.AddRange(SimpleSerialPort.GetSortedPortObjectsArray());

            cboComPorts.SelectedIndex = 0; // index 0 is invalid/blank
        }


        private void cboComPorts_SelectedIndexChanged(object sender, EventArgs e)
        {
            SelectedPort = (SimpleSerialPort)cboComPorts.SelectedItem;

            cmdOK.Enabled = SelectedPort.IsValid;
        }


        void CloseDialog(DialogResult rc)
        {
            this.DialogResult = rc;
            this.Hide();
            this.Close();
        }

        private void cmdCancel_Click(object sender, EventArgs e)
        {
            CloseDialog(DialogResult.Cancel);
        }

        private void cmdOK_Click(object sender, EventArgs e)
        {
            CloseDialog(DialogResult.OK);
        }

    }  // public partial class frmChooseComPort : Form

}  // namespace SerialPortPlayback
