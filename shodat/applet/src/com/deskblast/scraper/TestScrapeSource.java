package com.deskblast.scraper;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;

public class TestScrapeSource implements ScrapeSource {

	private List<TestImage> images = new ArrayList<TestImage>();
	
	class TestImage{		
		private int[] pixels;
		private int width;
		private int height;
		private String name;

		TestImage(File file) throws IOException{
			name = file.getAbsolutePath();
			BufferedImage bufferedImage = ImageIO.read(file);
			width = bufferedImage.getWidth();
			height = bufferedImage.getHeight();
			pixels = new int[width*height];
			bufferedImage.getRGB(0, 0, width, height, pixels, 0, width);
		}
		public int[] getPixels() {
			return pixels;
		}
		public int getWidth() {
			return width;
		}
		public int getHeight() {
			return height;
		}
		public String getName() {
			return name;
		}
	}
	public TestScrapeSource(File dir) throws IOException {
		File[] files = dir.listFiles(new FileFilter() {
			@Override
			public boolean accept(File pathname) {
				String path = pathname.getAbsolutePath().toLowerCase();
				return path.endsWith(".bmp") 
				//|| path.endsWith(".png")
				; 
			}
		});
		for(File file : files){
			images.add(new TestImage(file));
		}
	}
	@Override
	public void getNextScrape(int[] pixels, Rectangle rectangle) {
		TestImage testImage = images.remove(0);
		System.out.println(testImage.getName());
		if(rectangle.width != testImage.getWidth() || rectangle.getHeight() != testImage.getHeight()
				|| pixels.length != testImage.getPixels().length){
			throw new RuntimeException("scrape bounds must match image bounds");
		}
		System.arraycopy(testImage.getPixels(), 0, pixels, 0, pixels.length);
	}
	@Override
	public boolean hasMoreScrapes() {
		return !images.isEmpty();
	}
	@Override
	public Rectangle getScrapeRegion() {
		return new Rectangle(images.get(0).getWidth(), images.get(0).getHeight());
	}

}
