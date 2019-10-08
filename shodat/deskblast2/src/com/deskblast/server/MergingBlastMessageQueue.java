package com.deskblast.server;

import java.util.ArrayList;
import java.util.List;

import com.deskblast.server.json.BlastMessage;

public class MergingBlastMessageQueue {
	
	private List<BlastMessage> messages = new ArrayList<BlastMessage>();
	
	public synchronized void put(BlastMessage blastMessage){
		// most recent message is last
		messages.add(blastMessage);		
	}
	
	public synchronized BlastMessage[] get(){
		// do merging on get thread (no need to merge 
		// before client asks for data)
		List<BlastMessage> consolidatedMessages = new ArrayList<BlastMessage>(messages.size());
		// TODO: merge mergeable messages
		for(BlastMessage blastMessage : messages){
			consolidatedMessages.add(blastMessage);
		}
		messages.clear();
		return consolidatedMessages.toArray(new BlastMessage[0]);
	}
	
}
