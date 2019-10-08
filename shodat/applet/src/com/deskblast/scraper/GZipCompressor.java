package com.deskblast.scraper;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.GZIPOutputStream;

public class GZipCompressor {

	public static byte[] compress(byte[] ba) throws IOException{
		byte[] ret;
		//System.out.println("compress: before " + ba.length);
		ByteArrayOutputStream baos = new ByteArrayOutputStream(ba.length);
		GZIPOutputStream gzo = new GZIPOutputStream(baos);
		gzo.write(ba);
		gzo.close();
		ret = baos.toByteArray();
		//System.out.println("compress before: " + ba.length 
		//		+ " after: " + ret.length);
		return ret;
	}
	
	public static void main(String[] args) throws IOException{
		String fileName = "/home/johnm/screen_scrape.jpg";
		File file = new File(fileName);
		byte[] bytes = new byte[(int)file.length()];
		DataInputStream dis = new DataInputStream(new FileInputStream(file));
		dis.readFully(bytes);
		dis.close();
		byte[] gzipped = GZipCompressor.compress(bytes);
		File fileOut = new File(fileName + ".gz");
		DataOutputStream dos = new DataOutputStream(new FileOutputStream(fileOut));
		dos.write(gzipped);
		dos.close();
	}
	
}
