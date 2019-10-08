package com.deskblast.server.util;

import java.security.SecureRandom;

public class RandomCodeBuilder {

	static SecureRandom secureRandom = new SecureRandom();
	
	public static String buildNDigitCode(int digitCount){
		StringBuffer sb = new StringBuffer();
		for(int i = 0; i < digitCount; i++){
			sb.append(secureRandom.nextInt(10));
		}
		return sb.toString();
	}
	
	public static void main(String[] args){
		System.out.println(buildNDigitCode(6));
	}
	
}
