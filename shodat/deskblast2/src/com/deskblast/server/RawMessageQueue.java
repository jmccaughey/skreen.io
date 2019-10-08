package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.server.json.BlastMessage;

public interface RawMessageQueue {

//	public void addMessage(BlastMessage blastMessage) throws QueueFullException;
	
	public BlastMessage[] getPendingMessages();
	
	public void waitForMessages(int timeoutMillis) throws InterruptedException;

	public void addRpcMessage(RpcMessage blastMessage) throws QueueFullException;
	
	public RpcMessage[] getPendingRpcMessages();
	
//	public void waitForRpcMessages(int timeoutMillis) throws InterruptedException;

	
}
