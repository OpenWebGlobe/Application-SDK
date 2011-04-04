namespace HeaderGenerator
{
    partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.textBoxDataBaseFile = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonExit = new System.Windows.Forms.Button();
            this.buttonConnectDatabase = new System.Windows.Forms.Button();
            this.textBoxProvider = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonChooseTargetDir = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxTargetDir = new System.Windows.Forms.TextBox();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxDataBaseFile
            // 
            this.textBoxDataBaseFile.Location = new System.Drawing.Point(77, 23);
            this.textBoxDataBaseFile.Name = "textBoxDataBaseFile";
            this.textBoxDataBaseFile.Size = new System.Drawing.Size(463, 20);
            this.textBoxDataBaseFile.TabIndex = 0;
            this.textBoxDataBaseFile.Text = "T:\\A1334_IVGI_Projekte\\A1334_i3D\\10_Datenbanken\\OpenGlobe\\OpenGlobeSDK.accdb";
            this.textBoxDataBaseFile.TextChanged += new System.EventHandler(this.textBoxDataBaseFile_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Database:";
            // 
            // buttonExit
            // 
            this.buttonExit.Location = new System.Drawing.Point(12, 311);
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.Size = new System.Drawing.Size(75, 23);
            this.buttonExit.TabIndex = 7;
            this.buttonExit.Text = "Exit";
            this.buttonExit.UseVisualStyleBackColor = true;
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
            // 
            // buttonConnectDatabase
            // 
            this.buttonConnectDatabase.Location = new System.Drawing.Point(607, 311);
            this.buttonConnectDatabase.Name = "buttonConnectDatabase";
            this.buttonConnectDatabase.Size = new System.Drawing.Size(75, 23);
            this.buttonConnectDatabase.TabIndex = 8;
            this.buttonConnectDatabase.Text = "Generate...";
            this.buttonConnectDatabase.UseVisualStyleBackColor = true;
            this.buttonConnectDatabase.Click += new System.EventHandler(this.buttonConnectDatabase_Click);
            // 
            // textBoxProvider
            // 
            this.textBoxProvider.Location = new System.Drawing.Point(77, 50);
            this.textBoxProvider.Name = "textBoxProvider";
            this.textBoxProvider.Size = new System.Drawing.Size(462, 20);
            this.textBoxProvider.TabIndex = 11;
            this.textBoxProvider.Text = "Microsoft.ACE.OLEDB.12.0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(24, 53);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(49, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Provider:";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.textBoxDataBaseFile);
            this.groupBox2.Controls.Add(this.textBoxProvider);
            this.groupBox2.Location = new System.Drawing.Point(21, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(661, 100);
            this.groupBox2.TabIndex = 13;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Database Setup";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.buttonChooseTargetDir);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.textBoxTargetDir);
            this.groupBox1.Location = new System.Drawing.Point(21, 120);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(661, 156);
            this.groupBox1.TabIndex = 14;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Creation Settings";
            // 
            // buttonChooseTargetDir
            // 
            this.buttonChooseTargetDir.Location = new System.Drawing.Point(544, 35);
            this.buttonChooseTargetDir.Name = "buttonChooseTargetDir";
            this.buttonChooseTargetDir.Size = new System.Drawing.Size(37, 23);
            this.buttonChooseTargetDir.TabIndex = 2;
            this.buttonChooseTargetDir.Text = "...";
            this.buttonChooseTargetDir.UseVisualStyleBackColor = true;
            this.buttonChooseTargetDir.Click += new System.EventHandler(this.buttonChooseTargetDir_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(27, 39);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(86, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Target Directory:";
            // 
            // textBoxTargetDir
            // 
            this.textBoxTargetDir.Location = new System.Drawing.Point(115, 36);
            this.textBoxTargetDir.Name = "textBoxTargetDir";
            this.textBoxTargetDir.Size = new System.Drawing.Size(424, 20);
            this.textBoxTargetDir.TabIndex = 0;
            this.textBoxTargetDir.Text = "output/";
            // 
            // folderBrowserDialog1
            // 
            this.folderBrowserDialog1.HelpRequest += new System.EventHandler(this.folderBrowserDialog1_HelpRequest);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(703, 370);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.buttonExit);
            this.Controls.Add(this.buttonConnectDatabase);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "OpenGlobe SDK: Documentation and Header Generator";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxDataBaseFile;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.Button buttonConnectDatabase;
        private System.Windows.Forms.TextBox textBoxProvider;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button buttonChooseTargetDir;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxTargetDir;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
    }
}