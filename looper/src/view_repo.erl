-module(view_repo).

-export([add_view/3]).
-export([get_view/1]).
-export([delete_view/1]).

-record(skreen_view, {blaster_id, blast_id, ip}).

add_view(BlasterId, BlastId, Ip) ->
    F = fun() ->
        Existing = mnesia:read(skreen_view, BlasterId, write),
        case Existing of
            [] ->
                lager:info("adding view ~s for ~s ~w", [BlasterId, BlastId, Ip]),
                mnesia:write(#skreen_view{blaster_id = BlasterId, blast_id = BlastId, ip = Ip}),
                ok;
            _ ->
                lager:error("could not add view ~s for ~s", [BlasterId, BlastId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

get_view(BlasterId) ->
    F = fun() ->
        Result = mnesia:read(skreen_view, BlasterId, read),
        case Result of
            [{skreen_view, BlasterId, BlastId, Ip}] ->
                lager:info("got view ~s for ~s", [BlasterId, BlastId]),
                {ok, BlasterId, BlastId, Ip};
            _ ->
                lager:error("could not get view for ~s", [BlasterId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

delete_view(BlasterId) ->
    F = fun() ->
        lager:info("deleting view for ~s", [BlasterId]),
        mnesia:delete({skreen_view, BlasterId})
    end,
    mnesia:activity(transaction, F).
