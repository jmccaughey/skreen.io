package com.deskblast.server.json;

public class NumberWrappedBlastMessage {

	private BlastMessage message;
	private int number;
	
	public NumberWrappedBlastMessage(BlastMessage message, int number) {
		super();
		this.message = message;
		this.number = number;
	}
	public int getNumber() {
		return number;
	}
	public BlastMessage getMessage(){
		return this.message;
	}
}
