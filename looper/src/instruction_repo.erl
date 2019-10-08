-module(instruction_repo).

-export([keyframe/2]).
-export([close_skreen/2]).
-export([update/2]).
-export([get_update/4]).

-record(skreen_instruction, {name, instruction}).
-record(skreen_state, {name, keyframe_id, subframe_id, active}).
-record(skreen_waiter, {blast_id, process_id}).

keyframe(BlastId, Message) ->
    % get the state. Increment keyframe #, set subframe # to 0. store it.
    % write message to instruction store
    F = fun() ->
        State = mnesia:read(skreen_state, BlastId, write),
        case State of
            [#skreen_state{name=FoundName, keyframe_id=KeyframeId}] ->
                lager:debug("keyframe received"),
                NewKeyframeId = KeyframeId + 1,
                mnesia:write(#skreen_state{name = FoundName, keyframe_id = NewKeyframeId, subframe_id = 0, active = true}),
                Name = erlang:iolist_to_binary([BlastId, <<":">>, integer_to_list(NewKeyframeId), <<":0">>]), 
                mnesia:write(#skreen_instruction{name = Name, instruction = Message});
                % TODO: delete old instructions
            [] ->
                lager:debug("first keyframe received"),
                mnesia:write(#skreen_state{name = BlastId, keyframe_id = 0, subframe_id = 0, active = true}),
                Name = erlang:iolist_to_binary([BlastId, <<":0:0">>]), 
                mnesia:write(#skreen_instruction{name = Name, instruction = Message}) 
        end,
        alert_waiters(BlastId)
    end,
    mnesia:activity(transaction, F).

close_skreen(BlastId, _Key) ->
    lager:info("in close_skreen ~s", [BlastId]),
    F = fun() ->
        [State] = mnesia:read(skreen_state, BlastId, write),
        New = State#skreen_state{active = false},
        mnesia:write(New),
        alert_waiters(BlastId),
        lager:info("in close_skreen returned from alert_waiters")
    end,
    mnesia:activity(transaction, F).

update(BlastId, Message) ->
    % get the state. Increment subframe #. store it.
    % write message to instruction store
    F = fun() ->
        State = mnesia:read(skreen_state, BlastId, write),
        case State of
            [#skreen_state{name=FoundName, keyframe_id=KeyframeId, subframe_id=SubframeId}] ->
                lager:debug("update received"),
                NewSubframeId = SubframeId + 1,
                mnesia:write(#skreen_state{name = FoundName, keyframe_id = KeyframeId, subframe_id = NewSubframeId, active = true}),
                Name = erlang:iolist_to_binary([BlastId, <<":">>, integer_to_list(KeyframeId), <<":">>, 
                    integer_to_list(NewSubframeId)]), 
                mnesia:write(#skreen_instruction{name = Name, instruction = Message}),
                alert_waiters(BlastId);
                % TODO: delete old instructions
            [] ->
                lager:error("update received but no state found ~w", [BlastId])
        end
    end,
    mnesia:activity(transaction, F).

alert_waiters(BlastId) ->
    Result = mnesia:read(skreen_waiter, BlastId, write),
    case Result of 
        [] ->
            lager:debug("no waiters found");
        _ ->
            mnesia:delete({skreen_waiter, BlastId}),
            lists:foreach(fun(#skreen_waiter{blast_id = _BlastId, process_id = Pid}) -> Pid ! update end, Result)
    end.

get_update(BlastId, RequestedKeyframeId, RequestedSubframeId, ProcessId) ->
    % if keyframe requested does not match keyframe of current state, send full state
    % else if SubframeId is less than or equal to current state, send SubframeId - currentSubframe
    % else there is no update
    F = fun() ->
        State = mnesia:read(skreen_state, BlastId, read),
        case State of 
            [#skreen_state{keyframe_id=KeyframeId, subframe_id=SubframeId, active=Active}] ->
                lager:debug("requested keyframe: ~w current: ~w requested subframe: ~w current: ~w", 
                        [RequestedKeyframeId, KeyframeId, RequestedSubframeId, SubframeId]),
                case {Active, KeyframeId =:= RequestedKeyframeId, RequestedSubframeId =< SubframeId} of
                    {true, true, true} -> 
                        lager:debug("serving partial"),
                        get_instructions_since(BlastId, KeyframeId, RequestedSubframeId, SubframeId, []);
                    {true, true, false} -> 
                        lager:debug("no update available -- writing to skreen_waiter"),
                        mnesia:write(#skreen_waiter{blast_id = BlastId, process_id = ProcessId}),
                        [];
                    {true, false, _} -> 
                        lager:debug("serving full"),
                        get_full_state(BlastId);
                    {false, _, _} ->
                        lager:info("returning 'closed' ~s", [BlastId]),
                        skreen_closed
                end;
            [] ->
                lager:error("get_update but no state found")
        end
    end,    
    mnesia:activity(transaction, F).

get_full_state(BlastId) ->
    % build array of messages, return {KeyframeId, SubframeId, Messages}
    F = fun() ->
        State = mnesia:read(skreen_state, BlastId, read),
        case State of
            [#skreen_state{keyframe_id=KeyframeId, subframe_id=SubframeId}] ->
                lager:debug("full state requested"),
                get_instructions(BlastId, KeyframeId, SubframeId, []);
            [] ->
                lager:error("get_full_state but no state found")
        end
    end,
    mnesia:activity(transaction, F).

get_instructions(BlastId, KeyframeId, 0, Instructions) ->
    lager:debug("getting instructions..."),
    Name = erlang:iolist_to_binary([BlastId, <<":">>, integer_to_list(KeyframeId), <<":0">>]),    
    I = mnesia:read(skreen_instruction, Name, read),
    case I of
        [#skreen_instruction{instruction=Instruction}] ->
            Instruction1 = maps:put(<<"keyframeId">>, KeyframeId, Instruction),
            Instruction2 = maps:put(<<"subframeId">>, 0, Instruction1),
            Instruction3 = maps:put(<<"tileWidth">>, 256, Instruction2),
            Instruction4 = maps:put(<<"tileHeight">>, 180, Instruction3),
            [Instruction4 | Instructions];
        [] ->
            lager:error("keyframe not found")
    end;
get_instructions(BlastId, KeyframeId, SubframeId, Instructions) ->
    lager:debug("getting instructions..."),
    Name = erlang:iolist_to_binary([BlastId, <<":">>, integer_to_list(KeyframeId), <<":">>, 
            integer_to_list(SubframeId)]),
    I = mnesia:read(skreen_instruction, Name),
    case I of
        [#skreen_instruction{instruction=Instruction}] ->
            Instruction1 = maps:put(<<"keyframeId">>, KeyframeId, Instruction),
            Instruction2 = maps:put(<<"subframeId">>, SubframeId, Instruction1),
            get_instructions(BlastId, KeyframeId, SubframeId - 1, [Instruction2 | Instructions]);
        [] ->
            lager:error("instruction not found")
    end.

get_instructions_since(BlastId, KeyframeId, SubframeId, CurrentSubframeId, Instructions) ->
    case CurrentSubframeId < SubframeId of
        true ->
            lager:debug("returning partial current: ~w sub: ~w", [CurrentSubframeId, SubframeId]),
            Instructions;
        _ ->
            Name = erlang:iolist_to_binary([BlastId, <<":">>, integer_to_list(KeyframeId), <<":">>, 
                integer_to_list(CurrentSubframeId)]),
            [#skreen_instruction{name = _Name, instruction = Instruction}] 
                    = mnesia:read(skreen_instruction, Name, read),
            Instruction1 = maps:put(<<"keyframeId">>, KeyframeId, Instruction),
            Instruction2 = maps:put(<<"subframeId">>, CurrentSubframeId, Instruction1),
            get_instructions_since(BlastId, KeyframeId, SubframeId, CurrentSubframeId -1, 
                [Instruction2 | Instructions])
    end.

