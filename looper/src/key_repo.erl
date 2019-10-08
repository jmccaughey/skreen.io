-module(key_repo).

-export([add_key/2]).
-export([get_key/1]).
-export([delete_key/1]).

-record(skreen_key, {blast_id, key}).

add_key(BlastId, Key) ->
    F = fun() ->
        Existing = mnesia:read(skreen_key, BlastId, write),
        case Existing of
            [] ->
                lager:info("adding key ~s for ~s", [Key, BlastId]),
                mnesia:write(#skreen_key{blast_id = BlastId, key = Key}),
                ok;
            _ ->
                lager:error("could not add key ~s for ~s", [Key, BlastId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

get_key(BlastId) ->
    F = fun() ->
        Result = mnesia:read(skreen_key, BlastId, read),
        case Result of
            [{skreen_key, BlastId, Key}] ->
                lager:info("got key ~s for ~s", [Key, BlastId]),
                {ok, Key};
            _ ->
                lager:error("could not get key for ~s", [BlastId]),
                error
        end
    end,
    mnesia:activity(transaction, F).

delete_key(BlastId) ->
    F = fun() ->
        lager:info("deleting key for ~s", [BlastId]),
        mnesia:delete({skreen_key, BlastId})
    end,
    mnesia:activity(transaction, F).
