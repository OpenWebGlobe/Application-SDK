/*******************************************************************************
Project       : OpenGlobe SDK - Header and Documentaton Generator
Version       : 0.1 (PROTOTYPE)
Purpose       : Create headers and documentation of the OpenGlobe SDK
Last Modified : 8.9.2010
Author        : Martin Christen
Copyright     : (c) 2010 by FHNW/IVGI. All Rights Reserved
*******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.OleDb;
using System.Data;

namespace HeaderGenerator
{
    
    
    class Program
    {
        //---------------------------------------------------------------------

        [STAThreadAttribute]
        public static void Main(string[] args)
        {
            MainForm Form = new MainForm();
            Form.ShowDialog(); // show modal dialog
        }

        //---------------------------------------------------------------------

    } 

    
}


