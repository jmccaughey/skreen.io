package com.deskblast.scraper;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class UploadHelper {

	public static byte[] streamToByteArray(InputStream is) throws IOException{
	    ByteArrayOutputStream baos = new ByteArrayOutputStream(1);
        for(int iRead = is.read(); iRead != -1; iRead = is.read()){
        	baos.write(iRead);
        }
        return baos.toByteArray();
	}
	
	public static HttpURLConnection openPostConnection(String urlString) throws IOException {
		URL url = new URL(urlString);
		HttpURLConnection connection;
		connection = (HttpURLConnection) url.openConnection();
		connection.setDoOutput(true);
		connection.setRequestProperty("Content-type",
				"application/octet-stream");
		connection.connect();
		return connection;
	}

	
}
