using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Net;
using System.IO;
using System.IO.Compression;
using System.Threading;

namespace shodat
{
    class WebUtil
    {       

        static void Main(string[] args)
        {
            //SmoothProgressBar spb = new SmoothProgressBar();
            //spb.Show();
            string localPath = explicitGZipDownload(new Uri("http://dhcp39.rwc.openlane.com:8888/"), 
                "shodat.exe", createTemporaryFolder());
            //spb.Hide();            
            FileStream fs = new FileStream(localPath, FileMode.Open, FileAccess.Read, 
                FileShare.Delete | FileShare.Read, 4096, FileOptions.DeleteOnClose);                           
            Process process = Process.Start(localPath, "bogo parms");
            process.WaitForExit();     
            
        }
/*
        public static void finish(string localPath)
        {
            FileStream fs = new FileStream(localPath, FileMode.Open, FileAccess.Read,
                FileShare.Delete | FileShare.Read, 4096, FileOptions.DeleteOnClose);
            Process process = Process.Start(localPath, "bogo parms");
            process.WaitForExit();     
        }
 */
        public static string buildBaseUrl(Uri document)
        {
            // drop query string and doc
            string end = document.Segments[document.Segments.Length - 1];
            string context = document.OriginalString.Substring(0, document.OriginalString.IndexOf(end));
            return context;
        }

        public static string createTemporaryFolder()
        {
            Guid guid = Guid.NewGuid();
            string folderPath = System.IO.Path.GetTempPath() + "shodat/" + guid.ToString() + "/";
            System.IO.Directory.CreateDirectory(folderPath);
            return folderPath;
        }
/*
        public static string simpleDownload(Uri context, string binaryName, string destinationDir)
        {
            // TODO: gzip compress download
            WebClient myWebClient = new WebClient();
            string localPath = destinationDir + binaryName;            
            myWebClient.DownloadFile(context + binaryName, localPath);
            return localPath;
        }
*/
        public static string explicitGZipDownload(Uri context, string binaryName, string destinationDir)
        {
            WebClient myWebClient = new WebClient();
            //ManualResetEvent manualEvent = new ManualResetEvent(false);
            string localPath = destinationDir + binaryName;
            /*
            myWebClient.DownloadProgressChanged += (s, e) =>
            {
                //Console.WriteLine(e.ProgressPercentage);
                //progressBar.Value = e.ProgressPercentage;
            };
            myWebClient.DownloadFileCompleted += (s, e) =>
            {
                //Console.WriteLine("done");
                manualEvent.Set();
            };
             */ 
            string gzFile = binaryName + ".gz";
            Console.WriteLine("fetching " + context + gzFile);
            myWebClient.DownloadFile(new Uri(context + gzFile), localPath);
            //manualEvent.WaitOne();
            unzipInPlace(localPath);
            return localPath;
        }

/*
        public static string simpleGZipDownload(Uri context, string binaryName, string destinationDir)
        {         
            WebClient myWebClient = new GZipWebClient();
            ManualResetEvent manualEvent = new ManualResetEvent(false);
            string localPath = destinationDir + binaryName;
            myWebClient.DownloadProgressChanged += (s, e) =>
            {
                Console.WriteLine(e.ProgressPercentage);                
                //progressBar.Value = e.ProgressPercentage;
            };
            myWebClient.DownloadFileCompleted += (s, e) =>
            {
                Console.WriteLine("done");
                manualEvent.Set();
                //finish(localPath);
                //progressBar.Visible = false;
                // any other code to process the file
            };            
            myWebClient.DownloadFileAsync(new Uri(context + binaryName), localPath);            
            manualEvent.WaitOne();            
            return localPath;
        }
*/
        private static void unzipInPlace(string path)
        {
            MemoryStream ms = null;
            using (Stream fs = File.OpenRead(path))
            using (Stream csStream = new GZipStream(fs, CompressionMode.Decompress))
            {
                ms = new MemoryStream();
                byte[] buffer = new byte[4096];
                int nRead;
                while ((nRead = csStream.Read(buffer, 0, buffer.Length)) > 0)
                {
                    ms.Write(buffer, 0, nRead);
                }
            }
            File.Delete(path);
            byte[] unzipped = ms.ToArray();
            File.WriteAllBytes(path, unzipped);
        }

/*
        public static void fetchBinary(Uri activationUri, string binaryName, System.IO.FileStream fs)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(activationUri + binaryName);
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            WebHeaderCollection myWebHeaderCollection = response.Headers;
            for (int i = 0; i < myWebHeaderCollection.Count; i++)
            {
                String header = myWebHeaderCollection.GetKey(i);
                String[] values = myWebHeaderCollection.GetValues(header);
                if (values.Length > 0)
                {
                    Console.WriteLine("The values of {0} header are : ", header);
                    for (int j = 0; j < values.Length; j++)
                        Console.WriteLine("\t{0}", values[j]);
                }
                else
                    Console.WriteLine("There is no value associated with the header");

            }
            Console.WriteLine("conten length: {0}", response.ContentLength);
            Console.WriteLine("content type: {0}", response.ContentType);
            Stream receiveStream = response.GetResponseStream();            
            //string localPath = folderPath + binaryName;
            //Console.WriteLine(localPath);
            //System.IO.FileStream fs = new FileStream(
            //    localPath, FileMode.Create, FileAccess.ReadWrite, FileShare.Delete);
            //System.IO.FileStream fs = System.IO.File.Create(localPath, 4096, FileOptions.DeleteOnClose);
            int read = receiveStream.ReadByte();
            while (read != -1)
            {
                fs.WriteByte((byte)read);
                read = receiveStream.ReadByte();
            }            
            response.Close();
            receiveStream.Close();
            fs.Flush();
        }
*/
    
    }
}
