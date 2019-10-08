package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.client.ServerBoundMessageService;
import com.google.gwt.rpc.server.RpcServlet;

public class ServerBoundMessageServiceImpl extends RpcServlet
		implements ServerBoundMessageService {

	@Override
	public void sendMessages(RpcMessage[] messages, int blastId,
			int blasterId, String key) {
		Blast blast = BlastRegistrar.getBlastRegistrar(
				this.getServletContext()).getBlast(blastId); 
		for(RpcMessage message : messages){
			blast.handleRpcMessage(message, key);
		}
	}

}
