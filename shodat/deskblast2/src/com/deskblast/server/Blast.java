package com.deskblast.server;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.ehcache.Cache;
import net.sf.ehcache.CacheManager;
import net.sf.ehcache.Element;
import net.sf.ehcache.store.MemoryStoreEvictionPolicy;

import com.deskblast.client.RpcAbstractTileInfo;
import com.deskblast.client.RpcAbstractTileInfoSprite;
import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.client.RpcChatMessage;
import com.deskblast.client.RpcCompressedScreenScrape;
import com.deskblast.client.RpcCompressedScreenScrapeSprite;
import com.deskblast.client.RpcMessage;
import com.deskblast.client.RpcRosterMessage;
import com.deskblast.client.RpcScrapeMessage;
import com.deskblast.client.RpcScrapingScrapeMessage;
import com.deskblast.client.RpcScreenScrapeData;
import com.deskblast.client.RpcStartScrapingMessage;
import com.deskblast.client.RpcStopScrapingMessage;
import com.deskblast.server.json.NumberWrappedBlastMessage;

public class Blast implements BlastContext {

    private Map<String, ContentHandler> contentHandlers = Collections
            .synchronizedMap(new HashMap<String, ContentHandler>());

    private int blastId;
    private RpcBlasterCredentials ownerCredentials;
    private Map<Integer, BlasterConnection> blasters = Collections
            .synchronizedMap(new HashMap<Integer, BlasterConnection>());
    private Cache imageCache;
    private RpcBlasterCredentials blastingUser;
    private RpcBlasterInfo[] roster;

//    private List<PartialScrapeData> pendingScrapes = Collections
//    	.synchronizedList(new ArrayList<PartialScrapeData>());
    
    public Blast(RpcBlasterCredentials owner) {
        this.ownerCredentials = owner;
        blastId = owner.getBlastId();
        addBlaster(owner);
        ContentHandler chat = new ChatContentHandler();
        contentHandlers.put(chat.getName(), chat);
        ContentHandler scrape = new ScrapeContentHandler();
        contentHandlers.put(scrape.getName(), scrape);
        imageCache = new Cache(blastId+"", 100, MemoryStoreEvictionPolicy.FIFO,
                true, "", true, 0, 0, false, 0, null);
        CacheManager.getInstance().addCache(imageCache);
    }

    public int getId() {
        return this.blastId;
    }

