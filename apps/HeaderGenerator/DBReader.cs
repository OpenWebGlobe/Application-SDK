using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.OleDb;

namespace HeaderGenerator
{
    public class DBReader
    {
        public static void FunctionQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            cg.FunctionList.Clear();
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM Functions ORDER BY Topic, ID", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                Function curFunc = new Function();

                curFunc.ID = Convert.ToInt32(dr["ID"].ToString());
                curFunc.FunctionName = dr["FunctionName"].ToString();
                curFunc.Brief = dr["Brief"].ToString();
                curFunc.Description = dr["Description"].ToString();
                int nRetVal = Convert.ToInt32(dr["ReturnValue"].ToString());

                Int32 nTopic = Convert.ToInt32(dr["Topic"].ToString());

                if (nTopic > 0)
                {
                    curFunc.Topic = cg.TopicList[nTopic - 1].Name.ToString();
                }
                else
                {
                    curFunc.Topic = "";
                }

                if (nRetVal > 0)
                {
                    curFunc.ReturnValue = cg.DataTypeList[nRetVal - 1].CType.ToString();
                    curFunc.ReturnValueCS = cg.DataTypeList[nRetVal - 1].CSType.ToString();
                    curFunc.ReturnValueVB = cg.DataTypeList[nRetVal - 1].VBType.ToString();
                    curFunc.ReturnValuePY = cg.DataTypeList[nRetVal - 1].PythonType.ToString();
                }

                curFunc.ReturnValueDescription = dr["ReturnValueDescription"].ToString();

                for (int i = 0; i < Function.MaxParam; i++)
                {
                    string sParamName = "ParamName" + Convert.ToString(i + 1);
                    string sParamType = "ParamType" + Convert.ToString(i + 1);
                    string sParamDesc = "ParamDescription" + Convert.ToString(i + 1);
                    curFunc.HasParam[i] = false;

                    curFunc.ParamName[i] = dr[sParamName].ToString();
                    string sParamTmp = dr[sParamType].ToString();
                    int nParam = 0;
                    if (sParamTmp.Length > 0)
                    {
                        nParam = Convert.ToInt32(sParamTmp);
                        if (nParam > 0)
                        {
                            curFunc.ParamType[i] = cg.DataTypeList[nParam - 1].CType.ToString();
                            curFunc.ParamTypeCS[i] = cg.DataTypeList[nParam - 1].CSType.ToString();
                            curFunc.ParamTypeVB[i] = cg.DataTypeList[nParam - 1].VBType.ToString();
                            curFunc.ParamTypePY[i] = cg.DataTypeList[nParam - 1].PythonType.ToString();
                            curFunc.HasParam[i] = true;
                        }
                    }

                    curFunc.ParamDescription[i] = dr[sParamDesc].ToString();
                }

                cg.FunctionList.Add(curFunc);

                //curFunc.Print();
            }

            dr.Close();
        }

        //---------------------------------------------------------------------

        public static void DatatypeQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            cg.DataTypeList.Clear();
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM DataTypes", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                DataType curDatatype = new DataType();

                curDatatype.ID = Convert.ToInt32(dr["ID"].ToString());
                curDatatype.DataTypeName = dr["DataTypeName"].ToString();
                curDatatype.CType = dr["CType"].ToString();
                curDatatype.CTypeDef = dr["CTypeDef"].ToString();

                curDatatype.CSType = dr["CSType"].ToString();
                curDatatype.CSTypeDef = dr["CSTypeDef"].ToString();

                curDatatype.VBType = dr["VBType"].ToString();
                curDatatype.VBTypeDef = dr["VBTypeDef"].ToString();

                curDatatype.PythonType = dr["PythonType"].ToString();
                curDatatype.PythonTypeDef = dr["PythonTypeDef"].ToString();

                cg.DataTypeList.Add(curDatatype);
            }

            dr.Close();
        }

        //---------------------------------------------------------------------

        public static void TopicQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            cg.TopicList.Clear();
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM Topics", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                Topic curTopic = new Topic();
                curTopic.ID = Convert.ToInt32(dr["ID"].ToString());
                curTopic.Name = dr["Name"].ToString();
                curTopic.Description = dr["Description"].ToString();
                cg.TopicList.Add(curTopic);

                //curTopic.Print();

            }

            dr.Close();
        }

        //---------------------------------------------------------------------

        public static void SDKQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM SDK", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                cg.SDKInfo.Name = dr["Name"].ToString();
                cg.SDKInfo.Version = dr["Version"].ToString();
                cg.SDKInfo.Copyright = dr["Copyright"].ToString();
            }

            //cg.SDKInfo.Print();

            dr.Close();
        }

        //---------------------------------------------------------------------

        public static void ContributorQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            cg.ContributorList.Clear();
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM Contributors", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                Contributor curContrib = new Contributor();
                curContrib.Name = dr["Name"].ToString();
                curContrib.Email = dr["Email"].ToString();
                curContrib.Role = dr["Role"].ToString();
                cg.ContributorList.Add(curContrib);

                //curContrib.Print();
            }

            dr.Close();
        }


        //---------------------------------------------------------------------

        public static void ConstantQuery(OleDbConnection dbcon, CodeGenerator cg)
        {
            cg.ConstantList.Clear();
            OleDbCommand cmd = new OleDbCommand("SELECT * FROM Constants ORDER BY Topic, ID", dbcon);
            OleDbDataReader dr = cmd.ExecuteReader();

            while (dr.Read())
            {
                Constant curConst = new Constant();
                curConst.Name = dr["Name"].ToString();
                curConst.Value = dr["Value"].ToString();
                Int32 nTopic = Convert.ToInt32(dr["Topic"].ToString());
                if (nTopic > 0)
                {
                    curConst.Topic = cg.TopicList[nTopic - 1].Name.ToString();
                }
                else
                {
                    curConst.Topic = "";
                }
                curConst.Description = dr["Description"].ToString();
                cg.ConstantList.Add(curConst);

                //curConst.Print();
            }

            dr.Close();
        }

        //---------------------------------------------------------------------
    }
}
