package com.deskblast.scraper;

import java.awt.Point;
import java.awt.Rectangle;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.deskblast.scrape.protocol.AbstractTileInfo;
import com.deskblast.scrape.protocol.BackReferenceScreenScrape;
import com.deskblast.scrape.protocol.CompressedScreenScrape;
import com.deskblast.scrape.protocol.ScreenScrapeData;

public class TestStreamReader{
	
	private File file;
	
	public TestStreamReader(File file){
		this.file = file;
	}
	
	public ScreenScrapeData[] getScrapes() throws FileNotFoundException{
		List<ScreenScrapeData> scrapes = new ArrayList<ScreenScrapeData>();
		ScreenScrapeData data;
		DataInputStream dis = new DataInputStream(new FileInputStream(file));
		while(true){
			try {
				data = readScrape(dis);
				scrapes.add(data);
			} catch (IOException e) {
				e.printStackTrace();				
				break;
			}
		}
		return scrapes.toArray(new ScreenScrapeData[0]);
	}
	
	private ScreenScrapeData readScrape(DataInputStream dis) throws IOException{		
		boolean keyFrame = dis.readInt() > 0;
		Rectangle rect = new Rectangle(dis.readInt(), dis.readInt());
		Point mouse = new Point(dis.readInt(), dis.readInt());
		int rectCount = dis.readInt();
		AbstractTileInfo[] ati = new AbstractTileInfo[rectCount];
		byte[] hashBytes = new byte[32];
		for(int i = 0; i < rectCount; i++){
			dis.read(hashBytes);
			String hash = new String(hashBytes);
			int length = dis.readInt();
			int x = dis.readInt();
			int y = dis.readInt();
			int width = dis.readInt();
			int height = dis.readInt();
			int spriteX = dis.readInt();
			int spriteY = dis.readInt();
			if(length > 0){		
				byte[] imageBytes = new byte[length];
				dis.read(imageBytes);
				ati[i] = new CompressedScreenScrape(imageBytes, x, y, width, height, hash, spriteX, spriteY);
			}else{
				ati[i] = new BackReferenceScreenScrape(x, y, width, height, hash, spriteX, spriteY);
			}
		}
		return new ScreenScrapeData(ati, rect, mouse, keyFrame);
	}
	
	public static void main(String[] args) throws IOException{
		TestStreamReader tsr = new TestStreamReader(new File("stream.bin"));
		ScreenScrapeData[] datas = tsr.getScrapes();
		System.out.println(datas.length);
		for(int i = 0; i < datas.length; i++){
			System.out.println(datas[i]);
			AbstractTileInfo[] atis = datas[i].getRects();
			for(AbstractTileInfo ati : atis){
				System.out.println("\t" + ati);
			}
		}
	}
}