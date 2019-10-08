package com.deskblast.server.json;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ScrapeMetaMessage extends ScrapeMessage 
//implements MergeableMessage
{

	private List<ScrapeInfo> scrapeInfos = new ArrayList<ScrapeInfo>();
	private boolean keyframe;
	private int mouseX;
	private int mouseY;
	private int deskWidth;
	private int deskHeight;
	
	public ScrapeMetaMessage(int deskWidth, int deskHeight, 
	        ScrapeInfo[] scrapeInfos, boolean keyframe,
            int mouseX, int mouseY) {
        super(Arrays.asList(scrapeInfos)); 
        for(ScrapeInfo si : scrapeInfos){
        	this.scrapeInfos.add(si);
        }
        this.keyframe = keyframe;
        this.mouseX = mouseX;
        this.mouseY = mouseY;
        this.deskWidth = deskWidth;
        this.deskHeight = deskHeight;
    }
//    public ScrapeMessage(ScrapeMessage scrapeMessage) {
//		this(scrapeMessage.deskWidth, 
//				scrapeMessage.deskHeight,
//				scrapeMessage.getScrapeInfos(),
//				scrapeMessage.keyframe,
//				scrapeMessage.mouseX,
//				scrapeMessage.mouseY
//		);
//	}

	public boolean isKeyframe() {
		// TODO: discover this from # of tiles instead
		return keyframe;
	}
	public int getMouseX() {
		return mouseX;
	}
	public int getMouseY() {
		return mouseY;
	}
	public int getDeskWidth() {
        return deskWidth;
    }
    public int getDeskHeight() {
        return deskHeight;
    }
    public String getType() {
		return "scrape";
	}
    
    public String toString(){        
        String result = super.toString(); 
        for(ScrapeInfo scrapeInfo: this.scrapeInfos){
            result += "{" + scrapeInfo.toString() + "}";
        }
        return result;
    }
//	public MergeableMessage merge(MergeableMessage olderMessage) {
//		// if this is a key frame, no need to copy any info 
//		// from the older one
//		if(olderMessage instanceof ScrapeMetaMessage){
//			
//		}
//			
//	
//		
//		if(this.isKeyframe() && olsMe){
//			return this;
//		}else if(olderMessage instanceof ScrapeMetaMessage){
//			return  
//		}else if(olderMessage instanceof ScrapeMessage){
//			return this;
//		}else{
//			throw new RuntimeException("unexpected mergeablemessage type: " + olderMessage);
//		}
		
		
		/*
		
		ScrapeMetaMessage oldScrapeMessage = (ScrapeMetaMessage)oldMessage;
		ScrapeInfo[] oldScrapeInfos = oldScrapeMessage.getScrapeInfos();
		ScrapeInfo[] newScrapeInfos = this.getScrapeInfos();
		List<ScrapeInfo> mergedScrapeInfos = new ArrayList<ScrapeInfo>();
		for(ScrapeInfo siOld : oldScrapeInfos){
			boolean found = false;
			for(ScrapeInfo siNew : newScrapeInfos){
				if(siOld.getX() == siNew.getX() && siOld.getY() == siNew.getY()){
					found = true;
					break;
				}
			}
			if( ! found){
				System.out.println("carrying over " + siOld);
				mergedScrapeInfos.add(siOld);
			}else{
				System.out.println("dropping " + siOld);
			}
		}
		for(ScrapeInfo siNew : newScrapeInfos){
		    mergedScrapeInfos.add(siNew);
		}
		return new ScrapeMetaMessage(this.deskWidth, this.deskHeight, 
				mergedScrapeInfos.toArray(new ScrapeInfo[0]),
				oldScrapeMessage.isKeyframe(), this.mouseX, this.mouseY);
				*/
//	}
	
}