    public void handleRpcMessage(RpcMessage rpcMessage, String key) {
        System.out.println("got message: " + rpcMessage);
        BlasterConnection blasterConnection = blasters.get(rpcMessage
                .getSender().getId());
        ContentHandler handler = null;
        if (blasterConnection.keyMatches(key)) {
        	if(rpcMessage instanceof RpcChatMessage){
        		handler = findHandler("chat");
        	}else if(rpcMessage instanceof RpcScrapeMessage){
        		handler = findHandler("scrape");
        	}
            handler.handleContent(rpcMessage, this);
        } else {
            System.out.println("incorrect key!");
        }
    }
    public ContinueInfo handleScreenScrape(DataInputStream dataInputStream,//ScreenScrapeData screenScrapeData,
            int senderId, String key) {
        final BlasterConnection blasterConnection = blasters.get(senderId);
        // TODO: assert that this blaster can/is sharing (is moderator)
        if (blasterConnection.keyMatches(key)) {
        	// stream scrapes: send each tile as a BlastMessage
        	// meta message has region dimensions
        	RpcScrapeStreamHandler handler = new RpcScrapeStreamHandler() {
				@Override
				public void handleScrape(RpcScreenScrapeData rpcScreenScrapeData) {
		            RpcMessage rpcMessage = new RpcScrapingScrapeMessage(
		                    rpcScreenScrapeData);
		            RpcBlasterInfo rpcBlasterInfo = new RpcBlasterInfo();
		            rpcBlasterInfo.setId(blasterConnection.getBlasterInfo().getId());
		            rpcBlasterInfo.setName(blasterConnection.getBlasterInfo().getName());
		            rpcMessage.setSender(rpcBlasterInfo);
		            ContentHandler contentHandler = findHandler("scrape");
		            contentHandler.handleContent(rpcMessage, Blast.this);
		            //System.out.println("queueing message");
				}
			};			
			try {
				ScrapeStreamReader.readStream(dataInputStream, handler);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
            return new ContinueInfo(blasterConnection.getShouldContinueScraping(),
            		blasterConnection.getAndResetKeyframeRequested());
        } else {
            System.out.println("incorrect key!");
            return new ContinueInfo(false, false);
        }
    }
    public RpcBlasterInfo getOwnerInfo() {
        return this.ownerCredentials.getRpcBlasterInfo();
    }
    
    public RpcBlasterCredentials getOwnerCredentials(){
    	return this.ownerCredentials;
    }

    private ContentHandler findHandler(String className) {
        if (className.toLowerCase().indexOf("chat") > -1) {
            return contentHandlers.get("chat");
        } else {
            return contentHandlers.get("scrape");
        }
    }

    public NumberWrappedBlastMessage[] getMessages(int blasterId, String key,
            int messageNumber) throws InterruptedException {
        if(rosterChanged()){
            sendRosterMessage();
        }
        return blasters.get(blasterId).dequeueMessages(key, messageNumber);
    }
	public RpcMessage[] getRpcMessages(int blasterId, String key) {
		return blasters.get(blasterId).dequeueRpcMessages(key);
	}
    void addBlaster(RpcBlasterCredentials rpcBlasterInfo) {
    	System.out.println("adding blaster");
        blasters.put(rpcBlasterInfo.getRpcBlasterInfo().getId(),
                new BlasterConnection(rpcBlasterInfo));
        sendRosterMessage();
        this.setKeyframeRequested();
    }
//    
//    public void sendToAll(BlastMessage blastMessage) {
//        for (BlasterConnection blaster : blasters.values()) {
//            try {
//                blaster.queueMessage(blastMessage);
//            } catch (QueueFullException e) {
//                System.out.println("queue full: "
//                        + blaster.getBlasterInfo().getName());
//                handleConnectionProblem(blaster);
//            }
//        }
//    }
    private void handleConnectionProblem(BlasterConnection blaster) {
        // show blaster as disconnected in roster
        // TODO
    }
//    public void sendToAllButSelf(BlastMessage blastMessage) {
//        for (BlasterConnection blaster : blasters.values()) {
//            if (!blaster.getId().equals(blastMessage.getSender().getId())) {
//                try {
//                    blaster.queueMessage(blastMessage);
//                } catch (QueueFullException e) {
//                    System.out.println(e.getMessage());
//                    handleConnectionProblem(blaster);
//                }
//            }
//        }
//    }
    private void sendRosterMessage() {
        this.roster = buildRoster();
        RpcRosterMessage rosterMessage = new RpcRosterMessage(this.roster);
        sendRpcToAll(rosterMessage);
        System.out.println("sent roster: " + this.roster.length);
    }
    private RpcBlasterInfo[] buildRoster(){
        Collection<BlasterConnection> blasters = this.blasters.values();
        List<RpcBlasterInfo> roster = new ArrayList<RpcBlasterInfo>();
        //System.out.println("raw roster size: " + blasters.size());
        for (BlasterConnection bc : blasters) {
            if(bc.isConnected()){
                roster.add(bc.getBlasterInfo());
            }
        }
        return roster.toArray(new RpcBlasterInfo[0]);
    }
    private boolean rosterChanged(){
        //System.out.println("in roster changed");
        RpcBlasterInfo[] currentRoster = buildRoster();
        if(currentRoster.length != this.roster.length){
        	System.out.println("roster changed");
            return true;
        }
        for(int i = 0; i < currentRoster.length; i++){
            if( ! currentRoster[i].equals(this.roster[i])){
            	System.out.println("roster changed");
                return true;
            }
        }
        return false;
    }
    /*
    public void addScrapesToCache(ScreenScrapeData screenScrapeData) {
        AbstractTileInfo[] tiles = screenScrapeData.getCompressedRects();
        for (int i = 0; i < tiles.length; i++) {
            if (tiles[i] instanceof CompressedScreenScrape) {
                this.imageCache.put(new Element(
                        tiles[i].getHash(),
                        ((CompressedScreenScrape) tiles[i]).getBytes())
                );
            }
        }
    }*/
    public byte[] getScrapeFromCache(String name, int blasterId, String key) {
        BlasterConnection blasterConnection = this.blasters.get(blasterId);
        if(blasterConnection.keyMatches(key)){
            byte[] scrape = (byte[])this.imageCache.get(name).getObjectValue();
            return scrape;
        }else{
            throw new RuntimeException("key did not match: " + key);
        }
    }
    public void setShouldContinueScraping(boolean continueScraping,
            int blasterId) {
    	System.out.println("setting shouldContinueScraping: " + continueScraping);
        BlasterConnection blasterConnection = this.blasters.get(blasterId);
        blasterConnection.setShouldContinueScraping(continueScraping);
        if(continueScraping){
        	this.blastingUser = blasterConnection.getBlasterCredentials();
        }else{
        	this.blastingUser = null;
        }
    }
    public void setKeyframeRequested(){    	
    	System.out.println("setting keyframe requested");
    	if(this.blastingUser != null){
    		System.out.println("setting keyframe requested on blaster");
    		this.blasters.get(this.blastingUser.getRpcBlasterInfo().getId())
    			.setKeyframeRequested();
    	}
    }
	public boolean getShouldContinueScraping(int blasterId) {
		BlasterConnection blasterConnection = this.blasters.get(blasterId);
		if(blasterConnection != null){
			return blasterConnection.getShouldContinueScraping();
		}
		return false;
	}
	public void handleScreenScrapingStopped(int blasterId, String key) {
		if(blasters.get(blasterId).keyMatches(key) && this.blastingUser != null 
				&& this.blastingUser.getRpcBlasterInfo().getId() == blasterId){
			RpcStopScrapingMessage ssm = new RpcStopScrapingMessage();
			ssm.setSender(blasters.get(blasterId).getBlasterInfo());
			this.sendRpcToAll(ssm);
		}else{
			throw new RuntimeException("key did not match: " + key);
		}
	}
/*	
	public ContinueInfo handlePartialScreenScrape(
			PartialScrapeData partialScrape, Long blasterId, String key) {
		this.pendingScrapes.add(partialScrape);
		if(partialScrape.isLastScrape()){
			ScreenScrapeData screenScrapeData = new ScreenScrapeData(
					this.pendingScrapes.toArray(new PartialScrapeData[0]));
			return handleScreenScrape(screenScrapeData, blasterId, key);
		}
		return new ContinueInfo(true, false);
	}
*/	
	public void handlePluginInstalledStartScraping(int blasterId, String key){
		if(blasters.get(blasterId).keyMatches(key) 
				&& this.blastingUser == null 
				//&& this.blastingUser.getBlasterInfo().getId().equals(blasterId)
				){
			RpcStartScrapingMessage ssm = new RpcStartScrapingMessage();
			ssm.setSender(blasters.get(blasterId).getBlasterInfo());
			handleRpcMessage(ssm, key);
			//this.sendToAll(ssm);
		}else{
			throw new RuntimeException("key did not match: " + key);
		}
	}

	@Override
	public void sendRpcToAll(RpcMessage rpcMessage) {
        for (BlasterConnection blaster : blasters.values()) {
            try {
                blaster.queueRpcMessage(rpcMessage);
            } catch (QueueFullException e) {
                System.out.println("queue full: "
                        + blaster.getBlasterInfo().getName());
                handleConnectionProblem(blaster);
            }
        }
	}

	@Override
	public void addScrapesToCache(RpcScreenScrapeData rpcScreenScrapeData) {
        RpcAbstractTileInfoSprite[] tiles = rpcScreenScrapeData.getRects();
        for (int i = 0; i < tiles.length; i++) {
            if (tiles[i] instanceof RpcCompressedScreenScrapeSprite) {
                this.imageCache.put(new Element(
                        tiles[i].getHash(),
                        ((RpcCompressedScreenScrapeSprite)tiles[i]).bytes)
                );
            }
        }
	}

	@Override
	public void sendRpcToAllButSelf(RpcMessage rpcMessage) {
		// assumption: array of messages will have consistent
		// sender
		int senderId = rpcMessage.getSender().getId();
        for (BlasterConnection blaster : blasters.values()) {
            if (blaster.getId() != senderId){
                try {
                    blaster.queueRpcMessage(rpcMessage);
                } catch (QueueFullException e) {
                    System.out.println("queue full exception: " + e.getMessage());
                    handleConnectionProblem(blaster);
                }
            }
        }
	}

}
