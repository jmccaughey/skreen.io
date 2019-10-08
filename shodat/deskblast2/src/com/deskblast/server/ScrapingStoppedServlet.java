package com.deskblast.server;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class ScrapingStoppedServlet extends HttpServlet {

	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	protected void service(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		// this is from the activex/plugin/applet, reporting that the showing user
		// has stopped showing
		log.info("in service method");
		int blastId = Integer.parseInt(request.getParameter("blastId"));
		String key = request.getParameter("key");
		int blasterId = Integer.parseInt(request.getParameter("blasterId"));		
		BlastRegistrar.getBlastRegistrar(request).getBlast(blastId)
				.handleScreenScrapingStopped(blasterId, key);
		response.getOutputStream().write(13);
	}


	
}
