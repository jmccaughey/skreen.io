package com.deskblast.server;

public class ContinueInfo {

	private boolean bContinue;
	private boolean bKeyframeRequest;
	
	public ContinueInfo(boolean continue1, boolean keyframeRequest) {
		super();
		bContinue = continue1;
		bKeyframeRequest = keyframeRequest;
	}

	public boolean isContinue() {
		return bContinue;
	}

	public boolean isKeyframeRequest() {
		return bKeyframeRequest;
	}
	
	
	
}
