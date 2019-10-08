package com.deskblast.client;

import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("clientBoundMessages")
public interface ClientBoundMessageService extends 
//RpcService
RemoteService
{

	RpcMessage[] getMessages(int blastId, int blasterId, String key);
	
}
