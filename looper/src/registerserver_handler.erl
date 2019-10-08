-module(registerserver_handler).

-export([init/2]).

init(Req, Opts) ->
    lager:debug("registerserver_handler"),
    QsVals = cowboy_req:parse_qs(Req),
    {_, BlastId} = lists:keyfind(<<"blast_id">>, 1, QsVals),
    {_, Server} = lists:keyfind(<<"server">>, 1, QsVals),
    %{_, Key} = lists:keyfind(<<"key">>, 1, QsVals),
    lager:info("attempting to register blast_id: ~s server: ~s", [BlastId, Server]),
    Response = server_repo:add_server(BlastId, Server),
    case Response of
        ok ->
            Req2 = cowboy_req:reply(200, [], "", Req),
            {ok, Req2, Opts};
        _ ->
            Req2 = cowboy_req:reply(409, [], "", Req),
            {ok, Req2, Opts}
    end.    
                
