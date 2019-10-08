package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.server.json.BlasterCredentials;

public class CreateBlasterServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest) 
	throws ServletException {
		System.out.println("new CreateBlasterServlet");
		BlastRegistrar blastRegistrar = 
			BlastRegistrar.getBlastRegistrar(httpServletRequest);
		String ownerName = httpServletRequest.getParameter("ownerName"); 
		String joinerName = httpServletRequest.getParameter("joinerName"); 
		BlasterCredentials blasterCredentials;
		try {
			blasterCredentials = new BlasterCredentials(blastRegistrar.createBlaster(ownerName, joinerName));
		} catch (Exception e) {
			e.printStackTrace();
			throw new ServletException("problem with pre-join", e);
		}
		return blasterCredentials;
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		throw new ServletException("create blast via GET request");
	} 

}
