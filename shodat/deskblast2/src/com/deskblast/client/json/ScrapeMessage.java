package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.core.client.JsArray;

public class ScrapeMessage extends JavaScriptObject {

	protected ScrapeMessage() {}
	
	public final native boolean getKeyframe() /*-{ return this.keyframe; }-*/;
	
	//public final native ScrapeInfo[] getScrapeInfos() /*-{ return this.scrapeInfos; }-*/;
	
	public final native JsArray<ScrapeInfo> getScrapeInfos() /*-{ return this.scrapeInfos; }-*/;

	public final native int getMouseX() /*-{ return this.mouseX; }-*/;
	
	public final native int getMouseY() /*-{ return this.mouseY; }-*/;
	
	public final native int getDeskWidth() /*-{ return this.deskWidth; }-*/;
	
	public final native int getDeskHeight() /*-{ return this.deskHeight; }-*/;
	
	public static final native ScrapeMessage fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
	
}
