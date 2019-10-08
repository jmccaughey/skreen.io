package com.deskblast.client;

import com.google.gwt.rpc.client.RpcService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("joinBlastRpc")
public interface JoinBlastService extends RpcService {

	 void joinBlast(RpcBlasterCredentials ownerCredentials);
	
}
