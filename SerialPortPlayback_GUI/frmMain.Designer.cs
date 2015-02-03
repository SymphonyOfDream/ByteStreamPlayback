namespace SerialPortPlayback
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cmdRemovePlaybackFile = new System.Windows.Forms.Button();
            this.cmdAddPlaybackFile = new System.Windows.Forms.Button();
            this.lstSelectedPlaybackFiles = new System.Windows.Forms.ListBox();
            this.cmdPlay = new System.Windows.Forms.Button();
            this.cmdExit = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.lstBytesPlayed = new System.Windows.Forms.ListBox();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // openFileDialog
            // 
            this.openFileDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.OpenFileDialog_FileOk);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.lstBytesPlayed);
            this.groupBox1.Controls.Add(this.cmdRemovePlaybackFile);
            this.groupBox1.Controls.Add(this.cmdAddPlaybackFile);
            this.groupBox1.Controls.Add(this.lstSelectedPlaybackFiles);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(648, 256);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Selected Files To Playback";
            // 
            // cmdRemovePlaybackFile
            // 
            this.cmdRemovePlaybackFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdRemovePlaybackFile.Enabled = false;
            this.cmdRemovePlaybackFile.Location = new System.Drawing.Point(528, 211);
            this.cmdRemovePlaybackFile.Name = "cmdRemovePlaybackFile";
            this.cmdRemovePlaybackFile.Size = new System.Drawing.Size(114, 39);
            this.cmdRemovePlaybackFile.TabIndex = 2;
            this.cmdRemovePlaybackFile.Text = "Remove Selected";
            this.cmdRemovePlaybackFile.UseVisualStyleBackColor = true;
            this.cmdRemovePlaybackFile.Click += new System.EventHandler(this.cmdRemovePlaybackFile_Click);
            // 
            // cmdAddPlaybackFile
            // 
            this.cmdAddPlaybackFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cmdAddPlaybackFile.Location = new System.Drawing.Point(6, 211);
            this.cmdAddPlaybackFile.Name = "cmdAddPlaybackFile";
            this.cmdAddPlaybackFile.Size = new System.Drawing.Size(114, 39);
            this.cmdAddPlaybackFile.TabIndex = 1;
            this.cmdAddPlaybackFile.Text = "Add...";
            this.cmdAddPlaybackFile.UseVisualStyleBackColor = true;
            this.cmdAddPlaybackFile.Click += new System.EventHandler(this.cmdAddPlaybackFile_Click);
            // 
            // lstSelectedPlaybackFiles
            // 
            this.lstSelectedPlaybackFiles.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstSelectedPlaybackFiles.FormattingEnabled = true;
            this.lstSelectedPlaybackFiles.Location = new System.Drawing.Point(87, 19);
            this.lstSelectedPlaybackFiles.Name = "lstSelectedPlaybackFiles";
            this.lstSelectedPlaybackFiles.Size = new System.Drawing.Size(555, 186);
            this.lstSelectedPlaybackFiles.TabIndex = 0;
            this.lstSelectedPlaybackFiles.SelectedIndexChanged += new System.EventHandler(this.lstSelectedPlaybackFiles_SelectedIndexChanged);
            // 
            // cmdPlay
            // 
            this.cmdPlay.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cmdPlay.Enabled = false;
            this.cmdPlay.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdPlay.Location = new System.Drawing.Point(215, 301);
            this.cmdPlay.Name = "cmdPlay";
            this.cmdPlay.Size = new System.Drawing.Size(261, 39);
            this.cmdPlay.TabIndex = 2;
            this.cmdPlay.Text = "Play";
            this.cmdPlay.UseVisualStyleBackColor = true;
            this.cmdPlay.Click += new System.EventHandler(this.cmdPlay_Click);
            // 
            // cmdExit
            // 
            this.cmdExit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cmdExit.Location = new System.Drawing.Point(546, 440);
            this.cmdExit.Name = "cmdExit";
            this.cmdExit.Size = new System.Drawing.Size(114, 39);
            this.cmdExit.TabIndex = 3;
            this.cmdExit.Text = "Exit";
            this.cmdExit.UseVisualStyleBackColor = true;
            this.cmdExit.Click += new System.EventHandler(this.cmdExit_Click);
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // lstBytesPlayed
            // 
            this.lstBytesPlayed.FormattingEnabled = true;
            this.lstBytesPlayed.Location = new System.Drawing.Point(6, 19);
            this.lstBytesPlayed.Name = "lstBytesPlayed";
            this.lstBytesPlayed.Size = new System.Drawing.Size(78, 186);
            this.lstBytesPlayed.TabIndex = 3;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(672, 491);
            this.Controls.Add(this.cmdExit);
            this.Controls.Add(this.cmdPlay);
            this.Controls.Add(this.groupBox1);
            this.Name = "frmMain";
            this.Text = "Serial Port Playback";
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button cmdRemovePlaybackFile;
        private System.Windows.Forms.Button cmdAddPlaybackFile;
        private System.Windows.Forms.ListBox lstSelectedPlaybackFiles;
        private System.Windows.Forms.Button cmdPlay;
        private System.Windows.Forms.Button cmdExit;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ListBox lstBytesPlayed;
    }
}

