package com.deskblast.server;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.Random;

public class River {
	
	
	public static void main(String[] args) throws IOException{
		int target = new Random().nextInt(100);
		DataInputStream dis = new DataInputStream(System.in);
		while(true){
			System.out.println("Hi River. I have a number between 1 and 100. What is your guess?");
			int number = dis.readInt();
			System.out.println(number + " is a good guess!");
		}
	}

}
