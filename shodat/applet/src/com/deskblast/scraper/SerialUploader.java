package com.deskblast.scraper;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.util.ArrayList;
import java.util.List;

import com.deskblast.scrape.protocol.Streamable;

public class SerialUploader implements Uploader {

	@Override
	public List<byte[]> upload(String url, Streamable[] streamables) throws IOException {
		List<byte[]> responses = new ArrayList<byte[]>();
		for(Streamable streamable : streamables){
            HttpURLConnection connection = UploadHelper.openPostConnection(url);
            DataOutputStream oos = new DataOutputStream(connection.getOutputStream());
            streamable.stream(oos);
            oos.flush();
            oos.close();
            InputStream is = connection.getInputStream();
            responses.add(UploadHelper.streamToByteArray(is));
		}
		return responses;
	}
	
	public static void main(String[] args) throws IOException{
		Uploader uploader = new SerialUploader();
		Streamable streamable = new Streamable() {
			@Override
			public void stream(DataOutputStream outputStream) throws IOException {
				outputStream.writeUTF("test upload string");
			}
		};
		Streamable streamable2 = new Streamable() {
			@Override
			public void stream(DataOutputStream outputStream) throws IOException {
				outputStream.writeUTF("second test upload string");
			}
		};
		List<byte[]> responses = uploader.upload(
				"http://localhost:8080/deskblast/justlisten", 
				new Streamable[]{ streamable, streamable2 }
		);
		for(byte[] response : responses){
			System.out.println(response);
			System.out.println(new String(response));
		}
	}

}
