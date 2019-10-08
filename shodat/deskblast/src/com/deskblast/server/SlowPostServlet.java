package com.deskblast.server;

import java.io.IOException;
import java.io.InputStream;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class SlowPostServlet extends HttpServlet {

	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		// this is from the activex/plugin/applet, reporting that the showing user
		// has stopped showing
		log.info("in service method");
		InputStream is = request.getInputStream();
		//int read = 0;
		byte[] buff = new byte[1024];
		long total = 0;
		for(long read = is.read(buff); read > -1; read = is.read(buff)){
			total += read;
			System.out.println("read " + total);
			try {
				Thread.sleep(20);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		response.getWriter().write("thanks!");
	}


	
}
