package com.deskblast.scrape.protocol;

import java.awt.Point;
import java.awt.Rectangle;
import java.io.DataOutputStream;
import java.io.IOException;

public class ScreenScrapeData {

    static final int FILE_NAME_LENGTH = 32;

//	private int iByteCount;
//	private int backreferenceCount;
	private boolean bKeyFrame;
	private Rectangle rectArea;
	private AbstractTileInfo[] rects;
	private Point mousePosition;
    
	/*
	public ScreenScrapeData(DataInputStream in) throws IOException{		
		bKeyFrame = in.readInt() > 0;
		rectArea = new Rectangle(in.readInt(), in.readInt());
		mousePosition = new Point(in.readInt(), in.readInt());
		rects = new AbstractTileInfo[in.readInt()];
		for(int i = 0; i < rects.length; i++){
			byte[] hash = new byte[FILE_NAME_LENGTH];
			in.readFully(hash);
			String name = new String(hash);
			int length = in.readInt();			
			//System.out.println("image length: " + length);
			Point point = new Point(in.readInt(), in.readInt());
			if(length > 0){
				byte[] jpeg = new byte[length]; 
				in.readFully(jpeg);
				rects[i] = new CompressedScreenScrape(jpeg, point.x, point.y, name);
				this.iByteCount += jpeg.length;
			}else{
				rects[i] = new BackReferenceScreenScrape(point.x, point.y, name);
				backreferenceCount++;
			}
		}
	}
	*/
	/*
	public static void readStream(DataInputStream in, ScrapeStreamHandler handler) throws IOException{
		boolean bKeyFrame = in.readInt() > 0;
		Rectangle rectArea = new Rectangle(in.readInt(), in.readInt());
		Point mousePosition = new Point(in.readInt(), in.readInt());
		AbstractTileInfo[] rects = new AbstractTileInfo[in.readInt()];		
		for(int i = 0; i < rects.length; i++){
			byte[] hash = new byte[FILE_NAME_LENGTH];
			in.readFully(hash);
			String name = new String(hash);
			int length = in.readInt();			
			//System.out.println("image length: " + length);
			Point point = new Point(in.readInt(), in.readInt());
			if(length > 0){
				byte[] jpeg = new byte[length]; 
				in.readFully(jpeg);
				rects[i] = new CompressedScreenScrape(jpeg, point.x, point.y, name);
				//this.iByteCount += jpeg.length;
			}else{
				rects[i] = new BackReferenceScreenScrape(point.x, point.y, name);
				//backreferenceCount++;
			}
			if(i == 0){				
				handler.handleScrape(new ScreenScrapeMetaData(
						new AbstractTileInfo[]{rects[i]}, 
						rectArea, mousePosition, bKeyFrame));
			}else{
				handler.handleScrape(new ScreenScrapeData(new AbstractTileInfo[]{rects[i]}));
			}
		}		
	}
	*/
	public ScreenScrapeData(AbstractTileInfo[] rectsToSend, 
			Rectangle rectArea, Point mousePosition, boolean bKeyFrame) {
		this.rects = rectsToSend;
//		for (int i = 0; i < rectsToSend.length; i++) {
//			if (rectsToSend[i] instanceof BackReferenceScreenScrape) {
//				this.backreferenceCount++;
//			} else {
//				this.iByteCount += ((CompressedScreenScrape) rectsToSend[i])
//						.getByteCount();
//			}
//		}
		this.rectArea = rectArea;
		this.bKeyFrame = bKeyFrame;
		this.mousePosition = mousePosition;
	}
//	public ScreenScrapeData(PartialScrapeData[] partialScrapeDatas){
//		this.rects = new AbstractTileInfo[partialScrapeDatas.length];
//		for(int i = 0; i < partialScrapeDatas.length; i++){
//			this.rects[i] = partialScrapeDatas[i].getTile();
//		}
//		this.rectArea = partialScrapeDatas[partialScrapeDatas.length-1].getRectArea();
//		this.bKeyFrame = partialScrapeDatas[partialScrapeDatas.length-1].isKeyFrame();
//		this.mousePosition = partialScrapeDatas[partialScrapeDatas.length-1].getMousePosition();
//	}
//	
	public boolean isKeyFrame() {
		return this.bKeyFrame;
	}

	public int getWidth() {
		return this.rectArea.width;
	}

	public int getHeight() {
		return this.rectArea.height;
	}

	public AbstractTileInfo[] getRects() {
		return this.rects;
	}

//	public int getByteCount() {
//		return iByteCount;
//	}
//	public int getBackreferenceCount() {
//		return backreferenceCount;
//	}
	public Point getMousePosition() {
		return this.mousePosition;
	}
	public void writeToDataOututStream(DataOutputStream out) throws IOException{
		out.writeInt(this.bKeyFrame ? 1 : 0);
		out.writeInt(this.rectArea.width);
		out.writeInt(this.rectArea.height);
		out.writeInt(this.mousePosition.x);
		out.writeInt(this.mousePosition.y);
		out.writeInt(this.rects.length);
		for(int i = 0; i < this.rects.length; i++){
			int length = 0;
			if(this.rects[i] instanceof CompressedScreenScrape){
				length = ((CompressedScreenScrape)rects[i]).getByteCount();			
			}
			byte[] hashBytes = rects[i].getHash().getBytes();
			out.write(hashBytes, 0, FILE_NAME_LENGTH);
			out.writeInt(length);			
			out.writeInt(rects[i].x);
			out.writeInt(rects[i].y);
			out.writeInt(rects[i].width);
			out.writeInt(rects[i].height);
			out.writeInt(rects[i].spriteX);
			out.writeInt(rects[i].spriteY);
			if(length > 0){
				out.write(((CompressedScreenScrape)rects[i]).getBytes());
			}
		}
		
	}
	/*
	public String toString() {
		String str = super.toString();
		str += "\n\tbyte count: " + this.getByteCount();
		str += "\n\trect count: " + this.getCompressedRects().length;
		str += "\n\tbackreference count: " + this.backreferenceCount;
		str += "\n\tregion: " + this.rectArea;
		str += "\n\tkey frame: " + this.isKeyFrame();
		str += "\n\tmouse position: " + this.getMousePosition();
		return str;
	}
	public static void main(String[] args) throws IOException, ClassNotFoundException{
		ByteArrayOutputStream boss = new ByteArrayOutputStream();
		CompressedScreenScrape scrape1 = new CompressedScreenScrape(
				new byte[1024], 0, 0, "12345678901234567890123456789012");
		BackReferenceScreenScrape scrape2 = new BackReferenceScreenScrape(
				256, 256,            "45678901234567890123456789012456");
		ScreenScrapeData data = new ScreenScrapeData(new AbstractTileInfo[]{scrape1, scrape2},
				new Rectangle(1280, 1024), new Point(400, 500), false);
		System.out.println(data);
		//ObjectOutputStream oos = new ObjectOutputStream(boss);
		DataOutputStream dos = new DataOutputStream(boss);
		data.writeToDataOututStream(dos);
		dos.close();
		//oos.writeObject(data);
		//oos.close();
		ByteArrayInputStream bais = new ByteArrayInputStream(boss.toByteArray());
		//ObjectInputStream ois = new ObjectInputStream(bais);
		DataInputStream dis = new DataInputStream(bais);
		ScreenScrapeData readData = new ScreenScrapeData(dis);
		System.out.println(readData);
		
	}
	*/
	
}
