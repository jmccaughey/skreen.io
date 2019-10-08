package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface CreateBlasterServiceAsync {

	void createBlaster(String ownerName, String joinerName,
			AsyncCallback<RpcBlasterCredentials> callback);

}
