package com.deskblast.server;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class TokenServlet extends javax.servlet.http.HttpServlet {

	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		try {
			// Call a random compliment, built with Java5 enums!
			resp.getWriter().println("HI");
		} catch (Exception e) {
			// completely unexpected exception!
			e.printStackTrace(resp.getWriter());
		}
	}

}
