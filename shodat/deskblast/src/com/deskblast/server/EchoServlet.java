package com.deskblast.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class EchoServlet extends javax.servlet.http.HttpServlet {

	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		
		InputStream is = req.getInputStream();
		OutputStream os = resp.getOutputStream();
		int read = is.read();
		while(read > -1){
			os.write(read);
			read = is.read();
		}
		
	}


	@Override
	protected void doPut(HttpServletRequest req, HttpServletResponse resp)
                throws ServletException, IOException {
		doPost(req, resp);
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
