package com.deskblast.server;

import com.deskblast.client.JoinBlastService;
import com.deskblast.client.RpcBlasterCredentials;
import com.google.gwt.rpc.server.RpcServlet;

public class JoinBlastServiceImpl extends RpcServlet
		implements JoinBlastService {

	@Override
	public void joinBlast(RpcBlasterCredentials ownerCredentials) {
		try {
			BlastRegistrar.getBlastRegistrar(
					this.getServletContext()).joinBlast(ownerCredentials);
		} catch (Exception e) {
			e.printStackTrace();
			throw new RuntimeException("problem joining blast", e);
		}
	}

}
