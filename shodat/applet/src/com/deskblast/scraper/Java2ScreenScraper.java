package com.deskblast.scraper;

import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.MouseInfo;
import java.awt.Point;
import java.awt.PointerInfo;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Robot;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;

import javax.imageio.ImageIO;

import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGEncodeParam;
import com.sun.image.codec.jpeg.JPEGImageEncoder;

public class Java2ScreenScraper
    extends AbstractScreenScraper
{

	private static final Point DEFAULT_POINT = new Point(50, 50);
	
	public byte[] doJpegCompression(int[] pixels, int iWidth, int iHeight) {
    	byte[] baRet = null;
    	try{
    		long start = System.currentTimeMillis();
	    	ByteArrayOutputStream baos = new ByteArrayOutputStream();
	        JPEGImageEncoder jpeg = JPEGCodec.createJPEGEncoder(baos);
	        BufferedImage bi = new BufferedImage(
	        				iWidth, iHeight, BufferedImage.TYPE_INT_RGB);
	        bi.setRGB(0, 0, iWidth, iHeight, pixels, 0, iWidth);
	        //bi.set
	        JPEGEncodeParam parm = jpeg.getDefaultJPEGEncodeParam(bi);
	        parm.setQuality(0.65f, false);
	        jpeg.encode(bi, parm);
	        baRet = baos.toByteArray();
	        bi.flush();
    	}catch(Exception e){
    		e.printStackTrace();
    	}
    	return baRet;
	}
	
	public byte[] doGifCompression(int[] pixels, int width, int height){
		byte[] baRet = null;
    	try{
    		long start = System.currentTimeMillis();
	    	ByteArrayOutputStream baos = new ByteArrayOutputStream();
	    	GifEncoder gifEncoder = new GifEncoder(pixels, width, height, baos);
	    	if(gifEncoder.encode()){
	    		baRet = baos.toByteArray();
	    	}
    	}catch(Exception e){
    		e.printStackTrace();
    	}
    	return baRet;
	}
	
	public byte[] doPngCompression(int[] iaPixels, int iWidth, int iHeight) {
		byte[] baRet = null;
    	try{
    		long start = System.currentTimeMillis();
	    	ByteArrayOutputStream baos = new ByteArrayOutputStream();
	        BufferedImage bi = new BufferedImage(
	        				iWidth, iHeight, BufferedImage.TYPE_INT_RGB);
	        bi.setRGB(0, 0, iWidth, iHeight, iaPixels, 0, iWidth);
	        ImageIO.write(bi, "PNG", baos);	        
	        baRet = baos.toByteArray();
	        bi.flush();
    	}catch(Exception e){
    		e.printStackTrace();
    	}
    	return baRet;
	}
	
	public Java2ScreenScraper()
    throws Exception{
        super();
        robot = new Robot();        
    }

    public int[] grabPixels(int[] pixels, Rectangle rect, Rectangle rectObscure)
    throws Exception{
        BufferedImage bImage = grabBufferedImage(rect);
        Graphics g = bImage.getGraphics();
        if(biMedallion == null){
        	Font font = new Font("SansSerif", Font.BOLD, 26);
        	
        	biMedallion = new BufferedImage(rectObscure.width, rectObscure.height, 
        			BufferedImage.TYPE_INT_RGB);
        	Graphics textGraphics = biMedallion.getGraphics();
        	textGraphics.setFont(font);
        	((Graphics2D)textGraphics).setRenderingHint(RenderingHints.KEY_ANTIALIASING,
        	        RenderingHints.VALUE_ANTIALIAS_ON);
        	FontMetrics fm = textGraphics.getFontMetrics(font);
        	Rectangle2D dim = fm.getStringBounds("shodat", textGraphics);        	
        	textGraphics.drawString("shodat", (int)
        			((rectObscure.width / 2) - (dim.getWidth() / 2)), 
        			(int) dim.getHeight() - fm.getMaxDescent());
        }
        g.drawImage(biMedallion, rectObscure.x, rectObscure.y, 
        		rectObscure.width, rectObscure.height, 
        		null);
        
        bImage.getRGB(0, 0, rect.width, rect.height, pixels, 0, rect.width);
        bImage.flush();
        return pixels;
    }
    public void stopScraping(){
        super.stopScraping();
        robot = null;
    }
    private BufferedImage grabBufferedImage(Rectangle rect){
    	return robot.createScreenCapture(rect);        
    }    
    public Point getMousePosition() {
        PointerInfo pointerInfo = MouseInfo.getPointerInfo();
        if(pointerInfo != null){
        	return new Point(pointerInfo.getLocation().x, 
                    pointerInfo.getLocation().y);	
        }
        return DEFAULT_POINT;        
    }
    private Robot robot;
    private BufferedImage biMedallion;
    
    public static void main(String[] args) throws Exception{
    	Java2ScreenScraper scraper = new Java2ScreenScraper();
    	BufferedImage bi = ImageIO.read(new File("../reference_desktops/Screenshot_1.png"));
    	int width = bi.getWidth();
    	int height = bi.getHeight();
    	int[] rgb = new int[width*height];
    	bi.getRGB(0, 0, width, height, rgb, 0, width);
    	Rectangle rect1 = new Rectangle(0, 0, width, height);
    	Rectangle rect2 = new Rectangle(450, 300, 120, 300);
    	int[] subPixels = PixelUtil.getSubtilePixels(rect2, rgb, width);
    	byte[] ba = scraper.doJpegCompression(subPixels, rect2.width, rect2.height);
    	FileOutputStream fos = new FileOutputStream("out_" + System.currentTimeMillis() + ".jpg");
    	fos.write(ba);
    	fos.close();
    }

}
