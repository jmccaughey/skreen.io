package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

public class BlastMessagesServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest request)
			throws ServletException {
		int blastId = Integer.parseInt((String)request.getParameter("blastId"));
		int blasterId = Integer.parseInt((String)request.getParameter("blasterId"));
		int messageNumber = Integer.valueOf((String)request.getParameter("messageNumber"));
		String key = (String)request.getParameter("key");
		Object objReturn = null;
		//try {
			objReturn = BlastRegistrar.getBlastRegistrar(request).getBlast(blastId)
					.getRpcMessages(blasterId, key)
					//.getMessages(blasterId, key, messageNumber)
				;
//		} catch (InterruptedException e) {
//			e.printStackTrace();
//			throw new ServletException("get messages call interrupted", e);
//		}
		return objReturn;
	}

	@Override
	public Object handlePostRequest(HttpServletRequest request,
			Object object) throws ServletException {
		int blastId = Integer.parseInt((String)request.getParameter("blastId"));
		String key = (String)request.getParameter("key");
		//BlastRegistrar.getBlastRegistrar(request)
		//	.getBlast(blastId).handleMessage((BlastMessage)object, key);
		throw new RuntimeException("this should not be used anymore");
	} 
	
//	private BlasterCredentials buildCredentials(HttpServletRequest request){
//		return new BlasterCredentials()
//	}

}
