package com.deskblast.server.json;

public class ScrapeInfo {

	private int x;
	private int y;
//	private int width;
//	private int height;
	private String name;

	public ScrapeInfo(int x, int y, String name) {
        super();
        this.x = x;
        this.y = y;
//        this.width = width;
//        this.height = height;
        this.name = name;
    }
    public int getX() {
		return x;
	}
	public int getY() {
		return y;
	}
//	public int getWidth(){
//		return width;
//	}
//	public int getHeight(){
//		return height;
//	}
	public String getName() {
		return name;
	}
	
	public String toString(){
		return name + " x: " + x + " y: " + y;
	}
	
}
