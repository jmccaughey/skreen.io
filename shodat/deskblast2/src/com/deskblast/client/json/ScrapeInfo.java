package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class ScrapeInfo extends JavaScriptObject {
	
	protected ScrapeInfo() {}

	public final native String getName() /*-{ return this.name; }-*/;
	
	public final native int getX() /*-{ return this.x; }-*/;
	
	public final native int getY() /*-{ return this.y; }-*/;
	
//	public final native int getWidth() /*-{ return this.width; }-*/;
	
//	public final native int getHeight() /*-{ return this.height; }-*/;
	
}
