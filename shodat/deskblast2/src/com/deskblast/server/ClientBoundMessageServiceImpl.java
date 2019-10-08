package com.deskblast.server;

import java.util.Random;

import com.deskblast.client.ClientBoundMessageService;
import com.deskblast.client.RpcMessage;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

public class ClientBoundMessageServiceImpl extends 
//RpcServlet
RemoteServiceServlet
		implements ClientBoundMessageService {

	@Override
	public RpcMessage[] getMessages(int blastId, int blasterId, String key) {
//		try {
//			Thread.currentThread().sleep(new Random().nextInt(300));
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
		return BlastRegistrar.getBlastRegistrar(this.getServletContext()).getBlast(blastId)
			.getRpcMessages(blasterId, key);
//		Long blastId = Long.valueOf((String)request.getParameter("blastId"));
//		Long blasterId = Long.valueOf((String)request.getParameter("blasterId"));
//		int messageNumber = Integer.valueOf((String)request.getParameter("messageNumber"));
//		String key = (String)request.getParameter("key");
//		Object objReturn = null;
//		try {
//			objReturn = BlastRegistrar.getBlastRegistrar(request).getBlast(blastId)
//						.getMessages(blasterId, key, messageNumber);
//		} catch (InterruptedException e) {
//			e.printStackTrace();
//			throw new ServletException("get messages call interrupted", e);
//		}
//		return objReturn;
	}

}
