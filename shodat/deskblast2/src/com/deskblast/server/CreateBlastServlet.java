package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.server.json.BlasterCredentials;
import com.deskblast.server.json.BlasterInfo;

public class CreateBlastServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest) {
		//throw new RuntimeException("this should not be used anymore");
		String remoteHost = httpServletRequest.getRemoteHost(); 
		if("localhost".equals(remoteHost) || "127.0.0.1".equals(remoteHost)){
			BlastRegistrar blastRegistrar = 
				BlastRegistrar.getBlastRegistrar(httpServletRequest);
			String ownerName = httpServletRequest.getParameter("ownerName"); 
			RpcBlasterCredentials blasterCredentials = 
				blastRegistrar.createBlast(ownerName);
			System.out.println("new CreateBlastServlet");
			BlasterInfo bi = new BlasterInfo(
					blasterCredentials.getRpcBlasterInfo().getName(), 
					blasterCredentials.getRpcBlasterInfo().getId());
			BlasterCredentials bc = new BlasterCredentials(
					bi, 
					blasterCredentials.getBlastId(), 
					blasterCredentials.getKey(),
					blasterCredentials.isOwner()); 
			return bc;//blasterCredentials.replace("rpcBlasterInfo", "blasterInfo");
		}
		return null;
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		throw new ServletException("create blast via GET request");
	} 

}
