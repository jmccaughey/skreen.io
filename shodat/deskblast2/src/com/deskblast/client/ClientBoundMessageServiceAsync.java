package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface ClientBoundMessageServiceAsync {

	void getMessages(int blastId, int blasterId, String key,
			AsyncCallback<RpcMessage[]> callback);

}
