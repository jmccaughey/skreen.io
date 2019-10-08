package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

class JsArray<E extends JavaScriptObject> extends JavaScriptObject {
	protected JsArray() {
	}

	public final native int length() /*-{ return this.length; }-*/;

	public final native E get(int i) /*-{ return this[i];     }-*/;
}