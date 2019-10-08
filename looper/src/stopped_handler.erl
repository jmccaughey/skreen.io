-module(stopped_handler).

-export([init/2]).

init(Req, Opts) ->
    %% hit master server to mark end date
    %% de-activate the local skreen_state
    lager:debug("stopped_handler"),
    QsVals = cowboy_req:parse_qs(Req),
    {_, BlastId} = lists:keyfind(<<"blastId">>, 1, QsVals),
    %% {_, Server} = lists:keyfind(<<"server">>, 1, QsVals),
    {_, Key} = lists:keyfind(<<"key">>, 1, QsVals),
    lager:info("attempting to stop blast_id: ~s", [BlastId]),
    skreen_repo:close_skreen(binary_to_list(BlastId), Key),
    %%% Response = server_repo:add_server(BlastId, Server),
    %case Response of
    %    ok ->
            Req2 = cowboy_req:reply(200, [], "", Req),
            {ok, Req2, Opts}.
     %   _ ->
     %       Req2 = cowboy_req:reply(409, [], "", Req),
     %       {ok, Req2, Opts}
    %end.    
                
