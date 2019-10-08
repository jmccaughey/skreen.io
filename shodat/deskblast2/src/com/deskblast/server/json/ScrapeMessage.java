package com.deskblast.server.json;

import java.util.ArrayList;
import java.util.List;

public class ScrapeMessage extends BlastMessage 
//implements
	//	MergeableMessage 
{

	private List<ScrapeInfo> scrapeInfos;
	
	public ScrapeMessage(List<ScrapeInfo> scrapeInfos){
		this.scrapeInfos = scrapeInfos;
	}
	
//	public MergeableMessage merge(MergeableMessage oldMessage) {
//		if(oldMessage instanceof ScrapeMessage){
//			return combineScrapes((ScrapeMessage)oldMessage);
//			
//			
//			ScrapeMessage oldScrapeMessage = (ScrapeMessage)oldMessage;
//			ScrapeInfo[] oldScrapeInfos = oldScrapeMessage.getScrapeInfos();
//			ScrapeInfo[] newScrapeInfos = this.getScrapeInfos();
//			List<ScrapeInfo> mergedScrapeInfos = new ArrayList<ScrapeInfo>();
//			for(ScrapeInfo siOld : oldScrapeInfos){
//				boolean found = false;
//				for(ScrapeInfo siNew : newScrapeInfos){
//					if(siOld.getX() == siNew.getX() && siOld.getY() == siNew.getY()){
//						found = true;
//						break;
//					}
//				}
//				if( ! found){
//					System.out.println("carrying over " + siOld);
//					mergedScrapeInfos.add(siOld);
//				}else{
//					System.out.println("dropping " + siOld);
//				}
//			}
//			for(ScrapeInfo siNew : newScrapeInfos){
//			    mergedScrapeInfos.add(siNew);
//			}
//		}else{
//			throw new RuntimeException("unexpected mergeable message type: " + oldMessage);
//		}
//	}
	public static ScrapeMessage combineScrapes(
			ScrapeMessage oldScrapeMessage, ScrapeMessage newScrapeMessage){
		ScrapeInfo[] oldScrapeInfos = oldScrapeMessage.getScrapeInfos();
		ScrapeInfo[] newScrapeInfos = newScrapeMessage.getScrapeInfos();
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
		newScrapeMessage.scrapeInfos = mergedScrapeInfos;
		return newScrapeMessage;
//		return new ScrapeMessage(mergedScrapeInfos);
//		sm.
//		return new ScrapeMessage()
//		target.scrapeInfos = mergedScrapeInfos;
//		return target;
	}
	public ScrapeInfo[] getScrapeInfos() {
		return scrapeInfos.toArray(new ScrapeInfo[0]);
	}	
	public String getType() {
		return "continuedScrape";
	}

}
