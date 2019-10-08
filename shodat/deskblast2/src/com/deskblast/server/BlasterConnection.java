package com.deskblast.server;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.deskblast.client.MergeableMessage;
import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.client.RpcMessage;
import com.deskblast.server.json.BlastMessage;
import com.deskblast.server.json.ExpirableMessage;
import com.deskblast.server.json.ExpiringMessage;
import com.deskblast.server.json.NumberWrappedBlastMessage;

public class BlasterConnection {
	
    static final int WAIT_FOR_MESAGE_TIME = 8 * 1000;
    static final int CONSIDER_DISCONNECTED_TIME = 10 * 1000;
    
	private int messageNumber = 0;
	private RpcBlasterCredentials rpcBlasterCredentials;
	private RawMessageQueue rawMessageQueue = new RawMessageQueueImpl();
	private ResendCache resendCache = new ResendCacheImpl();
	private List<Thread> threads = new ArrayList<Thread>();
	private boolean shouldContinueScraping = true;
	private boolean keyframeRequested = false;
	private long lastRequest = System.currentTimeMillis();
	
//	MergingBlastMessageQueue queue = new MergingBlastMessageQueue();

	public BlasterConnection(RpcBlasterCredentials blasterInfo) {
		this.rpcBlasterCredentials = blasterInfo;
	}
	private BlasterConnection(){}
	
	public int getId() {
		return rpcBlasterCredentials.getRpcBlasterInfo().getId();
	}
//
//	public void queueMessage(BlastMessage blastMessage)
//			throws QueueFullException {
//		this.rawMessageQueue.addMessage(blastMessage);
//		//queue.put(blastMessage);
//	}
	public void queueRpcMessage(RpcMessage blastMessage)
		throws QueueFullException {
		this.rawMessageQueue.addRpcMessage(blastMessage);
//		System.out.println("queued message to " + blastMessage 
//				+ " " + rpcBlasterCredentials.getRpcBlasterInfo().getName());
		//queue.put(blastMessage);
	}
	public RpcMessage[] dequeueRpcMessages(String key) {
		if (!key.equals(rpcBlasterCredentials.getKey())) {
			throw new RuntimeException("invalid key!");
		}
		RpcMessage[] response = new RpcMessage[0];
		try{
//			System.out.println("about to clear threads and register...");
			clearThreadsAndRegister();
//			System.out.println("...returned from clear threads and register");
//			System.out.println("about to waitformessages...");
			this.rawMessageQueue.waitForMessages(WAIT_FOR_MESAGE_TIME);
//			System.out.println("...returned from waitformessages");
			RpcMessage[] rawMessages = this.rawMessageQueue.getPendingRpcMessages();
			if(rawMessages.length == 1){
	//			NumberWrappedBlastMessage[] numberWrappedBlastMessages = 
	//								pruneAndNumberMessages(rawMessages);
	//			resendCache.backupMessages(numberWrappedBlastMessages);
	//			if(numberWrappedBlastMessages.length > 1){
	//				System.out.println("+++batching messages+++");
	//			}
				response = rawMessages;
			}else if(rawMessages.length > 1){
				//System.out.println("multiple messages: " + rawMessages.length);
				response = rawMessages;//mergeMessages(rawMessages);
			}
		} catch (InterruptedException ie) {
			System.out.println("take interrupted");
			ie.printStackTrace();
		} finally {
			deregisterThread();
		}		
		updateLastRequest();		
		return response;
	}
	public NumberWrappedBlastMessage[] dequeueMessages(String key,
			int messageNumber) {
		if (!key.equals(rpcBlasterCredentials.getKey())) {
			throw new RuntimeException("invalid key!");
		}
		NumberWrappedBlastMessage[] response = new NumberWrappedBlastMessage[0]; 
		try {
			clearThreadsAndRegister();
			NumberWrappedBlastMessage[] resends = this.resendCache.getResendMessages(messageNumber);
			if(resends != null && resends.length > 0){
				System.out.println("re-sending " + resends.length + " messages");
				response = resends;
			}else{
    			this.rawMessageQueue.waitForMessages(WAIT_FOR_MESAGE_TIME);
    			BlastMessage[] rawMessages = this.rawMessageQueue.getPendingMessages();
    			if(rawMessages.length > 0){
    				NumberWrappedBlastMessage[] numberWrappedBlastMessages = 
    									pruneAndNumberMessages(rawMessages);
    				resendCache.backupMessages(numberWrappedBlastMessages);
    				if(numberWrappedBlastMessages.length > 1){
    					System.out.println("+++batching messages+++");
    				}
    				response = numberWrappedBlastMessages;
    			}
			}
		} catch (InterruptedException ie) {
			System.out.println("take interrupted");
			ie.printStackTrace();
		} finally {
			deregisterThread();
		}		
		updateLastRequest();
		return response;
	}
	public boolean isConnected(){
	    return (System.currentTimeMillis() - getLastRequestTime()) < (CONSIDER_DISCONNECTED_TIME); 
	}
	private synchronized void updateLastRequest(){
	    this.lastRequest = System.currentTimeMillis();
	}
	private synchronized long getLastRequestTime(){
	    return this.lastRequest;
	}
	private NumberWrappedBlastMessage[] pruneAndNumberMessages(
			BlastMessage[] rawMessages) {
		//BlastMessage[] prunedMessages = pruneStaleMessages(rawMessages);
		BlastMessage[] prunedMessages = rawMessages;
		NumberWrappedBlastMessage numberWrappedBlastMessages[] = 
			new NumberWrappedBlastMessage[prunedMessages.length];
		for(int i = 0; i < prunedMessages.length; i++){
			numberWrappedBlastMessages[i] = new NumberWrappedBlastMessage(
					prunedMessages[i], messageNumber++);
		}
		return numberWrappedBlastMessages;
	}
	private RpcMessage[] mergeMessages(RpcMessage[] rawMessages) {		
		if(rawMessages.length == 1){
			return rawMessages;
		}		
		System.out.println("looking for mergable messages...");
		List<MergeableMessage> mergableMessages = new ArrayList<MergeableMessage>();
		List<RpcMessage> outputMessages = new ArrayList<RpcMessage>();
		for(int i = 0; i < rawMessages.length; i++){
			if(rawMessages[i] instanceof MergeableMessage){
				System.out.println("got mergeable message...");
				mergableMessages.add((MergeableMessage)rawMessages[i]);
			}else{
				outputMessages.add(rawMessages[i]);
			}
		}
		if(mergableMessages.size() < 2){
			System.out.println("less than 2 mergable messages; not merging");
			return rawMessages;
		}
		MergeableMessage mergedMessage = null;
		for(MergeableMessage mergeable : mergableMessages){
			if(mergedMessage == null){
				mergedMessage = mergeable; 
			}else{
				mergedMessage = mergeable.merge(mergedMessage);
				System.out.println("merged messages");
			}
		}
		outputMessages.add(0, (RpcMessage)mergedMessage);
		return outputMessages.toArray(new RpcMessage[0]);
	}

