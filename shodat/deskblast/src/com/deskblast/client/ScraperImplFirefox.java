package com.deskblast.client;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.DOM;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.FlowPanel;
import com.google.gwt.user.client.ui.HTML;

public class ScraperImplFirefox extends ScraperImplDefault{

	private static final String PLUGIN_NAME = "shodat plugin";
	
	public boolean installPlugin(ScraperParms scraperParms){
		this.scraperParms = scraperParms;
		if(!isFF32BitOnWindows() || pluginFound(PLUGIN_NAME)){
			return true;
		}
		InstallDialog dialog = new InstallDialog(
				this.scraperParms);
		dialog.center();
		return false;
	}
	public void startScraping(){
		if(isFF32BitOnWindows()){
			if(null == DOM.getElementById(scraperParms.getScraperId())){
				if(pluginFound(PLUGIN_NAME)){
					insertEmbed();
				}else{
					Window.alert("plugin not found");
				}
			}
		}else{
			super.startScraping();
		}
	}
	public void stopScraping(){		
		if(isFF32BitOnWindows()){
			removeEmbed(scraperParms.getContainerId());
		}else{
			super.stopScraping();
		}
	}
	native private boolean isFF32BitOnWindows() /*-{
	  var userAgent = navigator.userAgent.toLowerCase();
	  //alert(userAgent);
	  if(userAgent.indexOf('gecko') != -1 || userAgent.indexOf('chrome') != -1){
	  	if(userAgent.indexOf('windows') != -1){
	  	  if(userAgent.indexOf('win64') == -1){
	  	  	if(userAgent.indexOf('x64') == -1){
	  	  	  if(userAgent.indexOf('apple') != -1){
	  	  	  	if(userAgent.indexOf('chrome') != -1){	  	  	  	
	  	  		    return true;
	  	  	  	}
	  	  	  	return false;
	  	  	  }
	  	  	  return true;
	  	  	}
	  	  }
	  	}
	  }
	  return false;
	}-*/;
	private final native boolean pluginFound(String name) /*-{
		navigator.plugins.refresh();
		for(var index = 0; index < navigator.plugins.length; index++){
			if(navigator.plugins[index].name.toLowerCase() == name){
				return true;
			}
		}
		return false;
	}-*/;
	private void insertEmbed(){
		insertEmbedImpl(
				scraperParms.getBaseUrl(), 
				scraperParms.getHost(), 
				scraperParms.getPort(), 
				scraperParms.getContext(), 
				scraperParms.getContainerId(), 
				scraperParms.getScraperId(), 
				scraperParms.getBlastId(), 
				scraperParms.getBlasterId(), 
				scraperParms.getKey()
		);
	}
	private final native void insertEmbedImpl(String baseUrl, String host, String port,
			String context, String containerId,	String scraperId, String blastId, String blasterId, String key) /*-{
		var container = $wnd.document.getElementById(containerId);
		//var unsafeHost = 'ksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfj';
		//var unsafePort = 'dfgdgd';
		var ssl = baseUrl.indexOf('https://') == 0;
		var content = '<EMBED id="' + scraperId + '" name="shodat" WIDTH=2 HEIGHT=2 ' 
			+ ' type="application/x-shodat-plugin" '
			+ ' blastId="' + blastId + '"'
			+ ' blasterId="' + blasterId + '"'
			+ ' key="' + key + '"'
			+ ' host="' + host + '"'
			+ ' port="' + port + '"'
			+ ' context="' + context + '"'
			+ ' ssl="' + ssl + '"'
			+ '/>';
		//alert(content);
		container.innerHTML = content;
	}-*/;
	private final native void removeEmbed(String containerId) /*-{
		var container = $wnd.document.getElementById(containerId);		
		container.innerHTML = "";
	}-*/;

	static private class InstallDialog extends DialogBox {
		
	    public InstallDialog(final ScraperParms scraperParms) {
	      setText("Plugin Install");
	      setAnimationEnabled(true);
	      setGlassEnabled(true);
	      Button ok = new Button("OK");
	      
	      ok.addClickHandler(new ClickHandler() {
	        public void onClick(ClickEvent event) {
	          InstallDialog.this.hide();
	          String document = "shodat.exe";
	          if(-1 < Window.Navigator.getUserAgent().indexOf(".NET")){
	        	  document = "shod.at_installer.application?blastId="
	        		  + scraperParms.getBlastId()
	        		  + "&blasterId=" + scraperParms.getBlasterId()
	        		  + "&key=" + scraperParms.getKey(); 
	          }
	          Window.open(scraperParms.getBaseUrl() + document, 
						"deskblast_install", null);
	        }
	      });
	      Button cancel = new Button("Cancel");
	      cancel.addClickHandler(new ClickHandler() {
	        public void onClick(ClickEvent event) {
	          InstallDialog.this.hide();
	        }
	      });
	      FlowPanel okCancel = new FlowPanel();
	      okCancel.add(ok);
	      okCancel.add(cancel);

	      DockLayoutPanel p = new DockLayoutPanel(Unit.EM);
	      p.setHeight("6em");
	      p.setWidth("18em");
	      p.addNorth(new HTML("To share your desktop, Shodat needs to install a small plugin.  To continue, click 'OK'"), 4);
	      p.addEast(okCancel, 8);
	      setWidget(p);
	    }
    }
}
