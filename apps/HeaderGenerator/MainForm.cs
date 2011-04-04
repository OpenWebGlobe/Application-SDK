using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data.OleDb;

namespace HeaderGenerator
{

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------

    public partial class MainForm : Form
    {
        public static CodeGenerator cg = new CodeGenerator();

        //---------------------------------------------------------------------

        public MainForm()
        {
            InitializeComponent();
        }

        //---------------------------------------------------------------------

        private void MainForm_Load(object sender, EventArgs e)
        {
            cg.LoadAppData();
            textBoxDataBaseFile.Text = cg.oAppData.DbFile;
            textBoxProvider.Text = cg.oAppData.DbProvider;
            textBoxTargetDir.Text = cg.oAppData.destdir;

            if (cg.oAppData.autorun)
            {
                fillData();
                Close();
            }


        }

        //---------------------------------------------------------------------

        public void buttonConnectDatabase_Click(object sender, EventArgs e)
        {
            Console.WriteLine("Retrieving Data...");
            fillData();
        }


        //---------------------------------------------------------------------

        private string GetConnectionString()
        {
            string connectionstring = "Provider=";
            connectionstring += textBoxProvider.Text.ToString();
            connectionstring += ";Data Source=";
            connectionstring += textBoxDataBaseFile.Text.ToString();
            connectionstring += ";Persist Security Info=False;";

            return connectionstring;
        }

        //---------------------------------------------------------------------

        private void fillData()
        {
           
            

            OleDbConnection dbcon = new OleDbConnection(GetConnectionString());

            try
            {
                dbcon.Open();
                DBReader.SDKQuery(dbcon, cg);
                DBReader.ContributorQuery(dbcon, cg);
                DBReader.TopicQuery(dbcon, cg);
                DBReader.ConstantQuery(dbcon, cg);
                DBReader.DatatypeQuery(dbcon, cg);
                DBReader.FunctionQuery(dbcon, cg);
                dbcon.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }

            string directory = this.textBoxTargetDir.Text;

            if (directory.Length > 0)
            {
                string terminator = directory.Substring(directory.Length - 1, 1);
                if (terminator != "/" && terminator != "\\")
                {
                    directory += "\\";
                }

                if (!System.IO.Directory.Exists(directory))
                {
                    System.IO.Directory.CreateDirectory(directory);
                }
            }

            cg.oAppData.DbFile = textBoxDataBaseFile.Text;
            cg.oAppData.DbProvider = textBoxProvider.Text;
            cg.oAppData.destdir = textBoxTargetDir.Text;
            cg.SaveAppData();


            cg.GenerateAll(directory);

            Close();
           
        }

        private void buttonExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void buttonChooseTargetDir_Click(object sender, EventArgs e)
        {
            this.folderBrowserDialog1.ShowNewFolderButton = true;
            if (this.folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                this.textBoxTargetDir.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        private void textBoxDataBaseFile_TextChanged(object sender, EventArgs e)
        {

        }

        private void folderBrowserDialog1_HelpRequest(object sender, EventArgs e)
        {

        }
    }

   

   

}
