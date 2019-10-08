package com.deskblast.server;

import java.util.LinkedList;
import java.util.Queue;

import com.deskblast.client.RpcMessage;
import com.deskblast.server.json.BlastMessage;

public class RawMessageQueueImpl implements RawMessageQueue {

	private Queue<RpcMessage> queue = new LinkedList<RpcMessage>();
	private Object lock = new Object();
	private static int MAX_SIZE = 100;
	private boolean empty = true;
	
	@Override
	public synchronized void addRpcMessage(RpcMessage blastMessage)
			throws QueueFullException {
		if(queue.size() >= MAX_SIZE){
			throw new QueueFullException();
		}
		queue.add(blastMessage);
		setEmpty(false);
		synchronized(lock){
			lock.notifyAll();
		}
	}
	public synchronized void addMessage(BlastMessage blastMessage) throws QueueFullException {
		if(queue.size() >= MAX_SIZE){
			throw new QueueFullException();
		}
		//queue.add(blastMessage);
		setEmpty(false);
		synchronized(lock){
			lock.notifyAll();
		}
	}
	@Override
	public synchronized RpcMessage[] getPendingRpcMessages() {
		RpcMessage[] messages = queue.toArray(new RpcMessage[0]);		
		queue.clear();		
		setEmpty(true);
		// TODO: merge mergeable messages
		return messages;
	}
	public synchronized BlastMessage[] getPendingMessages() {
		BlastMessage[] messages = queue.toArray(new BlastMessage[0]);		
		queue.clear();		
		setEmpty(true);
		// TODO: merge mergeable messages
		return messages;
	}
	public void waitForMessages(int timeout) throws InterruptedException {
		if(getEmpty()){
			synchronized (lock) {
				lock.wait(timeout);	
			}			
		}
	}
	private synchronized void setEmpty(boolean empty){
		this.empty = empty;
	}
	private synchronized boolean getEmpty(){
		return this.empty;
	}

}
