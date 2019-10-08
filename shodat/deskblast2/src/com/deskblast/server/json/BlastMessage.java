package com.deskblast.server.json;

public abstract class BlastMessage {

	private BlasterInfo sender;

	public void setSender(BlasterInfo blasterInfo){
		this.sender = blasterInfo;
	}

	public BlasterInfo getSender(){
		return this.sender;
	}
	
	public abstract String getType(); 
}
