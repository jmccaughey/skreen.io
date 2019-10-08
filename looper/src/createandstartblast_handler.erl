%% Feel free to use, reuse and abuse the code in this file.

%% @doc Hello world handler.
-module(createandstartblast_handler).

-export([init/2]).

init(Req, Opts) ->
    lager:debug("createandstartblast_handler"),
    Blast = build_new_blast(),
    {ok, Key} = maps:find(<<"key">>, Blast),
    {ok, BlastId} = maps:find(<<"blastId">>, Blast),
    {ok, Id} = maps:find(<<"id">>, Blast),
    {ok, Name} = maps:find(<<"name">>, Blast),
    JsonResponse = io_lib:format("[{\"blastId\":\"~s\", \"id\":\"~s\",\"key\":\"~s\",\"name\":\"~s\"}]~n",
        [BlastId, Id, Key, Name]),    
    {ok, _Body, Req2} = cowboy_req:body(Req),
    lager:info("sending response to createandstart: ~s", [JsonResponse]),
    Req3 = cowboy_req:reply(200, [
		{<<"content-type">>, <<"text/javascript">>}
	], JsonResponse, Req2),
	{ok, Req3, Opts}.    

build_new_blast() ->
    ServerName = os:getenv("SKREEN_IO_SERVER_NAME", "skreen.io"),
    {BlastId, BlasterId, Key}  = loop_register:register_server(ServerName, 6),
    lager:info("got blastid ~s blasterid ~s and key ~s from server ~s", [BlastId, BlasterId, Key, ServerName]),
    #{<<"blastId">> =>  BlastId, <<"id">> => BlasterId, <<"key">> => Key, <<"name">> => BlastId}.

