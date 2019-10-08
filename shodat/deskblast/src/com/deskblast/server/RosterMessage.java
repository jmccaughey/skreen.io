package com.deskblast.server;

import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.server.json.BlastMessage;

public class RosterMessage extends BlastMessage {

	private RpcBlasterInfo[] blasterInfos;

	public RosterMessage(RpcBlasterInfo[] blasterInfos) {
		super();
		this.blasterInfos = blasterInfos;
	}

	public RpcBlasterInfo[] getBlasterInfos() {
		return blasterInfos;
	}

	public String getType() {
		return "roster";
	}
	
}
