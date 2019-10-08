-module(skreen_repo).

%-export([keyframe/2]).
-export([close_skreen/2]).
-export([update/2]).
-export([get_update/3]).

-record(skreen_skreen, {name, state}).
-record(skreen_waiter, {blast_id, process_id}).

close_skreen(BlastId, _Key) ->
    lager:info("in close_skreen ~s", [BlastId]),
    F = fun() ->
        [#skreen_skreen{name = _FoundName, state = State}] = mnesia:read(skreen_skreen, BlastId, write),
        New = maps:put(active, false, State),
        mnesia:write(#skreen_skreen{name = BlastId, state = New}),
        alert_waiters(BlastId),
        lager:info("in close_skreen returned from alert_waiters")
    end,
    mnesia:activity(transaction, F).

update(BlastId, Message) ->
    F = fun() ->
        lager:debug("update received"),
        Old = mnesia:read(skreen_skreen, BlastId, write),
        case Old of
            [#skreen_skreen{name=_FoundName, state=State}] -> 
                % update collection of tiles with new ones
                lager:debug("found existing state"),
                MergedTiles = merge_tiles(maps:get(tiles, State), maps:get(tiles, Message)),
                Serial = maps:get(serial, State) + 1,
                MouseX = maps:get(mX, Message),
                MouseY = maps:get(mY, Message),
                NewState = #{tiles => MergedTiles, serial => Serial, mX => MouseX, mY => MouseY},
                UpdatedState = maps:merge(State, NewState),
                mnesia:write(#skreen_skreen{name = BlastId, state = UpdatedState});        
            [] ->
                % no prior state
                lager:debug("no state found; writing..."),
                mnesia:write(#skreen_skreen{name = BlastId, state = #{ serial => 0, active => true, w => maps:get(w, Message), h => maps:get(h, Message), 
                        tW => 256, tH => 180, mX => maps:get(mX, Message), mY => maps:get(mY, Message),
                        tiles => maps:get(tiles, Message)}}),
                lager:debug("...done writing")
        end,                
        alert_waiters(BlastId)
    end,
    mnesia:activity(transaction, F).
    % TODO: move alert_waiters here?

merge_tiles(Original, New) ->
    maps:merge(Original, New).

alert_waiters(BlastId) ->
    Result = mnesia:read(skreen_waiter, BlastId, write),
    case Result of 
        [] ->
            lager:debug("no waiters found");
        _ ->
            mnesia:delete({skreen_waiter, BlastId}),
            lists:foreach(fun(#skreen_waiter{blast_id = _BlastId, process_id = Pid}) -> Pid ! update end, Result)
    end.

get_update(BlastId, Serial, ProcessId) ->
    F = fun() ->
        lager:debug("in get_update"),
        Existing = mnesia:read(skreen_skreen, BlastId, read),
        case Existing of 
            [#skreen_skreen{name=_FoundName, state=State}] ->
                FoundSerial = maps:get(serial, State),
                lager:debug("requested serial: ~w current: ~w ", [Serial, FoundSerial]),
                case {maps:get(active, State), Serial > FoundSerial} of
                    {true, true} -> 
                        lager:debug("no update available -- writing to skreen_waiter"),
                        mnesia:write(#skreen_waiter{blast_id = BlastId, process_id = ProcessId}),
                        [];
                    {true, false} -> 
                        lager:debug("serving full"),
                        State;
                    {false, _} ->
                        lager:info("returning 'closed' ~s", [BlastId]),
                        skreen_closed
                end;
            [] ->
                lager:error("get_update but no state found")
        end
    end,    
    mnesia:activity(transaction, F).
