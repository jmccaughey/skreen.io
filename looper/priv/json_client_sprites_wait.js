	
	//var console = console || {"log":function(){}, "info":function(){}};
	
	var credentials;
	var downloading = false;
	var onDeckTiles = null;
	var currentTiles = null;
	//var lastReceived = true;
	var mousePosition = {};
	var context = "/";
	var serial = 0;
	var totalTiles = 0;
	//var currentSubframe = -1;

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
		if(onDeckTiles){
			currentTiles = onDeckTiles;
			onDeckTiles = null;
			downloading = true;
			setTimeout(function(){ doDownload(); },1);
		}		 
	}
	function mergeWith(tiles){
		//console.info("new mergeWith");
		var i;
		for(i = 0; i < onDeck.length; i++){
			var j;
			var found = false;
			for(j = 0; j < tiles.length; j++){
				if(onDeck[i].x == tiles[j].x && onDeck[i].y == tiles[j].y){
					console.info("dropping old tile");
					found = true;
					break;
				}
			}		
			if(!found){
				tiles[tiles.length] = onDeck[i];
				//console.info("carried over");
			}
		}
		onDeck = tiles;
	}
	function buildSrc(name){
		return context + 'skreens/' + blastId + '/key/' + credentials.key + '/images/' + name;
		//+ '&blastId=' + credentials.blastId
		//+ '&blasterId=' + credentials.blasterInfo.id
		//+ '&key=' + credentials.key
	}
	function doDownload(){
		//console.info("doDownload");
		downloading = true;
		var pending = totalTiles;
		var i;		
		for(key in currentTiles){			
			//console.info("building src"); 
			var newSrc = buildSrc(currentTiles[key].h); 
			//console.info("new src is: " + newSrc);
					//context + 'scrapes?name=' + tiles[i].hash 
					//+ '&blastId=' + credentials.blastId
					//+ '&blasterId=' + credentials.blasterInfo.id
					//+ '&key=' + credentials.key;
			preLoadImage(newSrc, function(){
				//console.info("preload done (" + pending + " pending)");
				pending--;
				if(pending == 0){//} && lastReceived){
					//console.info("last image pre-loaded");
					showScrape();
					downloading = false;
					downloadDone();
				}
			});
		}
	}
	function enqueueScrape(rpcScreenScrapeData){	
		mousePosition.x = rpcScreenScrapeData.mX;
		mousePosition.y = rpcScreenScrapeData.mY;
		//console.log("mouse x =" + mousePosition.x);
		serial = rpcScreenScrapeData.serial;
		buildTiles(rpcScreenScrapeData);
		if(downloading){
			if(onDeckTiles){
				console.log("dropping onDeckTiles");
			}
			onDeckTiles = rpcScreenScrapeData.tiles;
		}else{
			currentTiles = rpcScreenScrapeData.tiles;
			doDownload();		
		}
	}
	function showScrape(){
		for(key in currentTiles){
			updateTile(key, currentTiles[key]);
		}
	}

	function buildTiles(rpcScreenScrapeData){
		if(rpcScreenScrapeData.tW && rpcScreenScrapeData.tH && $('desk').childElements().length < 2){
			var xCursor = 0;
			var yCursor = 0;
			while(yCursor < rpcScreenScrapeData.h){
				while(xCursor < rpcScreenScrapeData.w){
					insertTile(xCursor, yCursor, 
						Math.min(rpcScreenScrapeData.tW, rpcScreenScrapeData.w - xCursor), 
						Math.min(rpcScreenScrapeData.tH, rpcScreenScrapeData.h - yCursor));
					totalTiles++;
					xCursor = xCursor + Math.min(rpcScreenScrapeData.tW, rpcScreenScrapeData.w - xCursor);
				}
				xCursor = 0;
				yCursor = yCursor + Math.min(rpcScreenScrapeData.tH, rpcScreenScrapeData.h - yCursor);
			}
		}
	}

	function insertTile(x, y, width, height){
		//alert("insert x:" + x + " y:" + y + " width:" + width + " height:" + height);
		var theid = 'tile_' + x + '_' + y;
		var thestyle = 'position: absolute; left: ' + x + 'px; top: ' + y + 'px; '
			+ ' width: ' + width + 'px; height: ' + height + 'px;';
		var el = new Element('div', {id: theid, style: thestyle, 'class': 'unselectable'});
		$('desk').insert({ bottom: el });
	}

	function updateTile(key, tile){
		var newSrc = buildSrc(tile.h); 
		var theid = 'tile_' + key; 
		var background = "url('" + newSrc + "') -" + tile.sX + "px -" 
			+ tile.sY + "px";
		var element = $(theid);
		element.style.background = background;	
	}
	function handleMessage(message){
		//if(message.javaClass == 'com.deskblast.client.RpcChatMessage'){
		//	alert(message.text);
		//}else if(message.javaClass == 'com.deskblast.client.RpcScrapingScrapeMessage'){
			enqueueScrape(message);
		//}		
	}
	function handleMessages(json){
		var messages = json.evalJSON();		
		//alert(messages);
		//console.log("length: " + messages.length)
		var i;
		for(i = 0; i < messages.length; i++){
			handleMessage(messages[i]);
		}
		//handleMessage(messages);
	}
	var messageNumber = 1;
	var blastId = document.URL.substring(document.URL.length - 6);

	function getMessages(){
		var parms = //blastId
			'?blastId=' + blastId
			//+ '&serialId=' + currentKeyframe
			+ '&serial=' + (serial + 1)
			+ '&key=' + credentials.key
			//+ '&blasterId=' + credentials.blasterInfo.id + '&key=' + credentials.key 
			//+ '&messageNumber=' + messageNumber++ 
			;
		//alert(parms);
		//alert(blastId);
		new Ajax.Request(
				context + 'longpoll' + parms,
				{
					method: 'get',					
					onSuccess: function(response){
						//alert(response);
						//console.log(response.responseText);
						if(response.responseText === "session ended"){
							console.log("got closed message");
							alert("session closed");
						}else{
							handleMessages(response.responseText);
							setTimeout('getMessages()',1);
						}
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
		console.info('joining...');
		new Ajax.Request(
			context + 'join/' + blastId, {
				onSuccess: function(response){
					credentials = response.responseText.evalJSON();	
					getMessages();
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
		//alert(document.URL);
                join();
		//getMessages();
/*
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
			
			$('showButton').observe('click', function(){
				insertScraper();			
			});
			
		}
*/		
	});
