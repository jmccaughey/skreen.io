package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface StartBlastServiceAsync {

	void startBlast(RpcBlasterCredentials ownerCredentials,
			AsyncCallback<Void> callback);

}
