%% Feel free to use, reuse and abuse the code in this file.

%% @private
-module(eventsource_app).
-behaviour(application).

%% API.
-export([start/2]).
-export([stop/1]).

%% API.

-record(skreen_instruction, {name, instruction}).
-record(skreen_skreen, {name, state}).
-record(skreen_state, {name, keyframe_id, subframe_id, active}).
-record(skreen_waiter, {blast_id, process_id}).
-record(skreen_key, {blast_id, key}).
-record(skreen_view, {blaster_id, blast_id, ip}).
-record(skreen_server, {blast_id, server, start_date, end_date}).

create_image_dir() ->
    lager:debug("creating image dir"),
    filelib:ensure_dir("/tmp/skreen_images/").

install_table(Nodes) ->
    %DeleteResult = mnesia:delete_schema(Nodes),
    %case DeleteResult of
    %    ok ->
    %        lager:info("schema deleted")
    %end,
    CreateResult = mnesia:create_schema(Nodes),
    case CreateResult of
        ok ->
            lager:info("schema created");
        {error, {_Schema, {already_exists, _}}} ->
            lager:info("schema already exists") 
    end,
    MakeDiscResult = mnesia:change_table_copy_type(schema, node(), disc_copies),
    case MakeDiscResult of
        {atomic, ok} ->
            lager:info("schema changed to disc");
        {aborted, {already_exists, _, _, _}} ->
            lager:info("schema already on disc")
    end,    
 
    mnesia:create_table(skreen_state,
                        [{attributes, record_info(fields, skreen_state)},
                         {type, set},
                         {ram_copies, Nodes}]),

    mnesia:create_table(skreen_instruction,
                        [{attributes, record_info(fields, skreen_instruction)},
                         {type, set},
                         {ram_copies, Nodes}]),

    mnesia:create_table(skreen_waiter,
                        [{attributes, record_info(fields, skreen_waiter)},
                         {type, bag},
                         {ram_copies, Nodes}]),

    mnesia:create_table(skreen_key,
                        [{attributes, record_info(fields, skreen_key)},
                         {type, bag},
                         {ram_copies, Nodes}]),

    mnesia:create_table(skreen_view,
                        [{attributes, record_info(fields, skreen_view)},
                         {type, set},
                         {ram_copies, Nodes}]),

    mnesia:create_table(skreen_skreen,
                        [{attributes, record_info(fields, skreen_skreen)},
                         {type, set},
                         {ram_copies, Nodes}]),

    lager:info("about to create server table"),
    Result = mnesia:create_table(skreen_server,
                        [{attributes, record_info(fields, skreen_server)},
                         {type, set},
                         {disc_copies, Nodes}]),
    case Result of
        {aborted, {already_exists,skreen_server}} ->
            lager:info("skreen_server table already exists"),
            mnesia:change_table_copy_type(skreen_server, node(), disc_copies);
        {atomic, ok} ->
            lager:info("skreen_server table created")
            %mnesia:change_table_copy_type(skreen_server, Nodes, disc_copies)
    end.
    %mnesia:change_table_copy_type(skreen_server, Nodes, disc_copies),
    
start(_Type, _Args) ->
    ListenIP = os:getenv("SKREEN_IO_LISTEN_IP", "0.0.0.0"),
    {ok, ListenIPTuple} = inet_parse:address(ListenIP),
    
	ListenPort = list_to_integer(os:getenv("SKREEN_IO_LISTEN_PORT", "8080")),
	Dispatch = cowboy_router:compile([
		{'_', [
			{"/scrapes", scrapes_handler, []},
            {"/stoppedShowing", stopped_handler, []},
			{"/join/:skreen", join_handler, []},
			{"/eventsource_handlertsource", eventsource_handler, []},
			{"/createAndStartBlast", createandstartblast_handler, []},
			{"/skreens/:skreen/key/:key/images/:image", images_handler, []},
            {"/listServers", listservers_handler, []},
            {"/registerServer", registerserver_handler, []},
            {"/locateServer", locateserver_handler, []},
			%{"/images/[...]", cowboy_static, {dir, "/Users/jmccaughey/skreen_images/", 
			%			[{mimetypes, {<<"image">>, <<"jpeg">>, []}}]}}, %images_handler, []},
			{"/longpoll/[...]", poll_handler, []},
			{"/jquery.min.js", cowboy_static, {priv_file, eventsource, "jquery.min.js"}},
			{"/", cowboy_static, {priv_file, eventsource, "index.html"}},
			{"/prototype.js", cowboy_static, {priv_file, eventsource, "prototype.js"}},
			{"/mouse_pointer.gif", cowboy_static, {priv_file, eventsource, "mouse_pointer.gif"}},
			{"/json_client_sprites_wait.js", cowboy_static, {priv_file, eventsource, "json_client_sprites_wait.js"}},
            {"/index.html", cowboy_static, {priv_file, eventsource, "index.html"}},
            {"/windows/skreen.io.exe", cowboy_static, {priv_file, eventsource, "windows/skreen.io.exe"}},
            {"/mac/skreen.io.zip", cowboy_static, {priv_file, eventsource, "mac/skreen.io.zip"}},
            {"/favicon.ico", cowboy_static, {priv_file, eventsource, "favicon.ico"}},
			{"/[...]", client_handler, {priv_file, eventsource, "client.html"}}
		]}
	]),
    %application:set_env(mnesia, dir, "/home/john"),
    %DeleteResult = mnesia:delete_schema([node()]),
    %case DeleteResult of
    %    ok ->
    %        lager:info("schema deleted")
    %end,

	application:load(mnesia),
	application:start(mnesia),
    application:load(inets),
    application:start(inets),
    application:load(lager),
    application:start(lager),
    create_image_dir(),
     %application:set_env(mnesia, dir, "/home/john"),
	install_table([node()]),
	{ok, _} = cowboy:start_http(http, 100, [{port, ListenPort}
           , {ip, ListenIPTuple}  % {172,30,3,253}
            ], [
		{env, [{dispatch, Dispatch}]},
        {max_keepalive, 1000},
        {timeout, 30000} % how long keep alive keeps alive, in milliseconds
	]),
	eventsource_sup:start_link().

stop(_State) ->
	ok.

