package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class ParticipantInfo extends JavaScriptObject {

	protected ParticipantInfo() {
	}
	
	public final native String getMeetingKey() /*-{ return this.meetingKey; }-*/;  
	public final native String getConfID() /*-{ return this.confID; }-*/;
	public final native void setConfID(String confID) /*-{ this.confID = confID; }-*/;
	public final native String getMeetingID() /*-{ return this.meetingID; }-*/;
	public final native void setMeetingID(String meetingID) /*-{ this.meetingID = meetingID; }-*/;
	public final native String getParticipantID() /*-{ return this.participantID; }-*/;
	
	public final native String getName() /*-{ return this.name; }-*/;
	public final native void setName(String name) /*-{ this.name = name; }-*/;
	
	public final native String getLabel() /*-{ return this.label; }-*/;
	
	public final native String getServerID() /*-{ return this.serverID; }-*/;
	public final native void setServerID(String serverID) /*-{ this.serverID = serverID; }-*/;

	public final native void setJavaClass(String javaClass) /*-{ this.javaClass = javaClass; }-*/;
	
	public static final native ParticipantInfo fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
}
