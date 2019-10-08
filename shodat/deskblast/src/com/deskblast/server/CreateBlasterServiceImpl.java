package com.deskblast.server;

import com.deskblast.client.CreateBlasterService;
import com.deskblast.client.RpcBlasterCredentials;
import com.google.gwt.rpc.server.RpcServlet;

public class CreateBlasterServiceImpl extends RpcServlet
		implements CreateBlasterService {

	@Override
	public RpcBlasterCredentials createBlaster(String ownerName, String joinerName) {
		BlastRegistrar blastRegistrar = 
			BlastRegistrar.getBlastRegistrar(this.getThreadLocalRequest());
		RpcBlasterCredentials rpcBlasterCredentials;
		try {
			rpcBlasterCredentials = blastRegistrar.createBlaster(ownerName, joinerName);
		} catch (Exception e) {
			e.printStackTrace();
			throw new RuntimeException("problem with pre-join", e);
		}
		return rpcBlasterCredentials;	
	}

}
