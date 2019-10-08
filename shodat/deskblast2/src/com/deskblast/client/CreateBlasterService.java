package com.deskblast.client;

import com.google.gwt.rpc.client.RpcService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("createBlasterRpc")
public interface CreateBlasterService extends RpcService {

	RpcBlasterCredentials createBlaster(String ownerName, String joinerName);
	
}
