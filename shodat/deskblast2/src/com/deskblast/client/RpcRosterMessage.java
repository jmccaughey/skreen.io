package com.deskblast.client;

public class RpcRosterMessage extends RpcMessage {

	private RpcBlasterInfo[] rpcBlasterInfos;

	public RpcRosterMessage(){}
	
	public RpcRosterMessage(RpcBlasterInfo[] roster) {
		this.rpcBlasterInfos = roster;
	}
	
	
	public RpcBlasterInfo[] getRpcBlasterInfos() {
		return rpcBlasterInfos;
	}

	public void setRpcBlasterInfos(RpcBlasterInfo[] rpcBlasterInfos) {
		this.rpcBlasterInfos = rpcBlasterInfos;
	}
	
}
