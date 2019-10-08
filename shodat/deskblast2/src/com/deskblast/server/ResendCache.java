package com.deskblast.server;

import com.deskblast.server.json.NumberWrappedBlastMessage;

public interface ResendCache {

	public NumberWrappedBlastMessage[] getResendMessages(int requestedMessage);
	
	public void backupMessages(NumberWrappedBlastMessage[] messages);
	
}
