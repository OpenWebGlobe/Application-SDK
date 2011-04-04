using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;

// Compiler info:
// http://predef.sourceforge.net/precomp.html


namespace HeaderGenerator
{
    public class AppData
    {
        public string DbFile = "T:\\A1334_IVGI_Projekte\\A1334_i3D\\10_Datenbanken\\OpenGlobe\\OpenGlobeSDK.accdb";
        public string DbProvider = "Microsoft.ACE.OLEDB.12.0";
        public string destdir = "output\\";
        public bool autorun = false;
    };

    public class CodeGenerator
    {
        enum Language {LangC, LangCSharp, LangVB, LangPython};

        public List<Topic> TopicList = new List<Topic>();
        public List<DataType> DataTypeList = new List<DataType>();
        public List<Constant> ConstantList = new List<Constant>();
        public List<Function> FunctionList = new List<Function>();
        public List<Contributor> ContributorList = new List<Contributor>();
        public SDK SDKInfo = new SDK();

        public AppData  oAppData = new AppData();

        public void LoadAppData()
        {
            try
            {
                if (File.Exists("config.xml"))
                {
                    XmlSerializer reader = new XmlSerializer(oAppData.GetType());
                    StreamReader file = new StreamReader("config.xml");
                    oAppData = (AppData)reader.Deserialize(file);
                    file.Close();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        public void SaveAppData()
        {
            try
            {
                XmlSerializer writer = new XmlSerializer(oAppData.GetType());
                StreamWriter file = new StreamWriter("config.xml");
                writer.Serialize(file, oAppData);
                file.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        //----------------------------------------------------------------------

        public void GenerateAll(string directory)
        {
            Console.WriteLine("Generating all headers...");
            GenerateCHeader(directory);
            GenerateCSharpHeader(directory);
            GenerateVisualBasicFile(directory);
            GeneratePythonFile(directory);
            GenerateDokuWikiFiles(directory);
            Console.WriteLine("Done.");
        }

        //----------------------------------------------------------------------

        public void GenerateCHeader(string directory)
        {
            try
            {
                TextWriter sout = new StreamWriter(directory + "openglobe.h");

                sout.WriteLine("/******************************************************************************");
                sout.WriteLine(SDKInfo.Name + " Version " + SDKInfo.Version + "\n");
                sout.WriteLine("C Header");
                sout.WriteLine(SDKInfo.Copyright);
                sout.WriteLine();
                sout.WriteLine("This file was automatically generated on " + DateTime.Now);
                sout.WriteLine("Generated using OpenGlobe Header and Documentation Generator Version 1.0.0");
                sout.WriteLine("******************************************************************************/\n\n");

                //output contributors
                sout.WriteLine("\n//##############################################################################");
                sout.WriteLine("// CONTRIBUTORS");
                sout.WriteLine("//##############################################################################\n");

                for (int i = 0; i < ContributorList.Count; i++)
                {
                    sout.WriteLine("// " + ContributorList[i].Name + ", " + ContributorList[i].Email + " (" + ContributorList[i].Role + ")"); 
                }


                sout.WriteLine("\n\n#ifdef _MSC_VER"); 
                sout.WriteLine("#ifndef OPENGLOBE_API"); 
                sout.WriteLine(" #ifdef BUILD_OPENGLOBELIB"); 
                sout.WriteLine(" #define OPENGLOBE_API __declspec(dllexport)"); 
                sout.WriteLine(" #else"); 
                sout.WriteLine(" #define OPENGLOBE_API __declspec(dllimport)"); 
                sout.WriteLine(" #endif"); 
                sout.WriteLine("#endif"); 
                sout.WriteLine("#else"); 
                sout.WriteLine("#define OPENGLOBE_API");
                sout.WriteLine("#endif"); 

                // output constants:
                string curTopic = "";
                sout.WriteLine("\n//##############################################################################");
                sout.WriteLine("// CONSTANTS");
                sout.WriteLine("//##############################################################################");
                for (int i = 0; i < ConstantList.Count; i++)
                {
                    if (curTopic != ConstantList[i].Topic.ToString())
                    {
                        curTopic = ConstantList[i].Topic.ToString();
                        sout.WriteLine("\n//------------------------------------------------------------------------------");
                        sout.WriteLine("// " + ConstantList[i].Topic.ToString());
                        sout.WriteLine("//------------------------------------------------------------------------------");
                    }
                    sout.WriteLine("#define " + ConstantList[i].Name.ToString() + " " + ConstantList[i].Value.ToString());
                }

                // output typedefs
                sout.WriteLine("\n//##############################################################################");
                sout.WriteLine("// TYPES");
                sout.WriteLine("//##############################################################################");
                //public int ID;
                //public string CType;
                //public string CTypeDef;
                for (int i = 0; i < DataTypeList.Count; i++)
                {
                    if (DataTypeList[i].CTypeDef.Length > 0)
                    {
                        sout.WriteLine(DataTypeList[i].CTypeDef + ";");
                    }
                }

                // output functions
                curTopic = "";
                sout.WriteLine("\n//##############################################################################");
                sout.WriteLine("// FUNCTIONS");
                sout.WriteLine("//##############################################################################\n");

                for (int i = 0; i < FunctionList.Count; i++)
                {
                    string sParams = "";

                    if (curTopic != FunctionList[i].Topic.ToString())
                    {
                        curTopic = FunctionList[i].Topic.ToString();
                        sout.WriteLine("//------------------------------------------------------------------------------");
                        sout.WriteLine("// " + FunctionList[i].Topic.ToString());
                        sout.WriteLine("//------------------------------------------------------------------------------\n");
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (j != 0)
                            {
                                sParams += ", ";
                            }
                            sParams += FunctionList[i].ParamType[j];
                            sParams += " ";
                            sParams += FunctionList[i].ParamName[j];
                        }
                    }

                    if (FunctionList[i].Brief.Length > 0)
                    {
                        sout.WriteLine("//! \\brief " + FunctionList[i].Brief);
                    }
                    if (FunctionList[i].Description.Length > 0)
                    {
                        sout.WriteLine("//! " + FunctionList[i].Description);
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (FunctionList[i].ParamDescription[j].Length > 0)
                            {
                                sout.WriteLine("//! \\param " + FunctionList[i].ParamName[j] + " " + FunctionList[i].ParamDescription[j]);
                            }
                        }
                    }
                    if (FunctionList[i].ReturnValueDescription.Length > 0)
                    {
                        sout.WriteLine("//! \\return " + FunctionList[i].ReturnValueDescription);
                    }
                    // write function
                    sout.WriteLine("OPENGLOBE_API " + FunctionList[i].ReturnValue + " " + FunctionList[i].FunctionName + "(" + sParams + ");");
                    sout.WriteLine();
                }

                sout.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        //----------------------------------------------------------------------

        public void GenerateCSharpHeader(string directory)
        {
            try
            {
                TextWriter sout = new StreamWriter(directory + "openglobe.cs");

                sout.WriteLine("/******************************************************************************");
                sout.WriteLine(SDKInfo.Name + " Version " + SDKInfo.Version + "\n");
                sout.WriteLine("C# Header");
                sout.WriteLine(SDKInfo.Copyright);
                sout.WriteLine();
                sout.WriteLine("This file was automatically generated on " + DateTime.Now);
                sout.WriteLine("Generated using OpenGlobe Header and Documentation Generator Version 1.0.0");
                sout.WriteLine("******************************************************************************/\n\n");
                sout.WriteLine("using System;");
                sout.WriteLine("using System.Runtime.InteropServices;\n"); // for dll import    

                //output contributors
                sout.WriteLine("\n//##############################################################################");
                sout.WriteLine("// CONTRIBUTORS");
                sout.WriteLine("//##############################################################################\n");

                for (int i = 0; i < ContributorList.Count; i++)
                {
                    sout.WriteLine("// " + ContributorList[i].Name + ", " + ContributorList[i].Email + " (" + ContributorList[i].Role + ")"); 
                }

                // class OpenGlobe
                sout.WriteLine("\n\nclass OpenGlobe");
                sout.WriteLine("{");


                // output constants:
                string curTopic = "";
                sout.WriteLine("\n   //###########################################################################");
                sout.WriteLine("   // CONSTANTS");
                sout.WriteLine("   //###########################################################################");
                for (int i = 0; i < ConstantList.Count; i++)
                {
                    if (curTopic != ConstantList[i].Topic.ToString())
                    {
                        curTopic = ConstantList[i].Topic.ToString();
                        sout.WriteLine("\n   //---------------------------------------------------------------------------");
                        sout.WriteLine("   // " + ConstantList[i].Topic.ToString());
                        sout.WriteLine("   //---------------------------------------------------------------------------");
                    }

                    sout.WriteLine("   public const System.Int32 " + ConstantList[i].Name.ToString() + " = " + ConstantList[i].Value.ToString() + ";");
                   
                }

                // output typedefs
                sout.WriteLine("\n   //###########################################################################");
                sout.WriteLine("   // TYPES");
                sout.WriteLine("   //###########################################################################\n");
                for (int i = 0; i < DataTypeList.Count; i++)
                {
                    if (DataTypeList[i].VBTypeDef.Length > 0)
                    {
                        sout.WriteLine("   [UnmanagedFunctionPointer(CallingConvention.Cdecl)]");
                        sout.WriteLine("   " + DataTypeList[i].CSTypeDef + ";\n");
                    }
                }

                // output functions
                curTopic = "";
                sout.WriteLine("\n   //###########################################################################");
                sout.WriteLine("   // FUNCTIONS");
                sout.WriteLine("   //###########################################################################\n");

                for (int i = 0; i < FunctionList.Count; i++)
                {
                    string sParams = "";

                    if (curTopic != FunctionList[i].Topic.ToString())
                    {
                        curTopic = FunctionList[i].Topic.ToString();
                        sout.WriteLine("   //---------------------------------------------------------------------------");
                        sout.WriteLine("   // " + FunctionList[i].Topic.ToString());
                        sout.WriteLine("   //---------------------------------------------------------------------------\n");
                    }

                    int nParams = 0;
                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            nParams++;
                        }
                    }

                    if (nParams > 1)
                    {
                        sParams = "\n      ";
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            bool isRef = false;
                            // As Reference ?
                            if (FunctionList[i].ParamTypeCS[j].Count() > 0)
                            {
                                if (FunctionList[i].ParamTypeCS[j][0] == '#')
                                {
                                    isRef = true;
                                }
                            }

                            if (j != 0)
                            {
                                sParams += ",\n      ";
                            }

                            if (isRef)
                            {
                                sParams += "ref ";
                                for (int k = 1; k < FunctionList[i].ParamTypeCS[j].Count(); k++)
                                {
                                    sParams += FunctionList[i].ParamTypeCS[j][k];
                                }
                            }
                            else
                            {
                                sParams += FunctionList[i].ParamTypeCS[j];
                            }
                            sParams += " ";
                            sParams += FunctionList[i].ParamName[j];
                        }
                    }

                    if (FunctionList[i].Brief.Length > 0)
                    {
                        sout.WriteLine("   //! \\brief " + FunctionList[i].Brief);
                    }
                    if (FunctionList[i].Description.Length > 0)
                    {
                        sout.WriteLine("   //! " + FunctionList[i].Description);
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (FunctionList[i].ParamDescription[j].Length > 0)
                            {
                                sout.WriteLine("   //! \\param " + FunctionList[i].ParamName[j] + " " + FunctionList[i].ParamDescription[j]);
                            }
                        }
                    }
                    if (FunctionList[i].ReturnValueDescription.Length > 0)
                    {
                        sout.WriteLine("   //! \\return " + FunctionList[i].ReturnValueDescription);
                    }
                    // write function
                    sout.WriteLine("   [DllImport(\"OpenGlobe.dll\", CallingConvention = CallingConvention.Cdecl)]");
                    sout.WriteLine("   public static extern " + FunctionList[i].ReturnValueCS + " " + FunctionList[i].FunctionName + "(" + sParams + ");");
                    sout.WriteLine();
                }

                // end class
                sout.WriteLine("}");

                sout.Close();
        
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        //----------------------------------------------------------------------

        public void GenerateVisualBasicFile(string directory)
        {
            try
            {
                TextWriter sout = new StreamWriter(directory + "openglobe.vb");

                sout.WriteLine("'*******************************************************************************");
                sout.WriteLine("'"+ SDKInfo.Name + " Version " + SDKInfo.Version);
                sout.WriteLine("'");
                sout.WriteLine("'Visual Basic.NET 2010 Header");
                sout.WriteLine("' " + SDKInfo.Copyright);
                sout.WriteLine("'");
                sout.WriteLine("'This file was automatically generated on " + DateTime.Now);
                sout.WriteLine("'Generated using OpenGlobe Header and Documentation Generator Version 1.0.0");
                sout.WriteLine("'*******************************************************************************\n\n");

                //output contributors
                sout.WriteLine("\n'###############################################################################");
                sout.WriteLine("' CONTRIBUTORS");
                sout.WriteLine("'###############################################################################\n");

                for (int i = 0; i < ContributorList.Count; i++)
                {
                    sout.WriteLine("' " + ContributorList[i].Name + ", " + ContributorList[i].Email + " (" + ContributorList[i].Role + ")");
                }

                sout.WriteLine("'-------------------------------------------------------------------------------\n");


                sout.WriteLine("\nImports System.Runtime.InteropServices");
                sout.WriteLine();
                // START MODULE
                sout.WriteLine("Module OpenGlobe");


                // output constants:
                string curTopic = "";
                sout.WriteLine("\n   '############################################################################");
                sout.WriteLine("   ' CONSTANTS");
                sout.WriteLine("   '############################################################################");
                for (int i = 0; i < ConstantList.Count; i++)
                {
                    if (curTopic != ConstantList[i].Topic.ToString())
                    {
                        curTopic = ConstantList[i].Topic.ToString();
                        sout.WriteLine("\n   '----------------------------------------------------------------------------");
                        sout.WriteLine("   ' " + ConstantList[i].Topic.ToString());
                        sout.WriteLine("   '----------------------------------------------------------------------------");
                    }

                    sout.WriteLine("   Public Const " + ConstantList[i].Name.ToString() + " As System.UInt32 = " +  ConstantList[i].Value.ToString());
                }


                // output typedefs
                sout.WriteLine("\n   '############################################################################");
                sout.WriteLine("   ' TYPES");
                sout.WriteLine("   '############################################################################\n");
                //public int ID;
                //public string CType;
                //public string CTypeDef;
                for (int i = 0; i < DataTypeList.Count; i++)
                {
                    if (DataTypeList[i].VBTypeDef.Length > 0)
                    {
                        sout.WriteLine("   <UnmanagedFunctionPointer(CallingConvention.Cdecl)> _");
                        sout.WriteLine("   " + DataTypeList[i].VBTypeDef + "\n");
                    }
                }

                // output functions
                curTopic = "";
                sout.WriteLine("\n   '############################################################################");
                sout.WriteLine("   ' FUNCTIONS");
                sout.WriteLine("   '############################################################################\n");

                for (int i = 0; i < FunctionList.Count; i++)
                {
                    string sParams = "";

                    if (curTopic != FunctionList[i].Topic.ToString())
                    {
                        curTopic = FunctionList[i].Topic.ToString();
                        sout.WriteLine("   '----------------------------------------------------------------------------");
                        sout.WriteLine("   ' " + FunctionList[i].Topic.ToString());
                        sout.WriteLine("   '----------------------------------------------------------------------------\n");
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (j != 0)
                            {
                                sParams += ", _\n";
                            }
                            sParams += "      ";

                            bool isRef = false;
                            // As Reference ?
                            if (FunctionList[i].ParamTypeVB[j].Count() > 0)
                            {
                                if (FunctionList[i].ParamTypeVB[j][0] == '#')
                                {
                                    isRef = true;
                                }

                                
                                if (FunctionList[i].ParamTypeVB[j] == "System.String")
                                {
                                    sParams += "<MarshalAs(UnmanagedType.LPStr)> ";
                                }
                                else if (FunctionList[i].ParamTypeVB[j].Substring(0,8) == "CallBack")
                                {
                                    sParams += "<MarshalAs(UnmanagedType.FunctionPtr)> ";
                                }

                          
                                if (isRef)
                                {
                                    sParams += "ByRef ";
                                }
                                else
                                {
                                    sParams += "ByVal ";
                                }
                                sParams += FunctionList[i].ParamName[j] + " As ";

                                if (isRef)
                                {
                                    // ignore # (Reference-Symbol) at begin of string...
                                    for (int k = 1; k < FunctionList[i].ParamTypeVB[j].Count(); k++)
                                    {
                                        sParams += FunctionList[i].ParamTypeVB[j][k];
                                    }
                                }
                                else
                                {
                                    sParams += FunctionList[i].ParamTypeVB[j];
                                }
                            }
                        }
                    }

                    if (FunctionList[i].Brief.Length > 0)
                    {
                        sout.WriteLine("   '* \\brief " + FunctionList[i].Brief);
                    }
                    if (FunctionList[i].Description.Length > 0)
                    {
                        sout.WriteLine("   '* " + FunctionList[i].Description);
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (FunctionList[i].ParamDescription[j].Length > 0)
                            {
                               sout.WriteLine("   '* \\param " + FunctionList[i].ParamName[j] + " " + FunctionList[i].ParamDescription[j]);
                            }
                        }
                    }
                    if (FunctionList[i].ReturnValueDescription.Length > 0)
                    {
                       sout.WriteLine("   '* \\return " + FunctionList[i].ReturnValueDescription);
                    }
                    // write sub or function
                    bool IsFunction = false;

                    if (FunctionList[i].ReturnValueVB == "None")
                    {
                        IsFunction = true;
                    }

                    sout.WriteLine("   <DllImport(\"openglobe.dll\", CallingConvention:=CallingConvention.Cdecl)> _");
                    if (IsFunction)
                    {
                        if (sParams.Length == 0)
                        {
                            sout.WriteLine("   Public Function " + FunctionList[i].FunctionName + "()");

                            sout.WriteLine("   End Function");
                        }
                        else
                        {
                            sout.WriteLine("   Public Function " + FunctionList[i].FunctionName + "( _");
                            sout.WriteLine(sParams + ")");
                            sout.WriteLine("   End Function");
                        }
                    }
                    else
                    {
                        if (sParams.Length == 0)
                        {
                            sout.WriteLine("   Public Sub " + FunctionList[i].FunctionName + "()");
                            sout.WriteLine(" As " + FunctionList[i].ReturnValueVB);
                            sout.WriteLine("   End Sub");
                        }
                        else
                        {
                            sout.WriteLine("   Public Sub " + FunctionList[i].FunctionName + "( _");
                            sout.WriteLine(sParams + ")" + " As " + FunctionList[i].ReturnValueVB);
                            sout.WriteLine("   End Sub");
                        }
                    }

                      sout.WriteLine();
                }




                // END MODULE
                sout.WriteLine("End Module");
                sout.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }
        
        //----------------------------------------------------------------------

        public void GeneratePythonFile(string directory)
        {
            try
            {
                TextWriter sout = new StreamWriter(directory + "openglobe.py");

                sout.WriteLine("#*******************************************************************************");
                sout.WriteLine("# coding=utf-8");
                sout.WriteLine("# " + SDKInfo.Name + " Version " + SDKInfo.Version);
                sout.WriteLine("#");
                sout.WriteLine("# Python 2.6, 3.x Header");
                sout.WriteLine("# " + SDKInfo.Copyright);
                sout.WriteLine("#");
                sout.WriteLine("#This file was automatically generated on " + DateTime.Now);
                sout.WriteLine("#Generated using OpenGlobe Header and Documentation Generator Version 1.0.0");
                sout.WriteLine("#*******************************************************************************\n\n");

                //output contributors
                sout.WriteLine("\n################################################################################");
                sout.WriteLine("# CONTRIBUTORS");
                sout.WriteLine("################################################################################\n");

                for (int i = 0; i < ContributorList.Count; i++)
                {
                    sout.WriteLine("# " + ContributorList[i].Name + ", " + ContributorList[i].Email + " (" + ContributorList[i].Role + ")");
                }

                sout.WriteLine("\n#-------------------------------------------------------------------------------\n");

                sout.WriteLine("from ctypes import *");
                sout.WriteLine("\nog = cdll.openglobe");

                // output constants:
                string curTopic = "";
                sout.WriteLine("\n################################################################################");
                sout.WriteLine("# CONSTANTS");
                sout.WriteLine("################################################################################");
                for (int i = 0; i < ConstantList.Count; i++)
                {
                    if (curTopic != ConstantList[i].Topic.ToString())
                    {
                        curTopic = ConstantList[i].Topic.ToString();
                        sout.WriteLine("\n#-------------------------------------------------------------------------------");
                        sout.WriteLine("# " + ConstantList[i].Topic.ToString());
                        sout.WriteLine("#-------------------------------------------------------------------------------");
                    }

                    sout.WriteLine(ConstantList[i].Name.ToString() + " = " + ConstantList[i].Value.ToString());
                }


                // output typedefs
                sout.WriteLine("\n################################################################################");
                sout.WriteLine("# TYPES");
                sout.WriteLine("################################################################################\n");
                //public int ID;
                //public string CType;
                //public string CTypeDef;
                for (int i = 0; i < DataTypeList.Count; i++)
                {
                    if (DataTypeList[i].PythonTypeDef.Length > 0)
                    {
                        sout.WriteLine(DataTypeList[i].PythonTypeDef);
                    }
                }


                // output functions
                curTopic = "";
                sout.WriteLine("\n################################################################################");
                sout.WriteLine("# FUNCTIONS");
                sout.WriteLine("################################################################################\n");

                for (int i = 0; i < FunctionList.Count; i++)
                {
                    string sParams = "";
                    string sParamNames = "";

                    if (curTopic != FunctionList[i].Topic.ToString())
                    {
                        curTopic = FunctionList[i].Topic.ToString();
                        sout.WriteLine("#-------------------------------------------------------------------------------");
                        sout.WriteLine("# " + FunctionList[i].Topic.ToString());
                        sout.WriteLine("#-------------------------------------------------------------------------------\n");
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                           
                            if (j != 0)
                            {
                                sParams += ", ";
                                sParamNames += ", ";
                            }

                            sParamNames += FunctionList[i].ParamName[j].ToString();

                            bool isCallback = false;

                            if (FunctionList[i].ParamTypePY[j].Length>=8 && FunctionList[i].ParamTypePY[j].Substring(0, 8) == "CallBack")
                            {
                                isCallback = true;
                            }
                            

                            bool isRef = false;
                            // As Reference ?
                            if (FunctionList[i].ParamTypePY[j].Count() > 0)
                            {
                                if (FunctionList[i].ParamTypePY[j][0] == '#')
                                {
                                    isRef = true;
                                }

                                if (isRef)
                                {
                                    sParams += "byref(";
                                }


                                if (!isCallback)
                                {
                                    if (isRef)
                                    {
                                        // ignore # (Reference-Symbol) at begin of string...
                                        for (int k = 1; k < FunctionList[i].ParamTypePY[j].Count(); k++)
                                        {
                                            sParams += FunctionList[i].ParamTypePY[j][k];
                                        }
                                    }
                                    else
                                    {
                                        sParams += FunctionList[i].ParamTypePY[j];
                                    }
                                }

                                if (isCallback)
                                {
                                    sParams += FunctionList[i].ParamName[j];
                                }
                                else
                                {
                                    sParams += "(" + FunctionList[i].ParamName[j] + ")";
                                }
                                if (isRef)
                                {
                                    sParams += ")";
                                }
                            }
                        }
                    }

                    if (FunctionList[i].Brief.Length > 0)
                    {
                        sout.WriteLine("## \\brief " + FunctionList[i].Brief);
                    }
                    if (FunctionList[i].Description.Length > 0)
                    {
                        sout.WriteLine("## " + FunctionList[i].Description);
                    }

                    for (int j = 0; j < Function.MaxParam; j++)
                    {
                        if (FunctionList[i].HasParam[j])
                        {
                            if (FunctionList[i].ParamDescription[j].Length > 0)
                            {
                                sout.WriteLine("## \\param " + FunctionList[i].ParamName[j] + " " + FunctionList[i].ParamDescription[j]);
                            }
                        }
                    }
                    if (FunctionList[i].ReturnValueDescription.Length > 0)
                    {
                        sout.WriteLine("## \\return " + FunctionList[i].ReturnValueDescription);
                    }

                    sout.WriteLine("def " + FunctionList[i].FunctionName + "(" + sParamNames.ToString() + ") :");
                    sout.WriteLine("    return og." + FunctionList[i].FunctionName + "(" + sParams.ToString() + ")");
          
                    sout.WriteLine();
                }



                sout.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        //----------------------------------------------------------------------

        public void GenerateDokuWikiFiles(string directory)
        {
            try
            {
                //TextWriter sout = new StreamWriter(directory + "openglobe.py");
                Directory.CreateDirectory(directory + "sdk");
                Directory.CreateDirectory(directory + "sdk\\functions\\files");

                Directory.CreateDirectory(directory + "sdk\\constants\\files");

                Directory.CreateDirectory(directory + "sdk\\topics\\files");
                Directory.CreateDirectory(directory + "sdk\\topics\\files\\c");
                Directory.CreateDirectory(directory + "sdk\\topics\\files\\csharp");
                Directory.CreateDirectory(directory + "sdk\\topics\\files\\visualbasic");
                Directory.CreateDirectory(directory + "sdk\\topics\\files\\python");

                //*****************************Generate Function Files*******************************************

                //Create all Function files
                GenerateFunctionFiles(directory + "sdk\\functions\\files\\");


                //*****Generate the Function Overview******
                GenerateFunctionOverviewFiles(Language.LangC, directory + "\\sdk\\functions\\");
                GenerateFunctionOverviewFiles(Language.LangCSharp, directory + "\\sdk\\functions\\");
                GenerateFunctionOverviewFiles(Language.LangVB, directory + "\\sdk\\functions\\");
                GenerateFunctionOverviewFiles(Language.LangPython, directory + "\\sdk\\functions\\");

                TextWriter sout = new StreamWriter(directory + "sdk\\functions\\" + "overview.txt");
                sout.WriteLine("[float right background=#eeeeff size=12 width=180] **All Functions of OpenWebGlobeSDK.** [/float]");
                sout.WriteLine("====== Functions ======");
                sout.WriteLine("{{tabinclude>c,csharp,vb,python}}");
                sout.Close();

                //*****************************Generate Constants Files*******************************************

                GenerateConstantsFiles(directory + "sdk\\constants\\files\\");
                GenerateConstantsOverviewFiles(directory + "sdk\\constants\\");


                //*****************************Generate Topics Files*******************************************

                GenerateTopicsFiles(directory + "sdk\\topics\\files\\");

                GenerateTopicsOverviewFiles(directory + "sdk\\topics\\");

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        //----------------------------------------------------------------------

        //Returns the General name of the dataType submited.
        public string GetDataTypeName(string yourDataType) 
        {
            string retVal = null;
            for (int i = 0; i < DataTypeList.Count; i++)
            {
                if (yourDataType == DataTypeList[i].CType || yourDataType == DataTypeList[i].CSType ||
                    yourDataType == DataTypeList[i].VBType || yourDataType == DataTypeList[i].PythonType)
                {
                    retVal = DataTypeList[i].DataTypeName;
                }
            }
            return retVal;
        }

        //Generates dokuwiki Files for every Programming language (defined in enum Language) of every function in the specified directory
        void GenerateFunctionFiles(string directory) 
        {
            for (int i = 0; i < FunctionList.Count; i++)
            {
                System.String str = FunctionList[i].FunctionName;
                str = str.ToLower();
                Directory.CreateDirectory(directory + str);
                TextWriter sout = new StreamWriter(directory + str + "\\" + str + ".txt");
                sout.WriteLine("[float right background=#eeeeff size=12 width=180]");
                sout.WriteLine("**" + FunctionList[i].Brief + "**");
                sout.WriteLine("[/float]");
                //sout.WriteLine("====== " + language + " ======");

                sout.WriteLine("==== " + FunctionList[i].Topic + "::" + FunctionList[i].FunctionName + "====");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("{{tabinclude>c,csharp,vb,python}}");
                sout.Close();

                Language lang = 0;

                for (int j = 0; j < 4; j++) 
                {
                    string returnValue = null;
                    string[] paramType = null;
                    string language = null;
                    string filename = null;
                    switch (lang)
                    {
                        case Language.LangC:
                            returnValue = FunctionList[i].ReturnValue;
                            paramType = FunctionList[i].ParamType;
                            language = "C/C++";
                            filename = "c.txt";
                            break;
                        case Language.LangCSharp:
                            returnValue = FunctionList[i].ReturnValueCS;
                            paramType = FunctionList[i].ParamTypeCS;
                            language = "C#";
                            filename = "csharp.txt";
                            break;
                        case Language.LangVB:
                            returnValue = FunctionList[i].ReturnValueVB;
                            paramType = FunctionList[i].ParamTypeVB;
                            language = "Visual Basic";
                            filename = "vb.txt";
                            break;
                        case Language.LangPython:
                            returnValue = FunctionList[i].ReturnValuePY;
                            paramType = FunctionList[i].ParamTypePY;
                            language = "Python";
                            filename = "python.txt";
                            break;
                    }
                    TextWriter sout1 = new StreamWriter(directory + str + "\\" + filename);
                    sout1.WriteLine("====== " + language + " ======");
                    sout1.Write("**<fc #FF0000>" + returnValue + "</fc> " + FunctionList[i].FunctionName + "(");
                    int parameterCounter = 0;
                    for (int k = 0; k < FunctionList[i].ParamName.Length; k++)
                    {
                        if (!(paramType[k] == null))
                        {
                            if (k > 0)
                            {
                                sout1.Write(", ");
                            }
                            sout1.Write(paramType[k] + "<fc #0000FF> " + FunctionList[i].ParamName[k] + "</fc>");
                            parameterCounter++;
                        }
                    }
                    sout1.Write(")**\n");

                    sout1.WriteLine("  * Description: %%" + FunctionList[i].Description + "%%");
                    string retvalType = GetDataTypeName(returnValue);
                    sout1.WriteLine("  * <fc #FF0000>Return value</fc>: " + retvalType + ", %%" + FunctionList[i].ReturnValueDescription + "%%");
                    for (int l = 0; l < parameterCounter; l++)
                    {
                        string parameterType = GetDataTypeName(paramType[l]);
                        sout1.WriteLine("  * <fc #0000FF>Parameter " + (l + 1) + "</fc>: " + paramType + ", <fc #0000FF>" + FunctionList[i].ParamName[l] + "</fc>, %%" + FunctionList[i].ParamDescription[l] + "%%");
                    }
                    sout1.Close();
                    lang++;
                }

            }
        }

        //Generates dokuwiki Files for the specified Programming language. Creates an overview table
        void GenerateFunctionOverviewFiles(Language lang, string directory, string topic = null) 
        {
            string language = null;
            string filename = null;
            string filenameWithoutEnding = null;
            string wikiNamespace = "sdk:functions:files:";
            switch (lang)
            {
                case Language.LangC:
                    language = "C/C++";
                    filename = "c.txt";
                    filenameWithoutEnding = "c";
                    break;
                case Language.LangCSharp:
                    language = "C#";
                    filename = "csharp.txt";
                    filenameWithoutEnding = "csharp";
                    break;
                case Language.LangVB:
                    language = "Visual Basic";
                    filename = "vb.txt";
                    filenameWithoutEnding = "vb";
                    break;
                case Language.LangPython:
                    language = "Python";
                    filename = "python.txt";
                    filenameWithoutEnding = "python";
                    break;
            }

            TextWriter sout = new StreamWriter(directory + filename);
            if (topic == null)
            {
                sout.WriteLine("====== " + language + " ======");
            }    
            sout.WriteLine("^ Functions ^ Description ^");

            for (int i = 0; i < FunctionList.Count; i++)
            {
                string returnValue = null;
                string[] paramType = null;
                switch (lang)
                {
                    case Language.LangC:
                        returnValue = FunctionList[i].ReturnValue;
                        paramType = FunctionList[i].ParamType;
                        break;
                    case Language.LangCSharp:
                        returnValue = FunctionList[i].ReturnValueCS;
                        paramType = FunctionList[i].ParamTypeCS;
                        break;
                    case Language.LangVB:
                        returnValue = FunctionList[i].ReturnValueVB;
                        paramType = FunctionList[i].ParamTypeVB;
                        break;
                    case Language.LangPython:
                        returnValue = FunctionList[i].ReturnValuePY;
                        paramType = FunctionList[i].ParamTypePY;
                        break;
                }
                if (topic == null || FunctionList[i].Topic == topic)
                {
                    sout.Write("| " + returnValue + " [[" + wikiNamespace + FunctionList[i].FunctionName.ToLower() + ":" + filenameWithoutEnding + "|" + FunctionList[i].FunctionName + "]](");
                    for (int j = 0; j < FunctionList[i].ParamName.Length; j++)
                    {
                        if (!(paramType[j] == null))
                        {
                            if (j > 0)
                            {
                                sout.Write(", ");
                            }
                            sout.Write(paramType[j] + " " + FunctionList[i].ParamName[j]);
                        }
                    }
                    sout.Write("); | %%");
                    sout.WriteLine(FunctionList[i].Description + "%% |");
                }
            }
            sout.Close();
        }

        //Generates dokuwiki Files for the specified Programming language of every Constant in the specified directory
        void GenerateConstantsFiles(string directory) 
        {
            for (int i = 0; i < ConstantList.Count; i++)
            {
                string path = directory + ConstantList[i].Name.ToLower();
                Directory.CreateDirectory(path);

                System.String str = ConstantList[i].Name.ToLower();
                TextWriter sout = new StreamWriter(path + "\\" + str + ".txt");

                sout.WriteLine("==== " + ConstantList[i].Topic + "::" + ConstantList[i].Name + "====");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("**<fc #00FFFF>" + ConstantList[i].Name + "</fc>**");

                sout.WriteLine("  * Description: %%" + ConstantList[i].Description + "%%");
                sout.WriteLine("  * Value: " + ConstantList[i].Value);
                sout.Close();
            }
        }

        //Generates dokuwiki Files for the specified Programming language. Creates an overview table
        void GenerateConstantsOverviewFiles(string directory, string topic = null)
        {
            string wikiNamespace = "sdk:constants:files:";

            TextWriter sout = new StreamWriter(directory + "overview.txt");
            if (topic == null) 
            {
                sout.WriteLine("====== " + "Constants" + " ======");
            }            
            sout.WriteLine("^ Constants ^ Value ^ Description ^");

            for (int i = 0; i < ConstantList.Count; i++)
            {
                if (topic == null || ConstantList[i].Topic == topic)
                {
                    sout.Write("| " + " [[" + wikiNamespace + ConstantList[i].Name.ToLower() + ":" + ConstantList[i].Name.ToLower() + "|" + ConstantList[i].Name + "]]");
                    sout.Write(" | " + ConstantList[i].Value);
                    sout.Write(" | %%");
                    sout.WriteLine(ConstantList[i].Description + "%% |");
                }
                
            }
            sout.Close();
        }

        //Generates dokuwiki Files for every Programming language (defined in enum Language) of every topic in the specified directory
        void GenerateTopicsFiles(string directory)
        {
            for (int i = 0; i < TopicList.Count; i++)
            {
                System.String str = TopicList[i].Name;
                str = str.Replace(" ", "").ToLower();
                Directory.CreateDirectory(directory + str);
                TextWriter sout = new StreamWriter(directory + str + "\\" + str + ".txt");
                sout.WriteLine("[float right background=#eeeeff size=12 width=180]");
                sout.WriteLine("**" + FunctionList[i].Brief + "**");
                sout.WriteLine("[/float]");
                //sout.WriteLine("====== " + language + " ======");

                sout.WriteLine("==== " + TopicList[i].Name + " ====");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("\\\\ ");
                sout.WriteLine("{{tabinclude>c,csharp,vb,python}}");
                sout.Close();

                Language lang = 0;

                for (int j = 0; j < 4; j++) 
                {
                    string language = null;
                    string filename = null;
                    switch (lang)
                    {
                        case Language.LangC:
                            language = "C/C++";
                            filename = "c.txt";
                            break;
                        case Language.LangCSharp:
                            language = "C#";
                            filename = "csharp.txt";
                            break;
                        case Language.LangVB:
                            language = "Visual Basic";
                            filename = "vb.txt";
                            break;
                        case Language.LangPython:
                            language = "Python";
                            filename = "python.txt";
                            break;
                    }
                    GenerateFunctionOverviewFiles(lang, directory + str + "\\", TopicList[i].Name);
                    TextReader read = new StreamReader(directory + str + "\\" + filename);
                    string functions = read.ReadToEnd();
                    read.Close();

                    GenerateConstantsOverviewFiles(directory + str + "\\", TopicList[i].Name);
                    TextReader read2 = new StreamReader(directory + str + "\\" + "overview.txt");
                    string constants = read2.ReadToEnd();
                    read2.Close();
                    File.Delete(directory + str + "\\" + "overview.txt");

                    TextWriter sout1 = new StreamWriter(directory + str + "\\" + filename);
                    sout1.WriteLine("\\\\ ");
                    sout1.WriteLine("[float right background=#eeeeff size=12 width=180]");
                    sout1.WriteLine("**" + TopicList[i].Description + "**");
                    sout1.WriteLine("[/float]");

                    sout1.WriteLine("====== " + language + " ======");
                    sout1.WriteLine("\\\\ ");
                    sout1.WriteLine("\\\\ ");

                    sout1.Write(functions);
                    sout1.WriteLine("");
                    sout1.Write(constants);

                    sout1.Close();
                    
                    lang++;
                }
            }
        }

        //Generates dokuwiki Files for a topic overview in the specified directory
        void GenerateTopicsOverviewFiles(string directory)
        {
            string wikiNamespace = "sdk:topics:files:";

            TextWriter sout = new StreamWriter(directory + "overview.txt");
            sout.WriteLine("[float right background=#eeeeff size=12 width=180]");
            sout.WriteLine("** Functions and Constants sorted by Topic **");
            sout.WriteLine("[/float]");
            sout.WriteLine("====== Topics ======");
           
            sout.WriteLine("^ Topic ^ Description ^");

            for (int i = 0; i < TopicList.Count; i++)
            {
                sout.Write("| " + " [[" + wikiNamespace + TopicList[i].Name.Replace(" ", "").ToLower() + ":" + TopicList[i].Name.Replace(" ", "").ToLower() + "|" + TopicList[i].Name + "]]");
                sout.Write(" | %%");
                sout.WriteLine(TopicList[i].Description + "%% |");
            }
            sout.Close();
        }

    }

    //--------------------------------------------------------------------------

    public class Topic
    {
        public int ID;
        public string Name;
        public string Description;

        public void Print()
        {
            Console.WriteLine(Name);
        }
    }

    //--------------------------------------------------------------------------

    public class DataType
    {
        public int ID;
        public string DataTypeName;
        public string CType;
        public string CTypeDef;
        public string CSType;
        public string CSTypeDef;
        public string VBType;
        public string VBTypeDef;
        public string PythonType;
        public string PythonTypeDef;
    }

    //--------------------------------------------------------------------------

    public class Constant
    {
        public int ID;
        public string Name;
        public string Value;
        public string Topic;
        public string Description;

        public void Print()
        {
            Console.WriteLine("#define " + Name + " " + Value + "(" + Topic + ")");
        }
    }

    //--------------------------------------------------------------------------

    public class Function
    {
        public void Print()
        {
            string sParams = "";

            for (int i = 0; i < MaxParam; i++)
            {
                if (HasParam[i])
                {
                    if (i != 0)
                    {
                        sParams += ", ";
                    }
                    sParams += ParamType[i];
                    sParams += " ";
                    sParams += ParamName[i];
                }
            }

            Console.WriteLine(ReturnValue + " " + FunctionName + "(" + sParams + ")");
        }

        public int ID;
        public string FunctionName;
        public string Brief;
        public string Description;
        public string ReturnValue;
        public string ReturnValueCS;
        public string ReturnValueVB;
        public string ReturnValuePY;
        public string ReturnValueDescription;
        public string Topic;

        public const uint MaxParam = 8; // max number of params, currently set to 8!
        public string[] ParamName = new string[MaxParam];
        public string[] ParamType = new string[MaxParam];
        public string[] ParamTypeCS = new string[MaxParam];
        public string[] ParamTypeVB = new string[MaxParam];
        public string[] ParamTypePY = new string[MaxParam];
        public string[] ParamDescription = new string[MaxParam];
        public bool[] HasParam = new bool[MaxParam];
    }

    //--------------------------------------------------------------------------

    public class SDK
    {
        public string Name;
        public string Version;
        public string Copyright;

        public void Print()
        {
            Console.WriteLine("/******************************************************************************");
            Console.WriteLine(Name + " Version " + Version + "\n");
            Console.WriteLine(Copyright);
            Console.WriteLine("******************************************************************************/\n\n");
        }
    }

    //--------------------------------------------------------------------------

    public class Contributor
    {
        public string Name;
        public string Email;
        public string Role;

        public void Print()
        {
            Console.WriteLine("// " + Name + ", " + Email + " (" + Role + ")");
        }
    }

    //--------------------------------------------------------------------------
}
