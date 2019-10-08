-module(locateserver_handler).

-export([init/2]).

init(Req, []) ->
    QsVals = cowboy_req:parse_qs(Req),
    {_, BlastId} = lists:keyfind(<<"blast_id">>, 1, QsVals),
    lager:info("locateserver blast_id: ~s", [BlastId]),
    Response = server_repo:get_server(BlastId),
    case Response of
        {ok, Server} ->
            lager:info("server is: ~s", [Server]),
            Req2 = cowboy_req:reply(200, [], Server, Req),
            {ok, Req2, []};
        _ ->
            lager:error("server not found", []),
            Req2 = cowboy_req:reply(404, [], "", Req),
            {ok, Req2, []}
    end. 
                
