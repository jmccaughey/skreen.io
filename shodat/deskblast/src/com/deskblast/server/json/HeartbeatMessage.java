package com.deskblast.server.json;

public class HeartbeatMessage extends BlastMessage{

	public HeartbeatMessage(){}

	public String getType() {
		return "heartbeat";
	}

}
