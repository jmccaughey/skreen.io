package com.deskblast.scraper;

import java.awt.AWTException;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class SubTileTest {

	public static void main(String[] args) throws AWTException, IOException{
		Robot robot = new Robot();
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		BufferedImage bi = robot.createScreenCapture(new Rectangle(dim));
		BufferedImage biRegion = new BufferedImage(256, 512, BufferedImage.TYPE_INT_RGB);
		int[] allPixels = bi.getRGB(0, 0, dim.width, dim.height, null, 0, dim.width);
		int start = (dim.width * 512) + 512; 
		biRegion.setRGB(0, 0, 256, 512, allPixels, start, 
				dim.width
				//256
				);
		ImageIO.write(biRegion, "JPG", new File("scrape_" + System.currentTimeMillis() + ".jpg"));
	}
	
}
