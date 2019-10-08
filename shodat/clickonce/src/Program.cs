using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Collections.Specialized;
using System.Web;
using System.Net;
using System.Deployment.Application;
using System.Diagnostics;
using System.Windows.Forms;
using System.Threading;

namespace shod.at_installer
{

    class Program{

        static String PRODUCT_NAME = "shodat";
        static String[] MODULE_NAMES = { "np" + PRODUCT_NAME + ".dll", PRODUCT_NAME + ".dll" };
        static String INSTALLER_NAME = PRODUCT_NAME + "q.exe";

        static String[] GetProgramsUsingModules(String[] moduleNames)
        {
            List<String> programs = new List<String>();
            System.Diagnostics.Process[] ps = System.Diagnostics.Process.GetProcesses();
            foreach (System.Diagnostics.Process p in ps)
            {
                if (p.MainWindowTitle != "" && p.MainModule.FileVersionInfo.FileDescription != " "
                    && p.MainModule.FileVersionInfo.FileDescription != null)
                {              
                    ProcessModuleCollection pms = p.Modules;
                    foreach (ProcessModule pm in pms)
                    {
                        foreach (String moduleName in moduleNames)
                        {
                            if (pm.FileName.EndsWith(moduleName))
                            {
                                programs.Add(p.MainModule.FileVersionInfo.FileDescription);                                
                            }
                        }
                    }
                }
            }
            return programs.ToArray();
        }

        static void Main(string[] args)
        {
            // find the plugin dll (use registry?)
            // check it's version # (FileVersionInfo)
            // check current version # from server (or ActivationUri parms)
            // if no or newer version, get installer from server
            // check if dll is currently in use (open file in append mode)
            // [if so, find program name and prompt user to close it]
            // [if not, goto +2]
            // [goto -2]
            // run installer (should be silent)
            // make request to server (get key & other parms from ActivationUri)
            // to page 'pluginInstalledStartSharing'            
            String path = Path.Combine(Environment.GetFolderPath(
                    Environment.SpecialFolder.ApplicationData), "Mozilla\\plugins\\np" + PRODUCT_NAME + ".dll");
            FileVersionInfo fvi = null;
            if (File.Exists(path))
            {
                fvi = FileVersionInfo.GetVersionInfo(path);
            }            
            String[] programsUsingModules = null;
            do
            {
                programsUsingModules = GetProgramsUsingModules(MODULE_NAMES);
                String names = "";
                foreach (String name in programsUsingModules)
                {
                    names += name + " ";
                }
                if (names.Length > 0)
                {
                    MessageBox.Show(names + " are using the modules.");
                }            

            } while (programsUsingModules.Length > 0);         
            if(ApplicationDeployment.IsNetworkDeployed)
            {
                //MessageBox.Show("start network deployed");
                String uri = ApplicationDeployment.CurrentDeployment.ActivationUri.ToString();                
                int fileNameStart = uri.Length - Path.GetFileName(uri).Length;
                String root = uri.Substring(0, fileNameStart);
                String installer = root + INSTALLER_NAME;
                HttpWebRequest req = null;
                try
                {
                    req = (HttpWebRequest)WebRequest.Create(installer);
                    Stream data = req.GetResponse().GetResponseStream();
                    String tempFile = Path.Combine(Path.GetTempPath(), INSTALLER_NAME);
                    BinaryWriter bw = new BinaryWriter(new FileStream(tempFile, FileMode.Create));
                    int read = data.ReadByte();
                    while (read != -1)
                    {
                        bw.Write((byte)read);
                        read = data.ReadByte();
                    }
                    bw.Close();
                    // TODO: serve the current version, like shodat/activex_version
                    // returns 1.0.0.42
                    Process installerProcess = Process.Start(tempFile);
                    installerProcess.WaitForExit();
                    installerProcess.Close();
                    installerProcess.Dispose();
                    //MessageBox.Show("installer finished");
                    int sleepCount = 0;
                    while (File.Exists(tempFile) && sleepCount < 20)
                    {
                        try
                        {
                            File.Delete(tempFile);
                        }
                        catch (Exception e)
                        {
                            Thread.Sleep(200);
                            sleepCount++;
                        }
                    }
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                }         
                String url = root + "pluginInstalledStartSharing" 
                    + ApplicationDeployment.CurrentDeployment.ActivationUri.Query.ToString();                
                try
                {
                    req = (HttpWebRequest)WebRequest.Create(url);
                    Stream data = req.GetResponse().GetResponseStream();
                    int read = 0;
                    do
                    {
                        read = data.ReadByte();
                    } while (read != -1);
                    data.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                }
            }                        
        }
    }
}
