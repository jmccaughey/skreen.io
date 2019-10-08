<html>
    <head>
        <title><g:layoutTitle default="Shodat" /></title>
        <link rel="stylesheet" href="${resource(dir:'css',file:'main.css')}" />
        <link rel="shortcut icon" href="${resource(dir:'images',file:'favicon.ico')}" type="image/x-icon" />
        <g:layoutHead />
        <g:javascript library="application" />
		<g:javascript library='scriptaculous' />
    </head>
    <body>
    	<g:render template='/includes/ajaxLogin'/>
        <div id="spinner" class="spinner" style="display:none;">
            <img src="${resource(dir:'images',file:'spinner.gif')}" alt="Spinner" />
        </div>
        <div id="deskblastLogo" class="deskblastLogo">
        	<div style="float: left;">
        	<a href="${createLinkTo(dir:'/')}"><img src="${resource(dir:'images',file:'deskblast_logo.gif')}" alt="shodat" border="0" /><span style="font-size: 70pt; text-decoration: none;">shodat</span><!--<img src="${resource(dir:'images',file:'deskblast_text.gif')}" alt="deskblast" border="0" />--></a>
        	</div>
        	<div style="float:right;">
        	<span id='loginLink' style='margin-right: 30px; float: right;'>
   				<g:isLoggedIn>
   				Hi <g:loggedInUsername/> (<g:link controller='logout'>Logout</g:link>)
   				| <a href="<g:loggedInUsername/>">Shodat now!</a>
   				</g:isLoggedIn>
   				<g:isNotLoggedIn>
   				<g:link controller="login">Sign in</g:link>
   				</g:isNotLoggedIn>
   			</span>
   			</div>
        </div>
        <div style="clear: both;"><!----></div>
        <g:layoutBody />
    </body>
</html>