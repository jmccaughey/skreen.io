	
	//var console = console || {"log":function(){}, "info":function(){}};
	
	var credentials;
	var scrapeNumber = 0;
	var downloading = false;
	var onDeck = new Array();
	var lastReceived = true;
	var mousePosition = {};
	var context = "deskblast/";

	// https://gist.github.com/adunkman/2371101
	(function () {
	   // Detect if ClickOnce is supported by the browser. 
	   // Roughly based on http://stackoverflow.com/a/4653175/117402
	   var hasMimeSupport = function (desiredMime) {
	      var mimes = window.navigator.mimeTypes,
	          hasSupport = false;
	      for (var i = 0; i < mimes.length; i++) {
	         var mime = mimes[i];
	         if (mime.type == desiredMime) {
	            hasSupport = true;
	         }
	      }
	      return hasSupport;
	   };
	 
	   var sniffForClickOnce = function () {
	      var userAgent = window.navigator.userAgent.toUpperCase(),
	          hasNativeDotNet = userAgent.indexOf('.NET CLR') >= 0;
	      return hasNativeDotNet || hasMimeSupport("application/x-ms-application");
	   };
	   window.clickOnce = sniffForClickOnce();
	})();
	
	function preLoadImage(src, callback){
		var image = new Image();
		image.onload = function(){
			callback();
			image = null;
		};		
		image.src = src;
	}
	function showMouse(){
		$('mouse_pointer').show();
		$('mouse_pointer').style.left = mousePosition.x + 'px';
		$('mouse_pointer').style.top = mousePosition.y + 'px';
	}
	function downloadDone(){	  
		// start download if anything on deck
		showMouse();
		if(onDeck.length > 0){
			var copy = new Array();
			var i;
			for(i = 0; i < onDeck.length; i++){
				copy[i] = onDeck[i];
			}		
			lastReceived = true;
			onDeck.clear();
			downloading = true;
			setTimeout(function(){ doDownload(copy); },1);
		}		 
	}
	function mergeWith(rects){
		console.info("new mergeWith");
		var i;
		for(i = 0; i < onDeck.length; i++){
			var j;
			var found = false;
			for(j = 0; j < rects.length; j++){
				if(onDeck[i].x == rects[j].x && onDeck[i].y == rects[j].y){
					console.info("dropping old tile");
					found = true;
					break;
				}
			}		
			if(!found){
				rects[rects.length] = onDeck[i];
				console.info("carried over");
			}
		}
		onDeck = rects;
	}
	function buildSrc(name){
		return context + 'scrapes?name=' + name 
		+ '&blastId=' + credentials.blastId
		+ '&blasterId=' + credentials.blasterInfo.id
		+ '&key=' + credentials.key
	}
	function doDownload(rects){
		if(rects.length == 0){
			downloading = false;
			downloadDone();
			return;
		}
		downloading = true;
		console.info("doDownload");
		var pending = rects.length;
		var i;		
		for(i = 0; i < rects.length; i++){			
			var newSrc = buildSrc(rects[i].hash); 
				//context + 'scrapes?name=' + rects[i].hash 
					//+ '&blastId=' + credentials.blastId
					//+ '&blasterId=' + credentials.blasterInfo.id
					//+ '&key=' + credentials.key;
			preLoadImage(newSrc, function(){
				pending--;
				if(pending == 0 && lastReceived){
					showScrape(rects);
					downloading = false;
					downloadDone();
				}
			});
		}
	}
	function enqueueScrape(rpcScreenScrapeData, scrapeNumber){	
		//showMouse(rpcScreenScrapeData.mouseX, rpcScreenScrapeData.mouseY);
		mousePosition.x = rpcScreenScrapeData.mouseX;
		mousePosition.y = rpcScreenScrapeData.mouseY;
		if(downloading){
			if(onDeck.length == 0){
				onDeck = rpcScreenScrapeData.rects;
			}else{
				mergeWith(rpcScreenScrapeData.rects);
			}
		}else{
			if(rpcScreenScrapeData.javaClass.indexOf('Intermediate') > -1){
				console.log("got intermediate scrape");
				//lastReceived = false;
			}else{
				lastReceived = true;
			}
			doDownload(rpcScreenScrapeData.rects);		
		}
	}
	function showScrape(rects){
		for(i = 0; i < rects.length; i++){
			addOrUpdateTile(rects[i]);
		}
	}
	function addOrUpdateTile(tile){
		var newSrc = buildSrc(tile.hash); 
		var theid = 'tile_' + tile.x + '_' + tile.y; 
		if($(theid)){
		}else{
			var thestyle = 'position: absolute; left: ' + tile.x + 'px; top: ' + tile.y + 'px; '
			+ ' width: ' + tile.width + 'px; height: ' + tile.height + 'px;';
			var el = new Element('div', {id: theid, style: thestyle, 'class': 'unselectable'});
			$('desk').insert(el);
		}
		var background = "url('" + newSrc + "') -" + tile.spriteX + "px -" 
			+ tile.spriteY + "px";
		var element = $(theid);
		element.style.background = background;	
	}
	function handleMessage(message){
		if(message.javaClass == 'com.deskblast.client.RpcChatMessage'){
			alert(message.text);
		}else if(message.javaClass == 'com.deskblast.client.RpcScrapingScrapeMessage'){
			enqueueScrape(message.rpcScreenScrapeData, scrapeNumber++);
		}		
	}
	function handleMessages(json){
		var messages = json.evalJSON();
		//alert(messages);
		var i;
		for(i = 0; i < messages.length; i++){
			handleMessage(messages[i]);
		}
	}
	var messageNumber = 1;
	function getMessages(){
		var parms = '?blastId=' + credentials.blastId + '&blasterId=' + credentials.blasterInfo.id + '&key=' + credentials.key 
			+ '&messageNumber=' + messageNumber++ ;
		//alert(parms);
		new Ajax.Request(
				context + 'messages' + parms,
				{
					method: 'get',					
					onSuccess: function(response){
						//alert(response);
						//console.log(response.responseText);
						handleMessages(response.responseText);
						setTimeout('getMessages()',1);						
					},
					onFailure: function(response){
						console.log('failure getting messages: ' + response);
						//alert('failure getting messages: ' + response);
					}
				}
		);
	}
	function hideOtherContent(){
		$('desk').siblings().each(function(item){ 
			if(item.id != desk) item.hide();
		});
	}
	function join(){
		//alert('joining...');
		new Ajax.Request(
			context + 'joinBlast',
			{
				postBody: Object.toJSON(credentials),
				onSuccess: function(response){
					if($('join_inputs')){
						$('join_inputs').hide();
					}
					hideOtherContent();
					getMessages();
					$('desk').show();
				}
			}
		);
	}
	function startBlast(){
		//alert("in startBlast");
		//alert(credentials.blasterInfo.name);
		$('joinInfo').hide();
		$('joinInfo').update("Screen show starting. The viewer code is: " + credentials.blasterInfo.name);
		//alert("starting blast named: " + credentials.blasterInfo.name);
		new Ajax.Request(
				context + 'startBlast', 
				{
					postBody: Object.toJSON(credentials),
					onSuccess: function(response){
						//alert("hi" + reponse);
						$('startButton').observe('click', function(){
							//alert('clicked');
							insertScraper();			
						});
						//insertScraper();
					},
					onError: function(response){						
						alert(response);
					}
				}
		);
	}
	function insertScraper(){
		//alert("insertScraper");
		var port = "";
		var ssl = true;
		if(window.location.hostname.length != window.location.host){
			port = window.location.port;
		}
		if(window.location.protocol == 'https:'){
			ssl = true;
		}else{
			ssl = false;
		}
		if(clickOnce){
			buildClickOnceUrl(port, ssl);
		}else if(Prototype.Browser.IE){
			insertScraperActiveX(port, ssl);
		}else{
			insertScraperPlugin(port, ssl);
		}
		$('joinInfo').show();
	}	
	function insertScraperActiveX(port, ssl){
		var content = '<OBJECT id="scraper" name="shodat" WIDTH=1 HEIGHT=1 ' 
			+ ' CLASSID="CLSID:CF11AE46-27B0-475D-8187-133D1D47BFA4" '
			+ ' CODEBASE="' + '' + 'shodat-signed.cab#Version=-1,-1,-1,-1">'
			+ ' <PARAM name="blastId" value="' + credentials.blastId + '">'
			+ ' <PARAM name="blasterId" value="' + credentials.blasterInfo.id + '">'
			+ ' <PARAM name="key" value="' + credentials.key + '">'
			+ ' <param name="context" value="' + context + '">'
			// TODO: discover host & port from setsite
			+ ' <PARAM name="host" value="' + window.location.hostname + '">'
			+ ' <PARAM name="port" value="' + port + '">'
			+ '</OBJECT>';			  
		//alert(content);
		$('scraper-container').update(content);
	}
	function insertScraperPlugin(port, ssl){		
		var content = '<EMBED id="scraper" name="shodat" WIDTH=1 HEIGHT=1 ' 
		+ ' type="application/x-shodat-plugin" '
		+ ' blastId="' + credentials.blastId + '"'
		+ ' blasterId="' + credentials.blasterInfo.id + '"'
		+ ' key="' + credentials.key + '"'
		+ ' host="' + window.location.hostname + '"'
		+ ' port="' + port + '"'
		+ ' context="' + context + '"'
		+ ' ssl="' + ssl + '"'
		+ '/>';
		//alert(content);
		$('scraper-container').update(content);
	}
	function buildClickOnceUrl(port, ssl){
		var clickOnceUrl = window.location.protocol + "//" 
			+ window.location.hostname
			+ ":" + port
			+ "/" + context
			+ "shodat_launcher.application?"
			+ "host=" + window.location.hostname
			+ "&port=" + port
			+ "&context=" + context
			+ "&blastId=" + credentials.blastId
			+ "&blasterId=" + credentials.blasterInfo.id
			+ "&key=" + credentials.key;	
		//alert("navigating to:" + clickOnceUrl);
		window.location.href = clickOnceUrl; 
		//$('scraper-container').update(content);
	}
	function createBlast(){
		//alert("creating blast");
		new Ajax.Request(
				context + 'createBlast', 
				{
					method: 'get', 
					onSuccess: function(response){ 
						credentials = response.responseText.evalJSON();
						startBlast();
					}
				}
		);
	}
	function createBlaster(owner, joiner){	
		//alert("creating blaster " + owner);
		new Ajax.Request(
				context + 'createBlaster?ownerName=' + owner + '&joinerName=' + joiner, 
						{
							method: 'get', 
							onSuccess: function(response){
								//alert(response.responseText);
								credentials = response.responseText.evalJSON();
								join();
							}
						}
		);		
	}
	Event.observe(window, 'load', function(){
		//alert('hi'); window['varname'] != undefined
		context = "";
		if(window['guestCredentials'] != undefined){
			credentials = guestCredentials;
			join();			
		}else if(window['joinCode'] != undefined){
			var joinCode = window['joinCode'];
			//alert("joinCode: " + joinCode);
			createBlaster(joinCode, 'random');
		}else{
			// IE blocks setting window.location after ajax. see below. So create a speculative
			// blast and attach its credentials
			// http://stackoverflow.com/questions/5299076/window-location-doesnt-work-after-an-ajax-request-in-internet-explorer			
			$('join_inputs').show();
			$('joinButton').observe('click', function(){
				//alert('clicked');
				createBlaster($('code').getValue(), 'random');			
			});
			//$('startButton').observe('click', function(){
				//alert('clicked');
				createBlast();			
			//});
			/*
			$('showButton').observe('click', function(){
				insertScraper();			
			});
			*/
		}
	});
