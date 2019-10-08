<html>
    <head>
        <title>Shodat</title>
		<meta name="layout" content="main" />
    </head>
    <body>
		<div id="nav">
			<div class="homePagePanel">
				<div class="panelTop">

				</div>
				
				<div class="panelBody">
					<h1>About Shodat</h1>
					<ul>
						<li><g:link controller="requirements">Requirements</g:link></li>
						<li>Features</li>
						<li>Cost/Pricing</li>
						<li>Help</li>
						<li>About Us</li>
						<li>Privacy</li>
						<li>Trademark</li>
					</ul>
				</div>
				<div class="panelBtm">
				</div>
			</div>
		</div>
		<div id="pageBody">
			<g:if test="${flash.message}">
				<div class="message">${flash.message}</div>
			</g:if>
	        <h1>Welcome to Shodat</h1>
	        <p>Congratulations, you've found the best way to show them! In a matter of seconds, show 
	        your screen over the internet. <g:isNotLoggedIn><g:link controller="register">Sign up now!</g:link>
	        Already have an account? <g:link controller="login">Sign in here</g:link></g:isNotLoggedIn>
	        <g:isLoggedIn><a href="<g:loggedInUsername/>">Shodat now!</a></g:isLoggedIn></p>

	        <div id="controllerList" class="dialog">
				<h2>The Shodat Advantage:</h2>
	            <ul>
			        <li class="controller">Show your screen with about three clicks</li>
			        <li class="controller">To invite people, send this link: 
			        <b>shod.at/yourname</b> -- that's it! Bookmark it!</li>
	            </ul>
	        </div>
		</div>
    </body>
</html>
