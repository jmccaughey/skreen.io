package com.deskblast.client;

import com.google.gwt.rpc.client.RpcService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("startBlastRpc")
public interface StartBlastService extends RpcService {

	 void startBlast(RpcBlasterCredentials ownerCredentials);
	
}
