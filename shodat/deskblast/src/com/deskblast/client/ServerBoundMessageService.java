package com.deskblast.client;

import com.google.gwt.rpc.client.RpcService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("serverBoundMessages")
public interface ServerBoundMessageService extends RpcService {

	void sendMessages(RpcMessage[] messages, int blastId, int blasterId, String key);
	
}
