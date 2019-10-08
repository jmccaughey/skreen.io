package com.deskblast.server;

import java.io.DataInputStream;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.deskblast.client.RpcAbstractTileInfo;
import com.deskblast.client.RpcAbstractTileInfoSprite;
import com.deskblast.client.RpcBackReferenceScreenScrape;
import com.deskblast.client.RpcBackReferenceScreenScrapeSprite;
import com.deskblast.client.RpcCompressedScreenScrape;
import com.deskblast.client.RpcCompressedScreenScrapeSprite;
import com.deskblast.client.RpcScreenScrapeData;
import com.deskblast.client.RpcScreenScrapeMetaData;

public class ScrapeStreamReader {

	static final Log log = LogFactory.getLog(ScrapeStreamReader.class);
	
	public static final int FILE_NAME_LENGTH = 32;
	
	public static final long INTERMEDIATE_SEND_THRESHOLD = 200L;
	
	public static void readStream(DataInputStream in, RpcScrapeStreamHandler handler) throws IOException{
		long lStart = System.currentTimeMillis();
		
		boolean bKeyFrame = in.readInt() > 0; 
		int width = in.readInt();
		int height = in.readInt();
		int mouseX = in.readInt();
		int mouseY = in.readInt();
		int tileCount = in.readInt();

		List<RpcAbstractTileInfoSprite> pendingQueue = new LinkedList<RpcAbstractTileInfoSprite>();
		boolean bSentMetaData = false;
		
		for(int i = 0; i < tileCount; i++){
			byte[] hash = new byte[FILE_NAME_LENGTH];
			in.readFully(hash);
			String name = new String(hash);
			int length = in.readInt();
			//int available = in.available();
			long elapsed = System.currentTimeMillis() - lStart; 
			//System.out.println("elapsed: " + elapsed);
			if(i == 0 || (elapsed < INTERMEDIATE_SEND_THRESHOLD)){ //(length+8) <= available){
				// the idea is to check if the upload is taking
				// a while.  if it is a slowish upload, start streaming 
				// to viewers now instead of waiting for entire scrape to
				// get up to server
				// will not block so keep reading
			}else if(pendingQueue.size() > 0){
				// will block so send queued items now
				log.info("intermediate message: " + pendingQueue.size() + " tiles at " + System.currentTimeMillis());
				if(!bSentMetaData){
					bSentMetaData = true;
					handler.handleScrape(new RpcScreenScrapeMetaData(
							pendingQueue.toArray(new RpcAbstractTileInfoSprite[0]), 
							width, height, mouseX, mouseY, bKeyFrame));					
				}else{
					handler.handleScrape(new RpcScreenScrapeData(
							pendingQueue.toArray(new RpcAbstractTileInfoSprite[0])));
				}
				pendingQueue.clear();
				lStart = System.currentTimeMillis();
			}
			int tileX = in.readInt();
			int tileY = in.readInt();
			// begin new protocol
			int tileWidth = in.readInt();
			int tileHeight = in.readInt();
			int spriteX = in.readInt();
			int spriteY = in.readInt();
			// end new protocol
			if(length > 0){
				byte[] jpeg = new byte[length]; 
				in.readFully(jpeg);
				pendingQueue.add(new RpcCompressedScreenScrapeSprite(jpeg, tileX, tileY, 
						tileWidth, tileHeight, name, spriteX, spriteY));
			}else{
				pendingQueue.add(new RpcBackReferenceScreenScrapeSprite(tileX, tileY, 
						tileWidth, tileHeight, name, spriteX, spriteY));
			}
		}
		if(pendingQueue.size() > 0 || !bSentMetaData){
			if(!bSentMetaData){
				bSentMetaData = true;
				handler.handleScrape(new RpcScreenScrapeMetaData(
						pendingQueue.toArray(new RpcAbstractTileInfoSprite[0]), 
						width, height, mouseX, mouseY, bKeyFrame));					
			}else{
				handler.handleScrape(new RpcScreenScrapeData(
						pendingQueue.toArray(new RpcAbstractTileInfoSprite[0])));
			}
			pendingQueue.clear();
		}
		log.info("done reading scrape");
	}
	
}
