package com.deskblast.server;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.deskblast.server.json.NumberWrappedBlastMessage;

public class ResendCacheImpl implements ResendCache {

	private Map<Integer, NumberWrappedBlastMessage> cache = 
		new HashMap<Integer, NumberWrappedBlastMessage>();
	
	public void backupMessages(NumberWrappedBlastMessage[] messages) {
		for(NumberWrappedBlastMessage numberWrappedBlastMessage : messages){
			cache.put(numberWrappedBlastMessage.getNumber(), numberWrappedBlastMessage);
		}
	}

	public NumberWrappedBlastMessage[] getResendMessages(int requestedMessage) {
		if(cache.containsKey(requestedMessage)){
			List<NumberWrappedBlastMessage> out = 
				new ArrayList<NumberWrappedBlastMessage>();
			while(cache.containsKey(requestedMessage)){
				out.add(cache.get(requestedMessage));
				requestedMessage++;
			}
			return out.toArray(new NumberWrappedBlastMessage[out.size()]);
		}else{
			cache.clear();
			return null;
		}
	}

}
