package com.deskblast.server.json;

public class ChatMessage extends BlastMessage{

	private String text;
	
	public ChatMessage(){}
	
	public String getText(){
		return this.text;
	}
	public void setText(String text){
		this.text = text;
	}

	public String getType() {
		return "chat";
	}

}
