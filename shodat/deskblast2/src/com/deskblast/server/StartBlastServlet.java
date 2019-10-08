package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

public class StartBlastServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest)
			throws ServletException {
		throw new ServletException("start blast via POST request");
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		throw new RuntimeException("this should not be used anymore");
//		BlastRegistrar.getBlastRegistrar(httpServletRequest)
//			.startBlast((BlasterCredentials)object);
//		return null;
	} 

}
