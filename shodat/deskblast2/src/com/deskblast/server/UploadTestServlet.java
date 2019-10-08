package com.deskblast.server;

import java.io.IOException;
import java.io.InputStream;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class UploadTestServlet extends
		javax.servlet.http.HttpServlet {
	
	protected void doPost(HttpServletRequest httpServletRequest,
			HttpServletResponse httpServletResponse) throws ServletException,
			IOException {
		System.out.println("content-length: " + httpServletRequest.getContentLength());
		long start = System.currentTimeMillis();
		byte[] buff = new byte[4096];
		InputStream is = httpServletRequest.getInputStream();
		while(is.read(buff) > -1){
			long now = System.currentTimeMillis();
			if(now != start){
				System.out.println(now-start);
				start = now;
			}
		}
	
	}

}
