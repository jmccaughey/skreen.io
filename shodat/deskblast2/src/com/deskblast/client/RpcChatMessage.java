package com.deskblast.client;

public class RpcChatMessage extends RpcMessage {

	private String text;

	public String getText() {
		return text;
	}
//
//	public void setText(String text) {
//		this.text = text;
//	}

	public RpcChatMessage(){}
	
	public RpcChatMessage(String text) {		
		this.text = text;
	}
	
}
