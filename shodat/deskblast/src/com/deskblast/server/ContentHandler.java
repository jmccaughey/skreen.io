package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.server.json.BlastMessage;

public interface ContentHandler {

	public String getName();

	public void handleContent(RpcMessage rpcMessage,
			BlastContext messageSender);
	
	public void handleJsonContent(BlastMessage message, BlastContext messageSender);

}
