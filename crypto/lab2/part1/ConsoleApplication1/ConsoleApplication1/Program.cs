using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Data;
using System.Data.SqlClient;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {

            string cmdText =    "USE TaxiCompany\n" +
                                "SELECT tblTaxists.TaxistName, tblClients.ClientName\n" +
                                "FROM tblClients, tblQueries, tblTaxists\n" +
                                "WHERE tblTaxists.TaxistCar='ЗИЛ'\n" +
                                    "AND tblTaxists.TaxistID = tblQueries.TaxistID\n" +
                                    "AND tblClients.ClientID = tblQueries.ClientID\n" +
                                    "ORDER BY tblTaxists.TaxistName\n" +
                                    "FOR XML AUTO, ELEMENTS";

            string connect = "Data Source=(local);Initial Catalog=TaxiCompany;Integrated Security=SSPI";

            // Create SqlConnection
            SqlConnection sqlCN = new SqlConnection(connect);
            try
            {
                sqlCN.Open();

                // Create SqlCommand
                SqlCommand sqlCMD = new SqlCommand(cmdText, sqlCN);

                // Execute SqlCommand and load XmlReader
                XmlReader xr = sqlCMD.ExecuteXmlReader();
                while (xr.Read())
                {
                    if (xr.Name == "TaxistName")
                        Console.WriteLine(xr.ReadElementString()+":");
                    else if (xr.Name == "ClientName")
                        Console.WriteLine("     " + xr.ReadElementString());
                }
                xr.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            finally
            {
                if (!(sqlCN == null))
                {
                    sqlCN.Close();
                }
                Console.Read();
            }
        }
    }
}
