package com.deskblast.client;

public interface MergeableMessage {

	public MergeableMessage merge(MergeableMessage oldMessage);	
	
}