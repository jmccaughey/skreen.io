package com.deskblast.client;

import java.io.Serializable;

public class RpcMessage implements Serializable {

	private RpcBlasterInfo sender;

	public RpcMessage(){}
	
	public void setSender(RpcBlasterInfo blasterInfo){
		this.sender = blasterInfo;
	}

	public RpcBlasterInfo getSender(){
		return this.sender;
	}
	
}
