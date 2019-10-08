package com.deskblast.client;

import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.deskblast.client.json.BlasterCredentials;
import com.deskblast.client.json.BlasterInfo;
import com.deskblast.client.json.ChatMessage;
import com.deskblast.client.json.NumberWrappedBlastMessage;
import com.deskblast.client.json.ParticipantInfo;
import com.deskblast.client.json.RosterMessage;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.core.client.JsArray;
import com.google.gwt.dom.client.Document;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.http.client.URL;
import com.google.gwt.user.client.Cookies;
import com.google.gwt.user.client.Timer;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.WindowResizeListener;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.rpc.ServiceDefTarget;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.ClickListener;
import com.google.gwt.user.client.ui.DecoratedStackPanel;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.DockPanel;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.FormPanel;
import com.google.gwt.user.client.ui.HasHorizontalAlignment;
import com.google.gwt.user.client.ui.HasVerticalAlignment;
import com.google.gwt.user.client.ui.Image;
import com.google.gwt.user.client.ui.KeyboardListenerAdapter;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.SimplePanel;
import com.google.gwt.user.client.ui.TabPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.google.gwt.user.client.ui.Widget;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Deskblast implements EntryPoint, JoinedHandler, WindowResizeListener {

	
	private static final String DESKTOP_ID = "desktop";
	
	private static final String MOUSE_GIF_STYLE = "mouse-gif";

	private static final String DELETE_ME_STYLE = "delete-me";
	
    public static final String DOC_SCRAPES = "/scrapes?";

	public static final String SAMPLE_SCRAPE = "/test_scrape_0_0.jpg";
	
	public static final String MOUSE_POINTER = "/images/mouse_pointer.gif";

	private static final String START_SHARING_BUTTON_ID = "start-sharing-button";

	private static final String SCRAPER_ID = "scraper";
	
	private static final String SCRAPER_CONTAINER_ID = "scraper-container";

	private static Logger logger = Logger.getLogger(Deskblast.class + "");
	
	static String buildCredentialParms(BlasterCredentials blasterCredentials) {
		String parms = "?blastId=" + blasterCredentials.getBlastId()
				+ "&blasterId=" + blasterCredentials.getBlasterInfo().getId()
				+ "&key=" + blasterCredentials.getKey();
		return URL.encode(parms);
	}
	private static String oneUp = null;
	public static String buildServletUrl(String servlet){
	    if(oneUp == null){
	        String moduleBaseUrl = GWT.getModuleBaseURL().substring(0, GWT.getModuleBaseURL().length()-1);
	        oneUp = moduleBaseUrl.substring(0, moduleBaseUrl.lastIndexOf("/"));
	    }
	    return oneUp + servlet;	    
	}
	private static class JoinDialog extends DialogBox {

		private JoinedHandler joinedHandler;
		private RpcBlasterCredentials rpcBlasterCredentials;
		
		public void show(){
			super.show();
			this.getWidget().getElement().getChild(0).getChild(0);
		}		
		public final void displayError(String message) {
			Window.alert(message);
			hide();
		}
		public void handleCreateOrJoinResponse(
				RpcBlasterCredentials rpcBlasterCredentials) {
			this.rpcBlasterCredentials = rpcBlasterCredentials; 
			startOrJoinBlast(rpcBlasterCredentials);
		}
		private void fireHandleJoin(){
			joinedHandler.handleJoin(rpcBlasterCredentials);
		}
		private void startOrJoinBlast(RpcBlasterCredentials rpcBlasterCredentials) {
		    //Window.alert("got credentials");
//		    Dictionary parms = Dictionary.getDictionary("parms");
//		    Window.alert(parms.get("blastId"));
			
			if(rpcBlasterCredentials.isOwner()){
				StartBlastServiceAsync startBlastService = 
					GWT.create(StartBlastService.class);
				UrlUtil.oneUpService(startBlastService);
				AsyncCallback<Void> callback = new AsyncCallback<Void>() {
					@Override
					public void onFailure(Throwable caught) {
						displayError(caught.getMessage());
					}
					@Override
					public void onSuccess(Void result) {
						hide();
						fireHandleJoin();
					}
				};
				startBlastService.startBlast(rpcBlasterCredentials, callback);
			}else{
				JoinBlastServiceAsync joinBlastService = 
					GWT.create(JoinBlastService.class);
				UrlUtil.oneUpService(joinBlastService);
				AsyncCallback<Void> callback = new AsyncCallback<Void>() {
					@Override
					public void onFailure(Throwable caught) {
						displayError(caught.getMessage());
					}
					@Override
					public void onSuccess(Void result) {
						hide();
						fireHandleJoin();
					}
				};
				joinBlastService.joinBlast(rpcBlasterCredentials, callback);
			}
			/*
			String json = new JSONObject(rpcBlasterCredentials).toString();
			//Window.alert(json);
			RequestBuilder builder = new RequestBuilder(
					RequestBuilder.POST, rpcBlasterCredentials.isOwner() ?
					buildServletUrl(START_BLAST_URL) : buildServletUrl(JOIN_URL));
			try {				
				Request request = builder.sendRequest(json,
						new RequestCallback() {
							public void onError(Request request,
									Throwable exception) {
								displayError("Couldn't start/join blast");
							}
							public void onResponseReceived(
									Request request, Response response) {
								if (200 == response.getStatusCode()) {
									hide();
									//Window.alert(" OK");
									fireHandleJoin();
								} else {
									displayError("Couldn't retrieve JSON ("
											//+ response.getStatusText() 
											+ response.getText()
											+ ")");
								}
							}
						});
			} catch (RequestException e) {
				displayError("Couldn't start blast");
			}
			*/
		}
		void setJoinedHandler(JoinedHandler joinedHandler) {
			JoinDialog.this.joinedHandler = joinedHandler;
		}
		public JoinDialog() {
			setText("Join Meeting");
			final FormPanel form = new FormPanel();
			VerticalPanel panel = new VerticalPanel();
			form.setWidget(panel);
			final TextBox tbHost = new TextBox();
			tbHost.setTitle("Host name");
			panel.add(tbHost);

			final TextBox tbJoiner = new TextBox();
			tbJoiner.setTitle("Your name (if joining)");
			panel.add(tbJoiner);			
			
			panel.add(new Button("Submit", new ClickListener() {
				public void onClick(Widget sender) {
					boolean joining = tbJoiner.getText().trim().length() > 0;
					String url = null;
					if(joining){
						CreateBlasterServiceAsync createBlasterService = GWT.create(CreateBlasterService.class);
						UrlUtil.oneUpService(createBlasterService);
				        AsyncCallback<RpcBlasterCredentials> callback = new AsyncCallback<RpcBlasterCredentials>() {
						      public void onFailure(Throwable caught) {
						        // TODO: Do something with errors.
						    	  caught.printStackTrace();
						      }
							@Override
							public void onSuccess(RpcBlasterCredentials result) {
								handleCreateOrJoinResponse(result);
							}
						};		
						createBlasterService.createBlaster(tbHost.getText().trim(), 
								tbJoiner.getText().trim(), callback);
//						url = buildServletUrl(CREATE_BLASTER_URL) + "?ownerName=" + tbHost.getText().trim()
//							+ "&joinerName=" + tbHost.getText().trim();
					}else{
						//url = buildServletUrl(CREATE_BLAST_URL) + "?ownerName=" + tbHost.getText();
						CreateBlastServiceAsync createBlastService = GWT.create(CreateBlastService.class);
						UrlUtil.oneUpService(createBlastService);
				        AsyncCallback<RpcBlasterCredentials> callback = new AsyncCallback<RpcBlasterCredentials>() {
						      public void onFailure(Throwable caught) {
						        // TODO: Do something with errors.
						    	  caught.printStackTrace();
						      }
							@Override
							public void onSuccess(RpcBlasterCredentials result) {
								handleCreateOrJoinResponse(result);
							}
						};		
						createBlastService.createBlast(tbHost.getText().trim(), 
								 callback);
					}
				}
			}));
			setWidget(form);			
		}
	}
	JoinDialog joinDialog = new JoinDialog();
	ParticipantInfo participantInfo = null;
	//AbsolutePanel desktop = null;
	DockPanel appsharePanel = null;
//	TileDisplay tileDisplay = null;
	WaitingTileDisplay tileDisplay = null;
	TextArea chatDisplay = null;
	TextArea chatEntry = new TextArea();
	DockPanel chatEntryPanel = new DockPanel();
	Button chatSendButton = new Button("Send");	
	Button startSharing = new Button("Show My Desktop");	
	//Button stopSharing = new Button("Stop Showing My Desktop");
	//DialogBox controls = new DialogBox(false, false);
	SimplePanel controls = new SimplePanel();//DialogBox(false, false);
//	final HorizontalSplitPanel horizontalSplitPanel = new HorizontalSplitPanel();
	//final VerticalSplitPanel verticalSplitPanel = new VerticalSplitPanel();
	//DockPanel chatPanel = new DockPanel();
	VerticalPanel chatPanel = new VerticalPanel();
	TabPanel chatTabPanel = new TabPanel();
	DockPanel rosterPanel = new DockPanel();
	FlexTable roster = new FlexTable();
	DecoratedStackPanel stackPanel = new DecoratedStackPanel();
	private RpcBlasterCredentials rpcBlasterCredentials;
	private int expectedMessageNumber = 0;
	private Image mouseGif = null;
	private int tileSerialNumber = 0;
	private ScraperImplDefault scraperImpl = null;
	
	private ClientBoundMessageServiceAsync clientBoundMessageService = 
						GWT.create(ClientBoundMessageService.class);	
	private ServerBoundMessageServiceAsync serverBoundMessageService = 
						GWT.create(ServerBoundMessageService.class);
	private boolean clientBoundConfigured = false;
	private boolean serverBoundConfigured = false;
	
	void sendRpcChatMessage(){
//		Window.alert("sending chat");
		RpcChatMessage rpcChatMessage = new RpcChatMessage(
				chatEntry.getText().trim());
		rpcChatMessage.setSender(this.rpcBlasterCredentials.getRpcBlasterInfo());		
		sendRpcMessage(rpcChatMessage);
		chatEntry.setText("");
	}
	private String getContext() {
		if(GWT.getModuleBaseURL().endsWith("deskblast/deskblast/")){
			return "deskblast";
		}
		return "";
	}
	private String getPort(){
		String port = "80";
		String page = Document.get().getURL();
		String host = Document.get().getDomain();
		int colonIndex = page.indexOf(host) + host.length();
		if(page.charAt(colonIndex) == ':'){
			int rootSlash = page.indexOf("/", colonIndex);
			port = page.substring(colonIndex+1, rootSlash);
		}else if(page.toLowerCase().startsWith("https://")){
			port = "443";
		}
		return port;
	}
	private boolean isSsl(){
	    return Document.get().getURL().toLowerCase().startsWith("https://");
	}
	void displayError(String message){
		Window.alert(message);
	}
	void sendRpcMessage(RpcMessage rpcMessage){
		if(!serverBoundConfigured){
			serverBoundConfigured = true;
			ServiceDefTarget endpoint = (ServiceDefTarget) serverBoundMessageService;
			String url = endpoint.getServiceEntryPoint();
			endpoint.setServiceEntryPoint(UrlUtil.oneUpUrl(url));
		}
        AsyncCallback<Void> callback = new AsyncCallback<Void>() {
		      public void onFailure(Throwable caught) {
		        // TODO: Do something with errors.
		    	  caught.printStackTrace();
		      }
		      public void onSuccess(Void voyd) {		        
		      }
		};		
		serverBoundMessageService.sendMessages(new RpcMessage[]{rpcMessage}, 
				this.rpcBlasterCredentials.getBlastId(), 
				rpcBlasterCredentials.getRpcBlasterInfo().getId(), 
				rpcBlasterCredentials.getKey(), callback);
	}
	public void onModuleLoad() {
		Window.setMargin("0px");
		Window.addWindowResizeListener(this);
		this.startSharing.getElement().setId(START_SHARING_BUTTON_ID);
		this.startSharing.addStyleName("centered");
		this.startSharing.addClickHandler(new ClickHandler(){
            public void onClick(ClickEvent event) {
                handleStartScrapingClicked();
            }
	    });
		chatDisplay = new TextArea();
		chatDisplay.setWidth("100%");
		chatDisplay.setHeight("70px");
		chatDisplay.setReadOnly(true);
		chatPanel.setSize("100%", "100%");
		chatEntry.setWidth("100%");
		chatEntry.setHeight("40");
		chatEntryPanel.add(chatEntry, DockPanel.CENTER);
		chatEntryPanel.add(chatSendButton, DockPanel.EAST);
		chatSendButton.setHeight("100%");
		chatSendButton.addClickListener(new ClickListener() {
			public void onClick(Widget sender) {
				sendRpcChatMessage();
			}
		});
		chatEntry.addKeyboardListener(new KeyboardListenerAdapter(){
			@Override
			public void onKeyUp(Widget sender, char keyCode, int modifiers) {
				if(keyCode == KEY_ENTER){
					sendRpcChatMessage();
				}else{
					super.onKeyUp(sender, keyCode, modifiers);
				}
			}
		});
		chatEntryPanel.setWidth("100%");
		chatEntryPanel.setCellVerticalAlignment(chatSendButton,
				HasVerticalAlignment.ALIGN_MIDDLE);
		chatEntryPanel.setCellWidth(chatEntry, "100%");
		stackPanel.setWidth("200px");
		stackPanel.add(rosterPanel, "People", false);
		stackPanel.add(chatPanel, "Chat", false);
		controls.add(stackPanel);
		Window.setMargin("2");
		chatPanel.add(chatDisplay);//, DockPanel.CENTER);
		chatPanel.add(chatEntryPanel);//, DockPanel.SOUTH);
		chatPanel.setHeight("100%");
		rosterPanel.add(roster, DockPanel.CENTER);
		rosterPanel.setCellHeight(roster, "100%");
		rosterPanel.setSize("100%","100%");
		roster.setWidth("100%");
		appsharePanel = new DockPanel();
		appsharePanel.getElement().setId("appshare-background-container");
		appsharePanel.setStylePrimaryName("appshare-background");
		appsharePanel.setWidth("100%");
		appsharePanel.setHeight("100%");
        appsharePanel.setHorizontalAlignment(HasHorizontalAlignment.ALIGN_CENTER);
        appsharePanel.setVerticalAlignment(HasVerticalAlignment.ALIGN_MIDDLE);
		Label label = new Label();
		label.setStylePrimaryName("scraper-container");
		label.getElement().setId(SCRAPER_CONTAINER_ID);		
		RootPanel.get().add(label);
		RootPanel.get().add(appsharePanel);
		controls.addStyleName("fixed-bottom-right");
		controls.setVisible(false);
		RootPanel.get().add(controls);
		RootPanel.get().addStyleName("dark-gray-background");
		RpcBlasterCredentials credentials = null;
		try{
			BlasterCredentials bc = getWindowVariable("hostCredentials").cast();
			credentials = credentialsFromCredentials(bc);
		}catch(Throwable t){}	
		if(credentials == null){
			try{
				BlasterCredentials bc = getWindowVariable("guestCredentials").cast();
				credentials = credentialsFromCredentials(bc);
			}catch(Throwable t){}
		}
		if(credentials != null ){
			joinDialog.setJoinedHandler(this);
			joinDialog.handleCreateOrJoinResponse(credentials);
		}else{
			joinDialog.setJoinedHandler(this);
			joinDialog.center();
			joinDialog.show();
		}
	}
	private RpcBlasterCredentials credentialsFromCredentials(BlasterCredentials bc){
		RpcBlasterCredentials rbc = new RpcBlasterCredentials(
				new RpcBlasterInfo(bc.getBlasterInfo().getName(), bc.getBlasterInfo().getId()), 
				bc.getBlastId(), 
				bc.getKey(), 
				bc.isOwner()
				);
		return rbc;
	}
	private final native JavaScriptObject getWindowVariable(String name) /*-{
		return $wnd[name];
	}-*/;
    private final native JsArray<NumberWrappedBlastMessage> getMessages(String input) /*-{
	    return eval('(' + input + ')');
	}-*/;
	
    private void handleStartScrapingClicked(){
    	if(this.scraperImpl == null){
    		this.scraperImpl = GWT.create(ScraperImplDefault.class);
    	}
    	ScraperParms scraperParms = new ScraperParms(
    			SCRAPER_CONTAINER_ID, 
    			SCRAPER_ID,
    			Document.get().getDomain(), 
    			getPort(), 
    			getContext(),
    			this.rpcBlasterCredentials.getBlastId()+"", 
    			this.rpcBlasterCredentials.getRpcBlasterInfo().getId()+"", 
    			this.rpcBlasterCredentials.getKey(), 
    			isSsl());
    	boolean pluginAlreadyInstalled = this.scraperImpl.installPlugin(scraperParms);
    	if(pluginAlreadyInstalled){
    		sendStartScrapingMessage();
    	}
    }
    private void handleRpcMessages(RpcMessage[] rpcMessages){
    	if(rpcMessages.length > 1){
    		logger.log(Level.INFO, "got " + rpcMessages.length + " messages");
    	}
    	for(RpcMessage rpcMessage : rpcMessages){
    		if(rpcMessage instanceof RpcChatMessage){
    			handleRpcChat((RpcChatMessage)rpcMessage);
    		}else if(rpcMessage instanceof RpcRosterMessage){
    			handleRpcRoster((RpcRosterMessage)rpcMessage);
    		}else if(rpcMessage instanceof RpcScrapeMessage){
    			handleRpcScrapeMessage((RpcScrapeMessage)rpcMessage);
    		}
    	}
    	// TODO: wrapping in timer is experimental -- does not seem
    	// to remedy memory issue
    	new Timer(){
			@Override
			public void run() {
		    	rpcMessageLoop();
			}
    	}.schedule(1);
    }
	private void handleRpcScrapeMessage(RpcScrapeMessage rpcScrapeMessage){
		if(rpcScrapeMessage instanceof RpcStartScrapingMessage){
			rpcHandleStartScrapingMessage((RpcStartScrapingMessage)rpcScrapeMessage);
		}else if(rpcScrapeMessage instanceof RpcStopScrapingMessage){
			rpcHandleStopScrapingMessage((RpcStopScrapingMessage)rpcScrapeMessage);
		}else if(rpcScrapeMessage instanceof RpcScrapingScrapeMessage){
			rpcHandleScrape((RpcScrapingScrapeMessage)rpcScrapeMessage);
		}else{
			throw new RuntimeException("unexpected scrape message type: " 
					+ rpcScrapeMessage);
		}
	}
	private void rpcHandleStopScrapingMessage(RpcStopScrapingMessage rpcStopScrapingMessage){
		// TODO: make sure laggard scrapes are ignored?
		showNotSharingDesktop();
		setHtmlTitle("");
	}
	private void handleStopScrapingMessage() {
		// TODO: make sure laggard scrapes are ignored?
		showNotSharingDesktop();
		setHtmlTitle("");
	}
	private void rpcHandleStartScrapingMessage(RpcStartScrapingMessage rpcStartScrapingMessage){
		if(rpcStartScrapingMessage.getSender().getId() 
				== this.rpcBlasterCredentials.getRpcBlasterInfo().getId()){
	        startScraping();	
	    }else{
	        // show label: 'nnn is about to share their desktop'
	    }
	}
    private void startScraping(){
    	this.scraperImpl.startScraping();
    	// TODO: show sharing desktop only after scrapes have ben received on server
    	// or after scraper fires scraping event
    	showSharingDesktop();
	}    
    private void handleRoster(RosterMessage rosterMessage) {
		roster.clear(true);
		JsArray<BlasterInfo> participants = rosterMessage.getBlasterInfos(); 
		for(int i = 0; i < participants.length(); i++){
			roster.setText(i, 0, participants.get(i).getName());
		}
	}
    private void handleRpcRoster(RpcRosterMessage rpcRosterMessage){
    	roster.clear();
    	RpcBlasterInfo[] infos = rpcRosterMessage.getRpcBlasterInfos();
    	for(int i = 0; i < infos.length; i++){
    		roster.setText(i, 0, infos[i].getName());
    	}
    }
	private void handleChat(ChatMessage chatMessage) {
//		Window.alert("you've got chat");
		this.chatDisplay.setText(this.chatDisplay.getText()
				+ chatMessage.getSender().getName() + ":"
				+ chatMessage.getText() + "\n");
		this.chatDisplay.getElement().setScrollTop(
				this.chatDisplay.getElement().getScrollHeight());
	}
	private void handleRpcChat(RpcChatMessage rpcChatMessage){
		this.chatDisplay.setText(this.chatDisplay.getText()
				+ rpcChatMessage.getSender().getName() + ":"
				+ rpcChatMessage.getText() + "\n");
		this.chatDisplay.getElement().setScrollTop(
				this.chatDisplay.getElement().getScrollHeight());
	}
	static long keyframeSetNumber = 0;
	private void rpcHandleScrape(RpcScrapingScrapeMessage rpcScrapingScrapeMessage){
		this.tileDisplay.handleScrape(rpcScrapingScrapeMessage);
	}
	private void showNotSharingDesktop(){
		//Window.alert("showNotScrapingDesktop");
		//removeScrapingObject(SCRAPER_CONTAINER_ID);
		if(this.scraperImpl != null){
			this.scraperImpl.stopScraping();
		}
		//removeDesktopTiles();
		tileDisplay.showBlank();
	    this.startSharing.setVisible(true);
	    this.startSharing.getElement().scrollIntoView();
	    //this.startSharing.getElement().
	}
	private void showSharingDesktop(){
		this.startSharing.setVisible(false);
		//this.stopSharing.setVisible(true);
	}
	private void rpcMessageLoop(){
		  AsyncCallback<RpcMessage[]> callback = new AsyncCallback<RpcMessage[]>() {
		      public void onFailure(Throwable caught) {
		        // TODO: Do something with errors.
		      }
		      public void onSuccess(RpcMessage[] result) {
		        handleRpcMessages(result);
		      }
		  };
		  if(!clientBoundConfigured){
			  clientBoundConfigured = true;
			  ServiceDefTarget endpoint = (ServiceDefTarget) clientBoundMessageService;
			  String url = endpoint.getServiceEntryPoint();
			  endpoint.setServiceEntryPoint(UrlUtil.oneUpUrl(url));			  
		  }
		  clientBoundMessageService.getMessages(
				  rpcBlasterCredentials.getBlastId(), 
				  rpcBlasterCredentials.getRpcBlasterInfo().getId(), 
				  rpcBlasterCredentials.getKey(), callback);
	}
	private void sendStartScrapingMessage() {
		RpcStartScrapingMessage rpcStartScrapingMessage = new RpcStartScrapingMessage();
		rpcStartScrapingMessage.setSender(this.rpcBlasterCredentials.getRpcBlasterInfo());
		sendRpcMessage(rpcStartScrapingMessage);
    }
	public void handleJoin(RpcBlasterCredentials rpcBlasterCredentials) {	
		this.tileDisplay = new WaitingTileDisplay(rpcBlasterCredentials);
		this.tileDisplay.add(this.startSharing);
		appsharePanel.add(tileDisplay, DockPanel.CENTER);
		controls.addStyleName("opacity-95");
		controls.addStyleName("fixed-bottom-right");
		controls.setVisible(true);
		controls.addStyleName("fixed-bottom-right");
		//controls.show();
		this.rpcBlasterCredentials = rpcBlasterCredentials;
		startScrapingIfPending();
		// wrapping message loop in Timer removes the perpetual status bar update
		// and page loading spinner in chrome
		new Timer(){
			@Override
			public void run() {
				rpcMessageLoop();
			}
		}.schedule(1);		
	}
	private void startScrapingIfPending() {
		String startPending = Cookies.getCookie(ScraperImplDefault.SCRAPE_START_PENDING); 
		if(startPending != null){
			Date date = new Date();
			if(date.getTime() < Long.parseLong(startPending)){
				new Timer(){
					@Override
					public void run() {
						handleStartScrapingClicked();
					}
				}.schedule(500);
				
			}
		}
	}
	public void onWindowResized(int width, int height) {
		//horizontalSplitPanel.setSplitPosition("80%");
		//verticalSplitPanel.setSplitPosition("50%");
	}
	
	public static final native void setHtmlTitle(String theTitle) /*-{		
		$wnd.document.getElementsByTagName("HTML")[0].title = theTitle; 
	}-*/;

}
