<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<link type="image/x-icon" href="/images/favicon.ico" rel="shortcut icon"/>
<link type="text/css" rel="stylesheet" href="deskblast/Deskblast.css">
<title>shodat</title>
<script type="text/javascript">
	<g:if test="${ hostCredentials }">	
	var hostCredentials = ${hostCredentials};
	</g:if>
	<g:if test="${ guestCredentials }">
	var guestCredentials = ${guestCredentials};
	</g:if>
</script>

<g:if test="${ hostCredentials }">	
	<script type="text/javascript" language="javascript"
	src="deskblast/deskblast/deskblast.nocache.js"></script>
</g:if>
<g:if test="${ guestCredentials }">
	<script type="text/javascript" src="deskblast/prototype.js"></script>
	<script type="text/javascript" language="javascript"
	src="deskblast/json_client_sprites_wait.js"></script>
</g:if>	
</head>
<body style="margin: 0;">
<g:if test="${ guestCredentials }">
	<div id="desk" style="position: absolute; width: 100%; height: 100%;">
  		<img id="mouse_pointer" src="deskblast/images/mouse_pointer.gif" style="display: none; position: absolute; z-index: 2;">
	</div>
</g:if>
</body>
</html>
