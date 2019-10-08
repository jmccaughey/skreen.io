package com.deskblast.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

public interface ServerBoundMessageServiceAsync {

	void sendMessages(RpcMessage[] messages, int blastId, int blasterId,
			String key, AsyncCallback<Void> callback);

}
