<html>
    <head>
        <title>Requirements - Shodat</title>
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
						<li>Requirements</li>
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
	        <h1>Requirements</h1>
	        <p>To view your desktop, participants just need a web browser -- there is nothing to 
	        download or install. Shodat is tested with all the major browsers. To show your desktop, 
	        a tiny plugin is needed, which is compatible with Firefox, Internet Explorer, Safari and 
	        Chrome, on Windows and Mac OSX. <g:isNotLoggedIn><g:link controller="register">Sign up now!</g:link>
	        Already have an account? <g:link controller="login">Sign in here</g:link></g:isNotLoggedIn>
	        <g:isLoggedIn><a href="<g:loggedInUsername/>">Shodat now!</a></g:isLoggedIn></p>

	        
		</div>
    </body>
</html>