	/*
	private BlastMessage[] pruneStaleMessages(BlastMessage[] rawMessages) {		
		if(rawMessages.length == 1){
			return rawMessages;
		}		
		System.out.println("looking for mergable messages...");
		List<MergeableMessage> mergableMessages = new ArrayList<MergeableMessage>();
		List<BlastMessage> outputMessages = new ArrayList<BlastMessage>();
		for(int i = 0; i < rawMessages.length; i++){
			if(rawMessages[i] instanceof MergeableMessage){
				System.out.println("got mergeable message...");
				mergableMessages.add((MergeableMessage)rawMessages[i]);
			}else{
				outputMessages.add(rawMessages[i]);
			}
		}
		if(mergableMessages.size() < 2){
			System.out.println("less than 2 mergable messages; not merging");
			return rawMessages;
		}
		MergeableMessage mergedMessage = null;
		for(MergeableMessage mergeable : mergableMessages){
			if(mergedMessage == null){
				mergedMessage = mergeable; 
			}else{
				mergedMessage = mergeable.merge(mergedMessage);
				System.out.println("merged messages");
			}
		}
		outputMessages.add(0, (BlastMessage)mergedMessage);
		return outputMessages.toArray(new BlastMessage[0]);
		
		MergeableMessage mergeableMessage = mergableMessages.get(0);
		mergeableMessage.merge(mergableMessages.toArray(new MergableMessage[0]));
		BlastMessage[] mergedMessages = .merge(messages)
		BlastMessage[] allMessages = new BlastMessage[mergableMessages.]

		List<String> expirors = new ArrayList<String>();
		List<BlastMessage> survivors = new ArrayList<BlastMessage>();
		for(int i = rawMessages.length-1; i >= 0; i--){
			if(rawMessages[i] instanceof ExpirableMessage){
				String expirableType = ((ExpirableMessage)rawMessages[i]).getType();
				if(expirors.contains(expirableType)){
					System.out.println("dropping message: " + rawMessages[i]);
					continue;
				}else if(rawMessages[i] instanceof ExpiringMessage){
					expirors.add(expirableType);
				}
			}
			survivors.add(rawMessages[i]);
		}
		Collections.reverse(survivors);
		return survivors.toArray(new BlastMessage[0]);
		
	}
*/
	private synchronized void deregisterThread() {
		this.threads.remove(Thread.currentThread());
	}

	private synchronized void clearThreadsAndRegister() {
		if (this.threads.size() > 0) {
			for (Thread thread : threads) {
				thread.interrupt();
			}
		}
		this.threads.add(Thread.currentThread());
	}

	public boolean keyMatches(String key) {
	    //System.out.println("comparing " + key 
	    //        + " to " + blasterCredentials.getKey());
		return rpcBlasterCredentials.getKey().equals(key);
	}

	public RpcBlasterInfo getBlasterInfo() {
		return this.rpcBlasterCredentials.getRpcBlasterInfo();
	}	
	public RpcBlasterCredentials getBlasterCredentials(){
		return this.rpcBlasterCredentials;
	}
	public synchronized void setShouldContinueScraping(boolean continueScraping) {
		this.shouldContinueScraping = continueScraping;
	}
	public synchronized boolean getShouldContinueScraping() {
		return this.shouldContinueScraping;
	}
	public synchronized boolean getAndResetKeyframeRequested() {
		if(keyframeRequested){
			keyframeRequested = false;
			return true;
		}
		return false;
	}
	public synchronized void setKeyframeRequested() {
		this.keyframeRequested = true;
	}
}
