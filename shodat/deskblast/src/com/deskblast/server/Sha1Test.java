package com.deskblast.server;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.sun.org.apache.xerces.internal.impl.dv.util.Base64;

public class Sha1Test {

	public static void main(String[] args) throws NoSuchAlgorithmException, IOException{
		MessageDigest md = MessageDigest.getInstance("SHA-1");
		File file = new File(args[0]);
		byte[] bytes = new byte[(int)file.length()];
		DataInputStream dis = new DataInputStream(new FileInputStream(file));
		dis.readFully(bytes);
		System.out.println(Base64.encode(md.digest(bytes)));
	}

}
