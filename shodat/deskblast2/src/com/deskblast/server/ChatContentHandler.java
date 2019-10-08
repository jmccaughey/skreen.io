package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.server.json.BlastMessage;

public class ChatContentHandler implements ContentHandler {

	public String getName() {
		return "chat";	
	}
//
//	public void handleContent(BlastMessage blastMessage,
//			BlastContext messageSender) {
//		RpcChatMessage rpcChatMessage = new RpcChatMessage();
//		rpcChatMessage.setText(((ChatMessage)blastMessage).getText());
//		RpcBlasterInfo rpcBlasterInfo = new RpcBlasterInfo();
//		rpcBlasterInfo.setId(blastMessage.getSender().getId());
//		rpcBlasterInfo.setName(blastMessage.getSender().getName());
//		rpcChatMessage.setSender(rpcBlasterInfo);
//		messageSender.sendRpcToAll(rpcChatMessage);
//	}

	@Override
	public void handleContent(RpcMessage rpcMessage, BlastContext messageSender) {
		messageSender.sendRpcToAll(rpcMessage);		
	}

	@Override
	public void handleJsonContent(BlastMessage message,
			BlastContext messageSender) {
		// TODO Auto-generated method stub
		
	}

}
