-module(server_repo).

-export([add_server/2]).
-export([get_server/1]).
-export([stop_server/1]).

-record(skreen_server, {blast_id, server, start_date, end_date}).

add_server(BlastId, Server) ->
    F = fun() ->
        Existing = mnesia:read(skreen_server, BlastId, write),
        case Existing of
            [] ->
		        Date = calendar:local_time(),
                lager:info("adding server ~s for ~s at ~w", [Server, BlastId, Date]),
                mnesia:write(#skreen_server{blast_id = BlastId, server = Server, start_date = Date}),
                ok;
            _ ->
                lager:error("could not add server ~s for ~s (already present)", [Server, BlastId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

get_server(BlastId) ->
    F = fun() ->
        Result = mnesia:read(skreen_server, BlastId, read),
        case Result of
            [{skreen_server, BlastId, Server, Date, _Date}] ->
                lager:info("got server ~s for ~s created ~w", [Server, BlastId, Date]),
                {ok, Server};
            _ ->
                lager:error("could not get server for ~s", [BlastId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

stop_server(BlastId) ->
    F = fun() ->
        lager:info("stopping server for ~s", [BlastId]),
        Date = calendar:local_time(),
        [Existing] = mnesia:wread({skreen_server, BlastId}),
        New = Existing#skreen_server{end_date = Date},
        mnesia:write(New),
        lager:info("stopped server for ~s", [BlastId])
    end,
    mnesia:activity(transaction, F).
