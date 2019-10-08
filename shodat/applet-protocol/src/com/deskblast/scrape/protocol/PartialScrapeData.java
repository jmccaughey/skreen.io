package com.deskblast.scrape.protocol;



/**
 * @author johnm
 *
 */
public class PartialScrapeData {
/*
    static final int FILE_NAME_LENGTH = 32;

	//private int iByteCount;
	//private boolean bLastTile;
	//private int backreferenceCount;
	private boolean bKeyFrame;
	private Rectangle rectArea;
	private AbstractTileInfo tile;
	private Point mousePosition;
    
	public PartialScrapeData(AbstractTileInfo rectToSend, Rectangle rectArea,
			Point mousePosition, boolean bKeyframe) {
		this.rectArea = rectArea;
		this.tile = rectToSend;
		this.bKeyFrame = bKeyframe;
		this.mousePosition = mousePosition;
	}
	public boolean isKeyFrame() {
		return this.bKeyFrame;
	}
	public int getWidth() {
		return this.rectArea.width;
	}
	public int getHeight() {
		return this.rectArea.height;
	}
	public Point getMousePosition() {
		return this.mousePosition;
	}
	public AbstractTileInfo getTile(){
		return this.tile;
	}
	public Rectangle getRectArea(){
		return this.rectArea;
	}
	public boolean isLastScrape() {
		return this.rectArea != null;
	}
	public PartialScrapeData(DataInputStream in) throws IOException{
		boolean bLastTile = in.read() != 0;
		if(bLastTile){
			bKeyFrame = in.read() != 0;
			rectArea = new Rectangle(in.readInt(), in.readInt());
			mousePosition = new Point(in.readInt(), in.readInt()); 
		}
		byte[] hash = new byte[FILE_NAME_LENGTH];
		in.readFully(hash);
		String name = new String(hash);
		//System.out.println("got name: " + name);
		int length = in.readInt();			
		System.out.println("jpeg length: " + length);
		Point point = new Point(in.readInt(), in.readInt());
		if(length > 0){
			byte[] jpeg = new byte[length]; 
			in.readFully(jpeg);
			tile = new CompressedScreenScrape(jpeg, point.x, point.y, name);
		}else{
			tile = new BackReferenceScreenScrape(point.x, point.y, name);
		}
	}
	public void writeToDataOututStream(DataOutputStream out) throws IOException{
		if(this.rectArea != null){
			out.write(1);
			out.write(this.bKeyFrame ? 1 : 0);
			out.writeInt(this.rectArea.width);
			out.writeInt(this.rectArea.height);
			out.writeInt(this.mousePosition.x);
			out.writeInt(this.mousePosition.y);
		}else{
			out.write(0);
		}
		int length = 0;
		if(this.tile instanceof CompressedScreenScrape){
			length = ((CompressedScreenScrape)tile).getByteCount();			
		}
		byte[] hashBytes = tile.getHash().getBytes();
		out.write(hashBytes, 0, FILE_NAME_LENGTH);
		out.writeInt(length);			
		out.writeInt(tile.x);
		out.writeInt(tile.y);
		if(length > 0){
			out.write(((CompressedScreenScrape)tile).getBytes());
		}
	}
	public String toString() {
		String str = super.toString();
		str += "\n\tname: " + this.tile.getHash();
		str += "\n\tx: " + this.tile.x;
		str += "\n\ty: " + this.tile.y;
		str += "\n\tlength: " + (this.tile instanceof CompressedScreenScrape 
			? ((CompressedScreenScrape)this.tile).getByteCount() : 0);
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
		PartialScrapeData data = new PartialScrapeData(scrape2,
				new Rectangle(1024, 1280), new Point(50, 300), true);
		System.out.println(data);
		DataOutputStream dos = new DataOutputStream(boss);
		data.writeToDataOututStream(dos);
		dos.close();
		ByteArrayInputStream bais = new ByteArrayInputStream(boss.toByteArray());
		DataInputStream dis = new DataInputStream(bais);
		PartialScrapeData readData = new PartialScrapeData(dis);
		System.out.println(readData);
	}
	*/
}
