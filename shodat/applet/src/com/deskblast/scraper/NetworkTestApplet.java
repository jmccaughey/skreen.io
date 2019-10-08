package com.deskblast.scraper;

import java.applet.Applet;
import java.awt.Container;
import java.awt.Frame;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;

public class NetworkTestApplet extends Applet {

  //  private ScrapeSender scrapeSender;

	//static final int DATA_LENGTH = 1024 * 100;
	
	static final String SMALL_CONTENT = "the content here";
	
	static final String MEDIUM_CONTENT = "Tropical-storm warnings have been posted along the US Gulf Coast in anticipation of what could become tropical storm Lee before the end of Friday. Skip to next paragraph	Related stories		Katia shaping up to be major hurricane, impact on US unclear	As hurricane Katia picks up steam, potential tropical storm forming in Gulf	Tropical storm Katia strengthens to hurricane; too early to talk threat	Topics	Hurricanes and Cyclones Accidents and Disasters Natural Disasters Weather	As of 11 a.m. Eastern time Friday, forecast offices had issued tropical-storm warnings that stretched from Sabine Pass, Texas to Pascagoula, Miss.	The center of the storm, currently known only as tropical depression 13, is located some 300 miles southwest of Houston and some 224 miles south-southeast of New Orleans.	IN PICTURES: Hurricane Irene	The storm center is expected to close in on the southeastern coast of Louisiana this weekend at near-hurricane strength, according to the latest forecast from the National Hurricane Center in Miami.	It is currently is projected to dump 10 to 15 inches of rain on southern portions of Louisiana, Mississippi, and Alabama, with isolated pockets getting up to 20 inches.	Forecasters say they expect a storm surge of 2 to 4 feet along the coasts affected by the storm's on-shore winds.	The potential for large rainfall totals stems from the significant amounts of moisture feeding the storm and from its glacial pace, forecasters say. The storm system is moving at a lethargic 2 miles an hour. (By contrast, hurricane Katia, currently in the Atlantic, is traveling a bit more smartly along its path at some 15 miles an hour.)	Along the Gulf Coast, emergency managers are gearing up for tropical depression 13.	On Thursday, ExxonMobil and Royal Dutch Shell evacuated some of their offshore platforms in anticipation of the storm's arrival.";	

	static final String BIGGER_CONTENT = MEDIUM_CONTENT + MEDIUM_CONTENT + MEDIUM_CONTENT + MEDIUM_CONTENT + MEDIUM_CONTENT;
	
	private String doc;

	public void doGetTest(String query) throws IOException{
		long start = System.currentTimeMillis();
		URL url = new URL(doc + query);
		URLConnection conn = url.openConnection();		
		InputStream is = conn.getInputStream();
		int index = 0;
		byte[] received = new byte[query.length()];
		for(int read = is.read(); read != -1; read = is.read()){
			received[index] = (byte)read;
			index++;
		}
		is.close();
		System.out.println((System.currentTimeMillis() - start) + ("ms"));
		System.out.println(new String(received));
	}
	
	public void doPostTest(byte[] content, int times) throws IOException{
		System.out.println("sending " + content.length + " bytes " + times + " times");
		long start = System.currentTimeMillis();
		for(int i = 0; i < times; i++){
			send(new URL(doc),
					content
					//new byte[size]
					         );
		}		
		System.out.println((System.currentTimeMillis() - start) + ("ms"));
	}
	
	private void send(URL url, byte[] data) throws IOException{
		long start = System.currentTimeMillis();
		URLConnection conn = url.openConnection();		
		conn.setDoOutput(true);
		OutputStream os = conn.getOutputStream();		
		for(int i = 0; i < data.length; i++){
			os.write(data[i]);		
		}
		os.close();
		InputStream is = conn.getInputStream();
		int index = 0;
		byte[] received = new byte[data.length];
		for(int read = is.read(); read != -1; read = is.read()){
			received[index] = (byte)read;
			index++;
		}
		is.close();
		long elapsed = (System.currentTimeMillis() - start);
		System.out.println( elapsed + "ms" + " (" + calculateKbps(data.length, elapsed) + ")");
		if(received.length < 5000){
			System.out.println(new String(received));
		}
	}	
		
    public void init() {
        super.init();
        System.out
                .println("applet code base: " + this.getCodeBase().toString());
        System.out.println("name: " + getParameter("name"));
        System.out.println("id: " + getParameter("id"));
        URL url = this.getCodeBase();
        System.out.println("applet code base: " + url.toString());
        doc = url.toString() + "justlisten";
        System.out.println("sending to: " + doc);
        try {
        	doPostTest(getPhotoBytes("/mom_bode.jpg"), 3);
        	doPostTest(getPhotoBytes("/mom_bode.jpg"), 3);
        	doPostTest(getPhotoBytes("/snackers.jpg"), 1);
        	doGetTest("?tokenParm=tokenValue");
        	doGetTest("?tokenParm=tokenValue");
        	doGetTest("?tokenParm=tokenValue");
			doPostTest(SMALL_CONTENT.getBytes(), 10);
			doPostTest(MEDIUM_CONTENT.getBytes(), 10);
			doPostTest(BIGGER_CONTENT.getBytes(), 10);
			doPostTest(getPhotoBytes("/swingers.jpg"), 1);
			doPostTest(getPhotoBytes("/snackers.jpg"), 1);
			doGetTest("?tokenParm=tokenValue");
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }

    private byte[] getPhotoBytes(String name) throws IOException {
    	InputStream is = this.getClass().getResourceAsStream(name);
    	ByteArrayOutputStream baos = new ByteArrayOutputStream();
    	int read;
    	while((read = is.read()) > -1){
    		baos.write(read);
    	}
    	return baos.toByteArray();
    }

	private Frame findParentFrame() {
        Container c = this;
        while (c != null) {
            if (c instanceof Frame) {
                return (Frame) c;
            }
            c = c.getParent();
        }
        return (Frame) null;
    }

    public void destroy() {
//        this.scrapeSender.stopScraping();
        super.destroy();
    }
    
    private String calculateKbps(long bytes, long ms){
    	 return (((bytes*8L)/(float)ms)/1000.0 +" mbps");
    }

}
			