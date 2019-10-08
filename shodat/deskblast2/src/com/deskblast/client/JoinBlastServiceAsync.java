package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface JoinBlastServiceAsync {

	void joinBlast(RpcBlasterCredentials ownerCredentials,
			AsyncCallback<Void> callback);

}
