package com.deskblast.server;

import com.deskblast.client.RpcBlasterInfo;

public class QueueFullException extends Exception {

	private RpcBlasterInfo blasterInfo;
	
	public QueueFullException(){}
	
	public QueueFullException(BlasterConnection blasterConnection) {
		this.blasterInfo = blasterConnection.getBlasterInfo();
	}
	@Override
	public String toString(){
		return "Blaster " + blasterInfo.getName() + " id: " + blasterInfo.getId()
		+ "has too many message in queue.";
	}

}
