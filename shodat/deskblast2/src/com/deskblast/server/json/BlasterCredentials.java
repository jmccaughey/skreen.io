package com.deskblast.server.json;

import com.deskblast.client.RpcBlasterCredentials;

public class BlasterCredentials {

	private int blastId;
	private BlasterInfo blasterInfo;
	private String key;
	private Boolean owner;

	public BlasterCredentials(){}
	
	public BlasterCredentials(BlasterInfo blasterInfo, int blastId, String key, Boolean owner) {
		super();
		this.blasterInfo = blasterInfo;
		this.blastId = blastId;
		this.key = key;
		this.owner = owner;
	}
	public BlasterCredentials(RpcBlasterCredentials bc){
		this(new BlasterInfo(
					bc.getRpcBlasterInfo().getName(), 
					bc.getRpcBlasterInfo().getId()), 
				bc.getBlastId(), 
				bc.getKey(),
				bc.isOwner()); 
	}
	
	public void setBlastId(int blastId) {
		this.blastId = blastId;
	}
	public void setKey(String key) {
		this.key = key;
	}
	public void setOwner(Boolean owner) {
		this.owner = owner;
	}
	public int getBlastId() {
		return blastId;
	}
	public String getKey() {
		return key;
	}
	public Boolean getOwner(){
		return this.owner;
	}
	public BlasterInfo getBlasterInfo(){
		return this.blasterInfo;
	}
	public void setBlasterInfo(BlasterInfo blasterInfo){
		this.blasterInfo = blasterInfo;
	}
}