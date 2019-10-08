package com.deskblast.server;

import java.io.IOException;

import javax.inject.Singleton;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

@Singleton
public class JoinServlet extends javax.servlet.http.HttpServlet{
	
	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		
		// write minimal html page, with variables as JSON
		// includes prototype.js and json_client_sprites_wait.js
		
		String joinCode = req.getRequestURI().substring(1);
		log.info("got joinCode: " + joinCode);
		req.setAttribute("joinCode", joinCode);		
		req.getRequestDispatcher("/WEB-INF/test.jsp").forward(req, resp);
	}
	

}
