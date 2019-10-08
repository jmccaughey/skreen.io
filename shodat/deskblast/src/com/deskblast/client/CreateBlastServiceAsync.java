package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface CreateBlastServiceAsync {

	void createBlast(String ownerName,
			AsyncCallback<RpcBlasterCredentials> callback);

}
