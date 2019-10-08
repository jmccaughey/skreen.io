package com.deskblast.client;

import java.io.Serializable;

public class RpcBlasterCredentials implements Serializable{

	private String key;
	private int blastId;
	private RpcBlasterInfo rpcBlasterInfo;
	private boolean owner;
	
	public RpcBlasterCredentials(RpcBlasterInfo rpcBlasterInfo,
			int blastId, String key, boolean owner) {
		this.key = key;
		this.blastId = blastId;
		this.rpcBlasterInfo = rpcBlasterInfo;
		this.owner = owner;
	}
	public RpcBlasterCredentials(){}
	public boolean isOwner() {
		return owner;
	}
	public String getKey() {
		return key;
	}
	public int getBlastId() {
		return blastId;
	}
	public RpcBlasterInfo getRpcBlasterInfo() {
		return rpcBlasterInfo;
	}
	
}
