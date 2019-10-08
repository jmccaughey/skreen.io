package com.deskblast.scraper;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Panel;
import java.awt.Rectangle;
import java.util.Random;
import java.util.Vector;

public class RectMerger{

// BEGIN TEST CODE	
	private static final int TILE_WIDTH = 100;
	private static final int TILE_HEIGHT = 100;
	private static final int RECT_COUNT = 30;
	private static final Dimension BOUNDS = new Dimension(1024, 768);
	
	private static Vector generateRandomSquares(Dimension dimArea, int iTileWidth, int iTileHeight, int iCount){
		Rectangle[] rectsAll = TileMaker.makeTiles(dimArea, iTileWidth, iTileHeight);
		int[] aiIndices = generateRandomInts(rectsAll.length, iCount);
		Vector vect = new Vector();
		for(int i = 0; i < aiIndices.length; i++){
			vect.addElement(rectsAll[aiIndices[i]]);
		}
		return vect;
	}
	
	private static int[] generateRandomInts(int iRange, int iCount){
		Random rand = new Random(System.currentTimeMillis());	
		Vector vectInts = new Vector();		
		while(vectInts.size() < iCount){
			Integer objInt = new Integer(Math.abs(rand.nextInt()) % iRange);			
			if(!vectInts.contains(objInt)){
				vectInts.addElement(objInt);
			}
		}
		int[] ia = new int[iCount];
		for(int i = 0; i < ia.length; i++){
			ia[i] = ((Integer)vectInts.elementAt(i)).intValue();
		}
		return ia;
	}
	
	public static void main(String[] args) throws Exception{

		Frame frameBefore = new Frame("before");
//		Frame frameAfter = new Frame("after");
		
		Vector testRects = generateRandomSquares(
				BOUNDS,//new Dimension(1024, 768), 
				TILE_WIDTH, 
				TILE_HEIGHT,
				RECT_COUNT
		);		
		System.out.println("before: [" + testRects.size() + " rects]");
		dumpRects(testRects);
		frameBefore.add(new RectDrawingPanel(testRects, BOUNDS));
		frameBefore.setVisible(true);
		frameBefore.setBounds(0,0,BOUNDS.width,BOUNDS.height);//
		System.in.read();
		//Vector vectMerged = 
//		Rectangle[] mergedRects = 
		mergeRects(testRects, TILE_WIDTH * TILE_HEIGHT * 2);
		frameBefore.invalidate();
		frameBefore.repaint();
//		RectDrawingPanel
		System.out.println("after: [" 
				+ testRects.size() 
				+ " rects]");
		dumpRects(testRects);
		System.in.read();
		System.in.read();
		frameBefore.dispose();
	}
// END TEST CODE	

	public static Rectangle[] mergeRectangles(Rectangle[] rects, int maxArea){
		// make defensive copy of input
		Vector vectRects = new Vector();
		for(int i = 0; i < rects.length; i++){
			vectRects.addElement(new Rectangle(rects[i]));
		}
		mergeRects(vectRects, maxArea);
		Rectangle[] rectsReturn = new Rectangle[vectRects.size()];
		for(int i = 0; i < vectRects.size(); i++){
			rectsReturn[i] = (Rectangle)vectRects.elementAt(i);
		}
		return rectsReturn;
	}
	
	private static void mergeRects(Vector vectRects, int maxArea){
		// if any can be merged, merge and call back into this
		for(int i = 0; i < vectRects.size(); i++){			
			for(int j = i+1; j < vectRects.size(); j++){
				Rectangle rect1 = (Rectangle)vectRects.elementAt(i);
				Rectangle rect2 = (Rectangle)vectRects.elementAt(j);
				if(canMerge(rect1, rect2, maxArea)){
					// remove both
					// put new, merged on where first one was
					//System.out.println("merging " + rect1 + " and " + rect2);	
					vectRects.removeElement(rect1);
					vectRects.removeElement(rect2);
					Rectangle merged = mergeRects(rect1, rect2);
					//System.out.println("...result is: " + merged);
					vectRects.insertElementAt(merged, i);
					mergeRects(vectRects, maxArea);
				}
			}
		}
	}
	
	private static boolean canMerge(Rectangle rect1, Rectangle rect2, int maxArea){
		int iArea = getRectArea(rect1) + getRectArea(rect2);
		return iArea == getRectArea(rect1.union(rect2)) && iArea <= maxArea;
	}
	
	private static Rectangle mergeRects(Rectangle rect1, Rectangle rect2){
		return rect1.union(rect2);
	}
	
	private static int getRectArea(Rectangle rect){
		return rect.height * rect.width;
	}
	
//	
	public static void dumpRects(Vector rects){
		for(int i = 0; i < rects.size(); i++){
			System.out.println(rects.elementAt(i));
		}
	}
	
}
//
class RectDrawingPanel extends Panel{
	
	Vector vectRects;
	Dimension area;
	
	public RectDrawingPanel(Vector vectRects, Dimension area){
		this.area = area;
		this.vectRects = vectRects;
	}
	
	public void paint(Graphics g){
		g.setColor(Color.black);
		g.fillRect(0, 0, area.width, area.height);//, y, width, height)
		g.setColor(Color.red);
		for(int i = 0; i < this.vectRects.size(); i++){
			Rectangle rect = (Rectangle)vectRects.elementAt(i);
			g.drawRoundRect(rect.x, rect.y, rect.width, rect.height, 20, 20);
		}
	}
	
}