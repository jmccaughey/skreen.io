package com.deskblast.server.json;

public interface ExpirableMessage {

	
	/**
	 * An expiring message expires expirable messages of teh same type
	 */
	public String getType();
	
}
