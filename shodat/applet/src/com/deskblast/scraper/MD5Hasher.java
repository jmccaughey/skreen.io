package com.deskblast.scraper;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import javax.imageio.ImageIO;

public class MD5Hasher {

	static byte[] getBytes(Rectangle rect, int[] ia, int scanline){
		byte[] ba = new byte[(rect.width * rect.height) * 4];
		int baIndex = 0;
		int start = rect.x + (rect.y * scanline);
		for(int j = 0; j < rect.height; j++){			
			for(int i = 0; i < rect.width; i++){				
				ba[baIndex++] = (byte)((ia[start+i] >> 24));
				ba[baIndex++] = (byte)((ia[start+i] >> 16));
				ba[baIndex++] = (byte)((ia[start+i] >> 8));
				ba[baIndex++] = (byte)(ia[start+i]);
			}			
			start += scanline;
		}
		return ba;
	}
	
	public static String doMD5Hash(Rectangle rect, int[] ia, int scanline) {
		byte[] ba = getBytes(rect, ia, scanline);
		return doMD5Hash(ba); 
	}
	static String doMD5Hash(byte[] ba) {
        MessageDigest md;
        try {
            md = MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("MD5 not available");
        }
        byte[] hash = md.digest(ba);
        return convertToHex(hash);
    }
	
	// following found here:
    // http://www.anyexample.com/programming/java/java_simple_class_to_compute_md5_hash.xml
    private static String convertToHex(byte[] data) {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < data.length; i++) {
            int halfbyte = (data[i] >>> 4) & 0x0F;
            int two_halfs = 0;
            do {
                if ((0 <= halfbyte) && (halfbyte <= 9))
                    buf.append((char) ('0' + halfbyte));
                else
                    buf.append((char) ('a' + (halfbyte - 10)));
                halfbyte = data[i] & 0x0F;
            } while(two_halfs++ < 1);
        }
        return buf.toString();
    }
	
    public static void main(String[] args) throws IOException{
    	BufferedImage biA = ImageIO.read(new File("../reference_desktops/Screenshot_1.png"));
    	//Rectangle rect = new Rectangle
    	int width = biA.getWidth();
    	int height = biA.getHeight();
    	int[] rgb = new int[width * height];
    	biA.getRGB(0, 0, width, height, rgb, 0, width);
    	String hash = doMD5Hash(new Rectangle(width, height), rgb, width);
    	System.out.println(hash);
    	
    	ByteBuffer byteBuffer = ByteBuffer.allocate(rgb.length * 4);
    	IntBuffer intBuffer = byteBuffer.asIntBuffer();
    	intBuffer.put(rgb);
    	byte[] newBa = byteBuffer.array();
    	System.out.println("byte array: " + doMD5Hash(newBa));
    	
    	ByteArrayOutputStream baos = new ByteArrayOutputStream();
    	DataOutputStream dos = new DataOutputStream(baos);
    	for(int i = 0; i < rgb.length; i++){
    		dos.writeInt(rgb[i]);
    	}
    	dos.flush();
    	System.out.println("byte array: " + doMD5Hash(baos.toByteArray()));
    	
    	byte[] baFromRect = getBytes(new Rectangle(width, height), rgb, width);
    	
    	byte[] ba = new byte[(width*height)*4];
    	for(int i = 0; i < rgb.length; i++){
    		int x = rgb[i];
    		int j = i << 2;
			ba[j++] = (byte) (x >> 24);
			ba[j++] = (byte) (x >> 16);
			ba[j++] = (byte) (x >> 8);
			ba[j++] = (byte) (x);
		}
    	if(ba.length != baFromRect.length){
    		System.out.println("unequal lengths");
    	}
    	for(int i = 0; i < ba.length; i++){
    		if(ba[i] != baFromRect[i]){
    			System.out.println("not equal at " + i);
    			break;
    		}
    	}
    	String hash3 = doMD5Hash(ba);
    	System.out.println(hash3);
    	
    	Rectangle sub = new Rectangle(30, 70, 300, 150);
    	String hash2 = doMD5Hash(sub, rgb, width);
    	System.out.println(hash2);
    	
    	byte[] baRgb = new byte[(sub.width*sub.height)*4];
    	int[] fromRgb = new int[sub.width*sub.height];
    	int id = 0;
    	for(int i = 0; i < sub.height; i++){
    		for(int j = 0; j < sub.width; j++){
    			int x = sub.x + j;
    			int y = sub.y + i;
    			fromRgb[id++] = biA.getRGB(x, y);
    		}
    	}
//    	
//    	biA.getRGB(sub.x, sub.y,
//    			width, height,
//    			//sub.width, sub.height, 
//    			fromRgb, 0, width);
//    	
    	for(int i = 0; i < fromRgb.length; i++){
    		int x = fromRgb[i];
    		int j = i << 2;
			baRgb[j++] = (byte) (x >> 24);
			baRgb[j++] = (byte) (x >> 16);
			baRgb[j++] = (byte) (x >> 8);
			baRgb[j++] = (byte) x;
		}

    	System.out.println(doMD5Hash(baRgb));
    }
}
