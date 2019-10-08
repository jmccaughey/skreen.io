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
	
//	private static final String TILE_FOREGROUND_STYLE = "tile-foreground";
	
    public static final String DOC_SCRAPES = "/scrapes?";

	//public static final String CREATE_BLAST_URL = "/createBlast";
	
	//public static final String START_BLAST_URL = "/startBlast";
	
	//public static final String CREATE_BLASTER_URL = "/createBlaster";

	//public static final String JOIN_URL = "/joinBlast";
	
	//public static final String MESSAGES_URL = "/messages";
	
//	public static final String RPC_MESSAGES_URL = "/clientBoundMessages";
	
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
	    	//String realModuleBaseUrl = GWT.getModuleBaseURL();
	        String moduleBaseUrl = GWT.getModuleBaseURL().substring(0, GWT.getModuleBaseURL().length()-1);
//	        Window.alert("context:" + moduleBaseUrl);
	        oneUp = moduleBaseUrl.substring(0, moduleBaseUrl.lastIndexOf("/"));
	        //Window.alert(oneUp);
	    }
	    return oneUp + servlet;	    
	}
	private static class JoinDialog extends DialogBox {

		private JoinedHandler joinedHandler;
		private RpcBlasterCredentials rpcBlasterCredentials;
		
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
					/*
					url = URL.encode(url);
					RequestBuilder builder = new RequestBuilder(
							RequestBuilder.GET, url);
					try {
						Request request = builder.sendRequest(null,
								new RequestCallback() {
									public void onError(Request request,
											Throwable exception) {
										displayError("Couldn't retrieve JSON");
									}
									public void onResponseReceived(
											Request request, Response response) {
										if (200 == response.getStatusCode()) {
											try {
												BlasterCredentials blasterCredentials = BlasterCredentials
														.fromJson(response.getText());
												//Window.alert(response.getText());//handleJoinApiResponse(participantInfo);
												handleCreateOrJoinResponse(blasterCredentials);
											} catch (JSONException e) {
												displayError("Could not parse JSON");
											}
										} else {
											displayError("Couldn't retrieve JSON ("
													+ response.getStatusText() + ")");
										}
									}
								});
					} catch (RequestException e) {
						displayError("Couldn't retrieve JSON");
					}
					*/
				}
			}));
			setWidget(form);			
		}
	}
	JoinDialog joinDialog = new JoinDialog();
	ParticipantInfo participantInfo = null;
	//AbsolutePanel desktop = null;
	DockPanel appsharePanel = null;
	TileDisplay tileDisplay = null;
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
	/*
	void sendJsonMessage(String json) {
		String url = buildServletUrl(MESSAGES_URL) + buildCredentialParms(this.rpcBlasterCredentials);
		RequestBuilder builder = new RequestBuilder(RequestBuilder.POST, url);
		try {
			Request request = builder.sendRequest(json, new RequestCallback() {
				public void onError(Request request, Throwable exception) {
					displayError("Couldn't send json message");
				}
				public void onResponseReceived(Request request,
						Response response) {
					if (200 == response.getStatusCode()) {
					} else {
						displayError("Couldn't post JSON message: "
								+ response.getStatusText());
					}
				}
			});
		} catch (RequestException e) {
			displayError("Couldn't retrieve JSON");
		}
	}
	*/
	public void onModuleLoad() {
		//Window.enableScrolling(false);
		Window.setMargin("0px");
		Window.addWindowResizeListener(this);
		//desktop = new AbsolutePanel();
		//desktop.getElement().setId(DESKTOP_ID);
		this.startSharing.getElement().setId(START_SHARING_BUTTON_ID);
		//this.stopSharing.getElement().setId(STOP_SHARING_BUTTON_ID);
		this.startSharing.addStyleName("centered");
		//this.stopSharing.addStyleName("centered");
		//this.desktop.add(this.startSharing);
		//this.desktop.add(this.stopSharing);
		//this.stopSharing.setVisible(false);
		this.startSharing.addClickHandler(new ClickHandler(){
            public void onClick(ClickEvent event) {
                handleStartScrapingClicked();
            	//sendStartScrapingMessage();
            }
	    });
//		this.stopSharing.addClickHandler(new ClickHandler(){
//            public void onClick(ClickEvent event) {
//                stopSharing();                
//            }
//	    });
		//horizontalSplitPanel.setWidth("100%");
		//horizontalSplitPanel.setHeight("100%");
		//verticalSplitPanel.setHeight("100%");
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
		//controls.setWidth("180px");
		//controls.setHeight("160px");
		stackPanel.setWidth("200px");
//		stackPanel.setHeight("350px");
		stackPanel.add(rosterPanel, "People", false);
		stackPanel.add(chatPanel, "Chat", false);
		controls.add(stackPanel);
		//Window.enableScrolling(false);
		Window.setMargin("2");
		//chatPanel.add(new HTML("<h4>CHAT</h4>"), DockPanel.NORTH);
		chatPanel.add(chatDisplay);//, DockPanel.CENTER);
		chatPanel.add(chatEntryPanel);//, DockPanel.SOUTH);
		chatPanel.setHeight("100%");
		// chatPanel.setBorderWidth(1);
		//chatPanel.setCellHeight(chatDisplay, "100%");
		// chatTabPanel.add(chatPanel, "Chat");
		// chatTabPanel.setSize("100%","100%");
//		roster.setBorderWidth(3);
		//rosterPanel.add(new HTML("<h4>PARTICIPANTS</h4>"), DockPanel.NORTH);
		rosterPanel.add(roster, DockPanel.CENTER);
		rosterPanel.setCellHeight(roster, "100%");
		rosterPanel.setSize("100%","100%");
//		rosterPanel.setBorderWidth(3);
		roster.setWidth("100%");
		//verticalSplitPanel.setBottomWidget(chatPanel);// chatTabPanel);
		//verticalSplitPanel.setTopWidget(rosterPanel);
		appsharePanel = new DockPanel();
		appsharePanel.getElement().setId("appshare-background-container");
		appsharePanel.setStylePrimaryName("appshare-background");
		appsharePanel.setWidth("100%");
		appsharePanel.setHeight("100%");
        appsharePanel.setHorizontalAlignment(HasHorizontalAlignment.ALIGN_CENTER);
        appsharePanel.setVerticalAlignment(HasVerticalAlignment.ALIGN_MIDDLE);
		//horizontalSplitPanel.setLeftWidget(dockPanel);
		//horizontalSplitPanel.setSplitPosition("80%");
		//Element divElement = DOM.createDiv();		
		//divElement.setId(SCRAPER_CONTAINER_ID);
		Label label = new Label();
		label.setStylePrimaryName("scraper-container");
		label.getElement().setId(SCRAPER_CONTAINER_ID);		
		RootPanel.get().add(label);
		RootPanel.get().add(appsharePanel);
		controls.addStyleName("fixed-bottom-right");
		controls.setVisible(false);
		//desktop.add(controls);
		RootPanel.get().add(controls);
		RootPanel.get().addStyleName("dark-gray-background");
		//controls.setHTML("<b>Deskblast</b>");
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
	/*
	private RpcBlasterCredentials credentialsFromCredentials(JavaScriptObject jso){
		JSONObject jsonObject = new JSONObject(jso);
		JSONValue jsonValue = jsonObject.get("blasterInfo");
		Double blasterId = Double.valueOf(jsonValue.isObject().get("id").isNumber().doubleValue());
		RpcBlasterInfo rpcBlasterInfo = new RpcBlasterInfo(name, (int)blasterId);
		RpcBlasterCredentials rpcBlasterCredentials = new 
	}
	*/
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
    /*
	private void handleMessages(String json) {
		GWT.log(json, null);
		if( ! (json.length() < 5)){
			JsArray<NumberWrappedBlastMessage> blastMessages = getMessages(json);
			for(int i = 0, n = blastMessages.length(); i < n; i++){
				NumberWrappedBlastMessage nwbm = blastMessages.get(i);
				int messageNumber = nwbm.getNumber(); 
				if(messageNumber != this.expectedMessageNumber){
					//Window.alert("expected message #" + this.expectedMessageNumber
					//		+ " but got #" + messageNumber);
//					System.out.println("expected message #" + this.expectedMessageNumber
//							+ " but got #" + nwbm.getNumber());
//					throw new RuntimeException("messages out of sequence: " + "expected message #" + this.expectedMessageNumber
//							+ " but got #" + messageNumber);
				}
				BlastMessage blastMessage = nwbm.getMessage(); 
				String javaClass = blastMessage.getJavaClass();
				//Window.alert("got message: " + javaClass);
				if (javaClass.indexOf("ChatMessage") > -1) {
					handleChat(
						ChatMessage.fromJson(new JSONObject(blastMessage).toString())
					);
				} else if (javaClass.indexOf("Scrap") > -1) {
				    handleScrapeMessage(blastMessage, javaClass, messageNumber);
				} else if(javaClass.indexOf("Roster") > -1){
					handleRoster(RosterMessage.fromJson(
							new JSONObject(blastMessage).toString()));
				}
				expectedMessageNumber = (nwbm.getNumber() + 1);
			}
		}
		messageLoop();
	}
	*/
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
	/*
	private void handleScrapeMessage(BlastMessage blastMessage, String javaClass, int messageNumber) {
	    if(javaClass.indexOf("StartScrap") > -1){
	        handleStartScrapingMessage(
	                StartScrapingMessage.fromJson(
	                        new JSONObject(blastMessage).toString()
	                )
	        );
	    	if(blastMessage.getSender().getId() == 
	    		this.rpcBlasterCredentials.getBlasterInfo().getId()){
	    		setHtmlTitle("Showing " 
	    				+ this.rpcBlasterCredentials.getBlasterInfo().getName() 
	    				+ "'s desktop");
	    	}
	    }else if(javaClass.indexOf("StopScrap") > -1){
	    	handleStopScrapingMessage();
	    }else{
	        handleScrape(
	                ScrapeMessage.fromJson(
	                        new JSONObject(blastMessage).toString()
	                ), 
	                messageNumber);
	    }
    }
    */
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
//	private void handleStartScrapingMessage(StartScrapingMessage startScrapingMessage) {
//	    if(startScrapingMessage.getSender().getId() 
//	            == this.rpcBlasterCredentials.getBlasterInfo().getId()){
//	        startScraping();	
//	    }else{
//	        // show label: 'nnn is about to share their desktop'
//	    }
//    }
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
//	private void updateMouse(int x, int y, int width, int height){
//		if(null == mouseGif){
//		    mouseGif = new Image();
//		    mouseGif.setVisible(false);
//		    mouseGif.addStyleName(MOUSE_GIF_STYLE);
//		    mouseGif.setUrl(buildServletUrl(MOUSE_POINTER));
//		    desktop.add(mouseGif, 100, 100);
//		}
//		mouseGif.setVisible(true);
//		desktop.setWidgetPosition(mouseGif, 
//		            Math.min(x, width), 
//		            Math.min(y, height)
//		);
//		mouseGif.getElement().scrollIntoView();			
//	}
	private void rpcHandleScrape(RpcScrapingScrapeMessage rpcScrapingScrapeMessage){
		//RpcScreenScrapeData rpcScreenScrapeData = rpcScrapingScrapeMessage.getRpcScreenScrapeData();
		this.tileDisplay.handleScrape(rpcScrapingScrapeMessage);
//		boolean handlingKeyFrame = false;
//		if(rpcScreenScrapeData instanceof RpcScreenScrapeMetaData){			 
//			RpcScreenScrapeMetaData ssmd = (RpcScreenScrapeMetaData)rpcScreenScrapeData;
//			handlingKeyFrame = ssmd.isKeyFrame();
//			desktop.setWidth(ssmd.getWidth() + "px");
//			desktop.setHeight(ssmd.getHeight() + "px");
//			updateMouse(ssmd.getMouseX(), ssmd.getMouseY(), 
//					ssmd.getWidth(), ssmd.getHeight());
//			if(handlingKeyFrame){
//				keyframeSetNumber++;
//			}
//		}
//		final String keyframeSetId = "keyframe_" + keyframeSetNumber;
//		RpcAbstractTileInfo[] infos = rpcScreenScrapeData.getCompressedRects();
//		LoadHandler loadHandler = new DesktopImageLoadListener(
//		        infos.length, handlingKeyFrame, keyframeSetId);		
//		for(RpcAbstractTileInfo info : infos){
//			final String source = buildServletUrl(URL_SCRAPES) 
//			    + info.getHash() 
//			    + "&blastId=" + this.rpcBlasterCredentials.getBlastId()
//			    + "&key=" + this.rpcBlasterCredentials.getKey()
//			    + "&blasterId=" + this.rpcBlasterCredentials.getRpcBlasterInfo().getId();
//			final Image image = new Image();
//			image.setVisible(false);			
//			if (handlingKeyFrame) {
//				image.addStyleName(keyframeSetId);				
//			}
//			String positionStyle = "tile_position_x" + info.x
//												+ "_y" + info.y; 
//			image.addStyleName(positionStyle);
//			image.addStyleName("serial_" + tileSerialNumber++);
//	//		image.addStyleName(TILE_FOREGROUND_STYLE);
//			// TODO use registration to remove handler
//	        HandlerRegistration reg = image.addLoadHandler(loadHandler);
//	        logger.log(Level.SEVERE, "setting source for " + positionStyle 
//	        		+ " :" + source);
//			image.setUrl(source);
//			markExpiringImage(positionStyle);            
//			desktop.add(image, info.x, info.y);
//		}
	}
	/*
	private void handleScrape(ScrapeMessage scrape, int messageNumber) {		
		final boolean handlingKeyFrame = scrape.getKeyframe();
		if (handlingKeyFrame) {
			String width = scrape.getDeskWidth() + "px";
			String height = scrape.getDeskHeight() + "px";
			desktop.setWidth(width);
			desktop.setHeight(height);
		}
		final String keyframeSetId = "keyframe_" + messageNumber;
		JsArray<ScrapeInfo> scrapeInfos = scrape.getScrapeInfos();
		final int scrapeCount = scrapeInfos.length();
		LoadHandler loadHandler = new DesktopImageLoadListener(
		        scrapeCount, handlingKeyFrame, keyframeSetId);
		// give each image a style from its position
		// example: "tile_position_x512_y0" then check for existing image
		// with same style, add class 'remove_me'
		// on load, find by "tile_x512_y0" and "remove_me" and remove
		for(int i = 0; i < scrapeCount; i++) {
			final ScrapeInfo scrapeInfo = scrapeInfos.get(i);			
			final String source = buildServletUrl(URL_SCRAPES) 
			    + scrapeInfo.getName() 
			    + "&blastId=" + this.rpcBlasterCredentials.getBlastId()
			    + "&key=" + this.rpcBlasterCredentials.getKey()
			    + "&blasterId=" + this.rpcBlasterCredentials.getBlasterInfo().getId();
			final Image image = new Image();
			image.setVisible(false);			
			if (handlingKeyFrame) {
				image.addStyleName(keyframeSetId);				
			}
			String positionStyle = "tile_position_x" + scrapeInfo.getX()
												+ "_y" + scrapeInfo.getY(); 
			image.addStyleName(positionStyle);
			image.addStyleName("serial_" + tileSerialNumber++);
//			image.addStyleName(TILE_FOREGROUND_STYLE);
			// TODO use registration to remove handler
            HandlerRegistration reg = image.addLoadHandler(loadHandler);
			image.setUrl(source);
			markExpiringImage(positionStyle);            
			desktop.add(image, scrapeInfo.getX(), scrapeInfo.getY());
//			image.setWidth(scrapeInfo.getWidth()+"px");
//			image.setHeight(scrapeInfo.getHeight()+"px");
		}
		if(null == mouseGif){
		    mouseGif = new Image();
		    mouseGif.setVisible(false);
		    mouseGif.addStyleName(MOUSE_GIF_STYLE);
		    mouseGif.setUrl(buildServletUrl(MOUSE_POINTER));
		    desktop.add(mouseGif, 100, 100);
		}
		mouseGif.setVisible(true);
		desktop.setWidgetPosition(mouseGif, 
		            Math.min(scrape.getMouseX(), scrape.getDeskWidth()), 
		            Math.min(scrape.getMouseY(), scrape.getDeskHeight())
		);
		mouseGif.getElement().scrollIntoView();
		// start timeout for cleanup
		// testing for flicker
//		Timer t = new Timer() {
//		      @Override
//		      public void run() {
//		    	  hideDeleteMeImages();
//		      }
//		};
//		t.schedule(10000);
	}
	*/
//	private void hideDeleteMeImages(){
//    	Iterator<Widget> iter = desktop.iterator();
//        while (iter.hasNext()) {
//            Widget image = iter.next();
//            if (image.getStyleName().indexOf(DELETE_ME_STYLE) > -1) {
//            	//image.getElement().
//                image = null;
//                iter.remove();
//            }
//        }
//	}
//	private void markExpiringImage(String positionStyle) {
//    	Iterator<Widget> iter = desktop.iterator();
//        while (iter.hasNext()) {
//            Widget image = iter.next();
//            if (image.getStyleName().indexOf(positionStyle) > -1){
//                image.addStyleName(DELETE_ME_STYLE);
//                //image.removeStyleName(TILE_FOREGROUND_STYLE);
//            }
//        }
//	}
//	class DesktopImageLoadListener implements LoadHandler{
//        public DesktopImageLoadListener(int keyframeSetSize,
//                 boolean handlingKeyframeSet, String keyframeSetId) {
//            super(); 
//        }
//        public void onLoad(LoadEvent event) {
//        	// TODO: idea for improving image fetching:
//        	// each desktop region has a queue of urls
//        	// and a handler for load complete
//        	// on load complete, if the queue has more than one 
//        	// queued URL, drop all except the latest and start loading
//        	// that one
//            Image loadedImage = (Image)event.getSource();
//            logger.log(Level.SEVERE, "loaded " + loadedImage.getStyleName() 
//            		+ " " + loadedImage.getUrl());
//            loadedImage.setVisible(true);
//            // delete same positioned 
//            // visible tile if two others are visible
//            Iterator<Widget> iter = desktop.iterator();
//            List<Widget> sameTiles = new ArrayList<Widget>();
//            String locationStyle = StyleUtil.getStyleByPrefix(
//                    "tile_position", loadedImage.getStyleName());
//            while (iter.hasNext()) {
//              Widget image = iter.next();
//              if(image.getStyleName().indexOf(locationStyle) > -1){
//                  if(image.isVisible() 
//                          && image.getStyleName().indexOf(DELETE_ME_STYLE) > -1){
//                      sameTiles.add(image);
//                  }
//              }
//            }
//            if(sameTiles.size() > 2){
//                desktop.remove(sameTiles.get(0));
//            }
//        }
//	}
//	
//	private void cleanupOldTiles(){
//		int iWidgetCount = desktop.getWidgetCount();
//		if(iWidgetCount > 100){
//			Iterator<Widget> iter = desktop.iterator();
//            while (iter.hasNext() && --iWidgetCount > 70) {
//                Widget image = iter.next();
//                if((image.getStyleName().indexOf(DELETE_ME_STYLE) > -1) && image.isVisible()){
//                    image.setVisible(false);
//                }else if((image.getStyleName().indexOf(DELETE_ME_STYLE) > -1) && (!image.isVisible())){
//                	//image = null;
//                    iter.remove();
//                }
//            }
//		}
//	}
//	private String getPositionStyle(Widget widget) {	
//		String[] styles = widget.getStyleName().split(" ");
//		for(String style : styles){
//			if(style.indexOf("tile_position") > -1){
//				return style;
//			}
//		}
//		return null;
//	}	
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
	// TODO should be remove desktop tiles
//	private void removeDesktopTiles() {
////	    Window.alert("removing all tiles");
//        Iterator<Widget> iter = desktop.iterator();
//        while (iter.hasNext()) {
//            Widget widget = iter.next();
//            if (widget.getElement().getTagName().equalsIgnoreCase("img")){
//            	if(widget == mouseGif){
//            		widget.setVisible(false);
//            	}else{
//            		iter.remove();
//            	}
//            }
//        }
//        desktop.setWidth("");
//        desktop.setHeight("");
//	}
//	private String getScraperId(){
//		return "DeskBlast_" + this.blasterCredentials.getBlastId()
//			+ "_" + this.blasterCredentials.getBlasterInfo().getId()
//			+ "_" + this.blasterCredentials.getKey() 
//			+ "_" + oneUp;
//	}
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
	/*
	private void messageLoop() {
		String url = buildServletUrl(MESSAGES_URL) + buildCredentialParms(this.rpcBlasterCredentials);
		url += "&messageNumber=" + this.expectedMessageNumber;
		RequestBuilder builder = new RequestBuilder(RequestBuilder.GET, url);
		builder.setTimeoutMillis(16*1000);
		try {
			Request request = builder.sendRequest(null, new RequestCallback() {
				public void onError(Request request, Throwable exception) {
					if(exception.toString().indexOf("imeout") > -1){
						new Timer() {
						      @Override
						      public void run() {
						        messageLoop();
						      }
						    }.schedule(500);
					}else{
						Window.alert("Couldn't retrieve message");
					}
				}
				public void onResponseReceived(Request request,
						Response response) {
					if (200 == response.getStatusCode()) {
						handleMessages(response.getText());
					} else {
						Window.alert("Couldn't retrieve message ("
								+ response.getStatusText() + ")");
					}
				}
			});
		} catch (RequestException e) {
			Window.alert("Couldn't retrieve message");
		}
	}
	*/
//    private void stopSharing() {
//    	this.scraperImpl.stopScraping();
//    }
	private void sendStartScrapingMessage() {
		RpcStartScrapingMessage rpcStartScrapingMessage = new RpcStartScrapingMessage();
		rpcStartScrapingMessage.setSender(this.rpcBlasterCredentials.getRpcBlasterInfo());
//        StartScrapingMessage startMsg = StartScrapingMessage.fromJson("{}");
//        startMsg.setJavaClass("com.deskblast.server.json.StartScrapingMessage");
//        startMsg.setSender(this.rpcBlasterCredentials.getBlasterInfo());
//        startMsg.getSender().setJavaClass(
//                    "com.deskblast.server.json.BlasterInfo");
//        String json = new JSONObject(startMsg).toString();
//        sendJsonMessage(json);
		sendRpcMessage(rpcStartScrapingMessage);
    }
/*
	private void sendStopScrapingMessage() {
        StopScrapingMessage stopMsg = StopScrapingMessage.fromJson("{}");
        stopMsg.setJavaClass("com.deskblast.server.json.StopScrapingMessage");
        stopMsg.setSender(this.blasterCredentials.getBlasterInfo());
        stopMsg.getSender().setJavaClass(
                    "com.deskblast.server.json.BlasterInfo");
        String json = new JSONObject(stopMsg).toString();
        sendJsonMessage(json);
    }
*/    
	public void handleJoin(RpcBlasterCredentials rpcBlasterCredentials) {		
		this.tileDisplay = new TileDisplay(rpcBlasterCredentials);
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
			if(date.getTime() - Long.valueOf(startPending) < 60000){
				handleStartScrapingClicked();
			}
		}
	}
	public void onWindowResized(int width, int height) {
		//horizontalSplitPanel.setSplitPosition("80%");
		//verticalSplitPanel.setSplitPosition("50%");
	}
	
//	private void setHtmlTitle(String title){
//		DOM.getEle
//	}
	
	public static final native void setHtmlTitle(String theTitle) /*-{		
		$wnd.document.getElementsByTagName("HTML")[0].title = theTitle; 
	}-*/;

}
