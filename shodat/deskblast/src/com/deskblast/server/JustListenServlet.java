package com.deskblast.server;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.Enumeration;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class JustListenServlet extends javax.servlet.http.HttpServlet {

	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		
		DataInputStream dis = new DataInputStream(req.getInputStream());
		//DataOutputStream os = resp.getOutputStream();
		byte[] buffer = new byte[req.getContentLength()];
		dis.readFully(buffer);
		resp.getWriter().write("got " + buffer.length + " bytes.");
	}

	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		Enumeration enumer = req.getHeaderNames();
		while(enumer.hasMoreElements()){
			System.out.println(req.getHeader((String)enumer.nextElement()));
		}
		//super.doGet(req, resp);
		resp.getWriter().write(req.getQueryString());
	}
	
	


	
	
}
