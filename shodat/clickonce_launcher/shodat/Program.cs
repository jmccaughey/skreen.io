using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Collections;
using System.Collections.Specialized;
using System.Deployment.Application;
using System.Web;
using System.Windows.Forms;


namespace shodat
{    
    class Program
    {

        static string BINARY_NAME = "shodat.exe";

        static string[] positional_parms = { "host", "port", "blastId", "blasterId", "key", "context" };

        static void Main(string[] args)
        {
            try
            {                
                NameValueCollection nvc = GetQueryStringParameters();
                //MessageBox.Show(nvc.Count + " parameters found");
                string parmString = buildParameterString(nvc);
                //MessageBox.Show(parmString);
                Uri context = new Uri(WebUtil.buildBaseUrl(ApplicationDeployment.CurrentDeployment.ActivationUri));
                //MessageBox.Show("context: " + context.OriginalString);
                //MessageBox.Show("about to download: " + context.OriginalString + BINARY_NAME);
                string folderPath = WebUtil.createTemporaryFolder();
                string downloadedBinary = WebUtil.explicitGZipDownload(context, BINARY_NAME, folderPath);
                FileStream fs = new FileStream(downloadedBinary, FileMode.Open, FileAccess.Read,
                    FileShare.Delete | FileShare.Read, 4096, FileOptions.DeleteOnClose);
                Process process = Process.Start(downloadedBinary, parmString);
                process.WaitForExit();
            }
            catch (Exception e)
            {
                string msg = "Error launching application:\n\n" + e;
                string cap = "Error";
                MessageBox.Show(msg, cap, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private static NameValueCollection GetQueryStringParameters()
        {
            NameValueCollection nameValueTable = new NameValueCollection();

            if (ApplicationDeployment.IsNetworkDeployed)
            {
                string queryString = ApplicationDeployment.CurrentDeployment.ActivationUri.Query;
                nameValueTable = System.Web.HttpUtility.ParseQueryString(queryString);
            }

            return (nameValueTable);
        }

        private static string buildParameterString(NameValueCollection nameValueCollection)
        {
            string res = "";
            for (int i = 0; i < positional_parms.Length; i++)
            {
                string value = nameValueCollection.Get(positional_parms[i]);
                if (value != null)
                {
                    res = res + value + " ";
                }
                else
                {
                    return null;
                }
            }
            return res;
        }

    }
}
