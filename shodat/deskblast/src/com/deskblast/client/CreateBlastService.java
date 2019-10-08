package com.deskblast.client;

import com.google.gwt.rpc.client.RpcService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("createBlastRpc")
public interface CreateBlastService extends RpcService {

	RpcBlasterCredentials createBlast(String ownerName);
	
}
