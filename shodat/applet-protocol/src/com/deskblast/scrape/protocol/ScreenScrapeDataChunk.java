package com.deskblast.scrape.protocol;

import java.awt.Point;
import java.awt.Rectangle;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class ScreenScrapeDataChunk implements Streamable{

	private int sequence;
	private boolean bLastChunk;
	private int totalSequenceItems;
	private boolean bKeyFrame;
	private Rectangle rectArea = new Rectangle();
	private Point mousePosition = new Point();
	private List<AbstractTileInfo> rects = new ArrayList<AbstractTileInfo>();
    
	public ScreenScrapeDataChunk(int sequence){
		this.sequence = sequence;
	}
	public void addTile(AbstractTileInfo ati){
		this.rects.add(ati);
	}
	public void setLast(boolean last){
		this.bLastChunk = last;
	}
	public void setTotalSequenceItems(int totalSequenceItems){
		this.totalSequenceItems = totalSequenceItems;
	}
	public void setMousePosition(Point mousePosition){
		this.mousePosition = mousePosition;
	}
	public void setKeyFrame(boolean bKeyFrame){
		this.bKeyFrame = bKeyFrame;
	}
	public AbstractTileInfo[] getRects() {
		return this.rects.toArray(new AbstractTileInfo[0]);
	}
	public ScreenScrapeDataChunk(DataInputStream in) throws IOException{		
		sequence = in.readInt();
		bLastChunk = (in.readInt() > 0);
		if(bLastChunk){
			totalSequenceItems = in.readInt();
			bKeyFrame = in.readInt() > 0;
			rectArea.width = in.readInt();
			rectArea.height =  in.readInt();
			mousePosition.x = in.readInt();
			mousePosition.y = in.readInt();
		}
		int rectCount = in.readInt();
		for(int i = 0; i < rectCount; i++){
			byte[] hash = new byte[ScreenScrapeData.FILE_NAME_LENGTH];
			in.readFully(hash);
			String name = new String(hash);
			int length = in.readInt();			
			//System.out.println("image length: " + length);
			int x = in.readInt(); 
			int y = in.readInt();
			int width = in.readInt();
			int height = in.readInt();
			int spriteX = in.readInt();
			int spriteY = in.readInt();
			if(length > 0){
				byte[] jpeg = new byte[length]; 
				in.readFully(jpeg);
				rects.add(new CompressedScreenScrape(jpeg, x, y, width, height, name, spriteX, spriteY));
			}else{
				rects.add(new BackReferenceScreenScrape(x, y, width, height, name, spriteX, spriteY));
			}
		}
	}
	public void stream(DataOutputStream out) throws IOException{
		out.writeInt(this.sequence);
		out.writeInt(this.bLastChunk ? 1 : 0);
		if(this.bLastChunk){
			out.writeInt(this.totalSequenceItems);
			out.writeInt(this.bKeyFrame ? 1 : 0);
			out.writeInt(this.rectArea.width);
			out.writeInt(this.rectArea.height);
			out.writeInt(this.mousePosition.x);
			out.writeInt(this.mousePosition.y);
		}
		out.writeInt(this.rects.size());
		for(AbstractTileInfo ati : this.rects){
			int length = 0;
			if(ati instanceof CompressedScreenScrape){
				length = ((CompressedScreenScrape)ati).getByteCount();			
			}
			byte[] hashBytes = ati.getHash().getBytes();
			out.write(hashBytes, 0, ScreenScrapeData.FILE_NAME_LENGTH);
			out.writeInt(length);			
			out.writeInt(ati.x);
			out.writeInt(ati.y);
			out.writeInt(ati.width);
			out.writeInt(ati.height);
			out.writeInt(ati.spriteX);
			out.writeInt(ati.spriteY);
			if(length > 0){
				out.write(((CompressedScreenScrape)ati).getBytes());
			}
		}
	}
	/*
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
	public void setLastData(boolean bKeyFrame, int totalSequenceItems, 
			Point mousePosition, Rectangle rectArea) {
		this.bLastChunk = true;
		this.bKeyFrame = bKeyFrame;
		this.totalSequenceItems = totalSequenceItems;
		this.mousePosition = mousePosition;
		this.rectArea = rectArea;
	}
	public int getSequence() {
		return sequence;
	}
	public boolean isLastChunk() {
		return bLastChunk;
	}
	public int getTotalSequenceItems() {
		return totalSequenceItems;
	}
	public boolean isKeyFrame() {
		return bKeyFrame;
	}
	public Rectangle getRectArea() {
		return rectArea;
	}
	public Point getMousePosition() {
		return mousePosition;
	}
	@Override
	public String toString() {
		return "ScreenScrapeDataChunk [sequence=" + sequence + ", bLastChunk="
				+ bLastChunk + ", totalSequenceItems=" + totalSequenceItems
				+ ", bKeyFrame=" + bKeyFrame + ", rectArea=" + rectArea
				+ ", mousePosition=" + mousePosition + ", rects=" + rects + "]";
	}
	
}
