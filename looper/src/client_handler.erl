-module(client_handler).

-export([init/2]).
-export([terminate/3]).

%init(_Transport, Req, []) ->
 %       {ok, Req, undefined}.

terminate(_Reason, _Req, _State) ->
        ok.

init(Req, _Opts) ->
    PathInfo = cowboy_req:path_info(Req),
    Path = binary_to_list(lists:max(PathInfo)),
    lager:debug("client_handler: ~s", [Path]),
    ThisServer = os:getenv("SKREEN_IO_SERVER_NAME", "skreen.io"),
    Result = lookup_server(Path),
    case Result of
        {ok, ThisServer} ->
            lager:debug("server is this server"),
            serve_html(Req, _Opts);
        {ok, Server} ->
            Location = io_lib:format("http://~s/~s", [Server, Path]),
            lager:debug("server is ~s", [Location]),
            Req2 = cowboy_req:reply(307, [{<<"location">>, Location}], Server, Req),
            {ok, Req2, []};
        _ ->
            lager:error("error looking up server"),
            serve_html(Req, _Opts)
    end.

serve_html(Req, _Opts) ->
    Html = io_lib:format("<html>
<head>
<title>skreen.io</title>
<script> var bogoVar = 42; </script>
<script type=\"text/javascript\" src=\"prototype.js\"></script>
<script type=\"text/javascript\" src=\"json_client_sprites_wait.js\"></script>
</head>
<body>
<div id=\"desk\"><img id=\"mouse_pointer\" src=\"mouse_pointer.gif\"
                        style=\"display: none; position: absolute; z-index: 2; opacity:.8;\"></div>
</body>
</html>", []),
    Req2 = cowboy_req:reply(200, [
        {<<"content-type">>, <<"text/html">>}
    ], Html, Req),
    {ok, Req2, []}.

lookup_server(BlastId) ->    
    RegisterServer = os:getenv("SKREEN_IO_REGISTER_SERVER", "skreen.io"),
    Query = 
        io_lib:format("http://~s/locateServer?blast_id=~s", 
        [
        RegisterServer,
         BlastId
        ])
        ,
    lager:debug("register server url: ~s", [Query]),
    Result = httpc:request(lists:flatten(Query)),
    case Result of
        {ok, {{_Version, 200, _ReasonPhrase}, _Headers, Server}} ->
            {ok, Server};
        {ok, {{_Version, 409, _ReasonPhrase}, _Headers, _Body}} ->
            {error};
        _ ->
            {error}
    end.
    
