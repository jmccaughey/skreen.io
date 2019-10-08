package com.deskblast.server;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class PluginInstalledStartScrapingServlet extends HttpServlet {

	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	protected void service(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		// this is from the .net installer, reporting that the user
		// plugin install finished
		log.info("in service method");
		int blastId = Integer.valueOf(request.getParameter("blastId"));
		String key = request.getParameter("key");
		final int blasterId = Integer.parseInt(request.getParameter("blasterId"));
		BlastRegistrar reg = BlastRegistrar.getBlastRegistrar(request);
		reg.getBlast(blastId).handlePluginInstalledStartScraping(blasterId, key);
		//StartScrapingMessage startMsg = StartScrapingMessage.fromJson("{}");
		//startMsg.setSender(new BlasterInfo(){public Long getId(){return blasterId;}});
		//startMsg.setSender(blasterInfo)
//        startMsg.setJavaClass("com.deskblast.server.json.StartScrapingMessage");
        //startMsg.setSender(reg.getBlast(blastId).);
        //startMsg.getSender().setJavaClass(
          //          "com.deskblast.server.json.BlasterInfo");
//		reg.getBlast(blastId).handleMessage(startMsg, key);
				//.handleScreenScrapingStopped(blasterId, key);
		response.getOutputStream().write(13);
	}


	
}
