package com.deskblast.scraper;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.deskblast.scrape.protocol.Streamable;

public class ParallelUploader implements Uploader{

	static final int THREAD_COUNT = 5; // JVM defaults to 5 connections per host
	boolean shutdown = false;
	Thread[] threads = new Thread[THREAD_COUNT];
	List<Streamable> streamables = Collections.synchronizedList(new ArrayList<Streamable>());
	List<byte[]> responses = Collections.synchronizedList(new ArrayList<byte[]>());
	Object lock = new Object();
	private String url;
	private IOException ioException;
	
	public ParallelUploader(){
		for(int i = 0; i < threads.length; i++){
			threads[i] = new Thread(new RunnableUpload());
			threads[i].setDaemon(true);
			threads[i].start();
		}
		System.out.println(THREAD_COUNT + " threads started in ParallelUploader.");
	}
	public void shutdown(){
		shutdown = true;
		synchronized(lock){
			lock.notifyAll();
		}
	}
	synchronized void setIOException(IOException e){
		this.ioException = e;
	}
	@Override
	public List<byte[]> upload(String url, Streamable[] streamables) throws IOException {
		// TODO: upload across multiple threads/connections
		// return when all items have been uploaded
		this.url = url;
		responses.clear();
		setIOException(null);
		for(Streamable streamable : streamables){
			this.streamables.add(streamable);
		}
		synchronized(lock){
			lock.notifyAll();
		}
		while(responses.size() < streamables.length && ioException == null){
			try {
				synchronized(lock){
					lock.wait();
				}
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		this.streamables.clear();
		if(ioException != null){
			throw ioException;
		}
		return responses;
	}

	class RunnableUpload implements Runnable{
		@Override
		public void run() {
			while(shutdown != true){
				if(streamables.size() > 0){
					
				}else{
					synchronized(lock){
						try {
							lock.wait();
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
				try{
					Streamable streamable = streamables.remove(0);
			        HttpURLConnection connection = UploadHelper.openPostConnection(url);
		            DataOutputStream oos = new DataOutputStream(connection.getOutputStream());
		            streamable.stream(oos);
		            oos.flush();
		            oos.close();
		            InputStream is = connection.getInputStream();
		            responses.add(UploadHelper.streamToByteArray(is));
		            synchronized(lock){
		            	lock.notifyAll();
		            }
				} catch (java.lang.IndexOutOfBoundsException ioob){
					// this just means the thread found nothing in the queue
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					setIOException(e);
					//throw new Exception(e);
				}
			}
			System.out.println("thread exiting");
		}
	}
	
	static class TestStreamable implements Streamable{
		String content;
		TestStreamable(String content){
			this.content = content;
		}
		@Override
		public void stream(DataOutputStream outputStream) throws IOException {
			outputStream.writeUTF(content);
		}
	}
	
	public static void main(String[] args) throws IOException{
		ParallelUploader uploader = new ParallelUploader();
		for(int i = 0; i < 4; i++){
			List<byte[]> responses = uploader.upload(
					"http://localhost:8080/deskblast/justlisten", 
					new Streamable[]{ 
							new TestStreamable("first"), 
							new TestStreamable("second"),
							new TestStreamable("third"),
							new TestStreamable("fourth"),
							new TestStreamable("fifth"),
							new TestStreamable("sixth"),
							new TestStreamable("seventh"),
							new TestStreamable("eighth"),
							new TestStreamable("ninth"),
							new TestStreamable("tenth"),
					}
			);
			for(byte[] response : responses){
				//System.out.println(response);
				System.out.println(new String(response));
			}
		}
		System.in.read();
		System.in.read();
		uploader.shutdown();
	}

}
