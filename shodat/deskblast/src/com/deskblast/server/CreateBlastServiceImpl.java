package com.deskblast.server;

import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.CreateBlastService;
import com.deskblast.client.RpcBlasterCredentials;
import com.google.gwt.rpc.server.RpcServlet;

public class CreateBlastServiceImpl extends RpcServlet
		implements CreateBlastService {

	@Override
	public RpcBlasterCredentials createBlast(String ownerName) {
		HttpServletRequest httpServletRequest = this.getThreadLocalRequest(); 
		String remoteHost = httpServletRequest.getRemoteHost(); 
		//httpServletRequest.getRem
		if("localhost".equals(remoteHost) || remoteHost.startsWith("127.0.")
				|| remoteHost.startsWith("0")){
			BlastRegistrar blastRegistrar = 
				BlastRegistrar.getBlastRegistrar(httpServletRequest);
			RpcBlasterCredentials blasterCredentials = 
				blastRegistrar.createBlast(ownerName);
			return blasterCredentials;
		}else{
			System.out.println("non-local host request: returning null");
		}
		return null;
	}

}
