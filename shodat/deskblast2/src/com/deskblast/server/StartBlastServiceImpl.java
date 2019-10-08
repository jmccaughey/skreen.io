package com.deskblast.server;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.StartBlastService;
import com.google.gwt.rpc.server.RpcServlet;

public class StartBlastServiceImpl extends RpcServlet
		implements StartBlastService {

	@Override
	public void startBlast(RpcBlasterCredentials ownerCredentials) {
		BlastRegistrar.getBlastRegistrar(
				this.getServletContext()).startBlast(ownerCredentials);
	}

}
