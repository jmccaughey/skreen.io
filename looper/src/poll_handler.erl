%% Feel free to use, reuse and abuse the code in this file.

%% @doc EventSource emitter.
-module(poll_handler).

-export([init/2]).
-export([terminate/3]).

init(Req, _Opts) ->
    % parse requested update number from request
    % if numbered update is available, send it with any other subsequent updates
    % if numbered update is not available, it is either because:
    %       it has not been received yet (so wait for message)
    %       a keyframe has been received since (send full update) 
    lager:debug("init"),
    QsVals = cowboy_req:parse_qs(Req),
    {_, BlastIdBin} = lists:keyfind(<<"blastId">>, 1, QsVals),
    BlastId = binary_to_list(BlastIdBin),
    {_, SerialBin} = lists:keyfind(<<"serial">>, 1, QsVals),
    %{_, SubframeIdBin} = lists:keyfind(<<"subframeId">>, 1, QsVals),
    lager:debug("blastId: ~s serial: ~s", [BlastId, SerialBin]),
    Serial = binary_to_integer(SerialBin),
    %SubframeId = binary_to_integer(SubframeIdBin),
    Instructions = skreen_repo:get_update(BlastId, Serial, self()),
    lager:debug("got instructions ~w", [Instructions]),
    case Instructions of
        skreen_closed ->
            lager:info("got skreen_closed, sending closed response to ~s", [BlastId]),
            Req2 = cowboy_req:reply(200, [{<<"content-type">>, <<"text/plain">>}], <<"session ended">>, Req),
            {ok, Req2, undefined_state};
        [] ->
            % no instructions available so wait
            receive
                update ->
                    NewInstructions = skreen_repo:get_update(BlastId, Serial, self()),
                    lager:debug("after wait got instruction ~w", [NewInstructions]),
                    case NewInstructions of
                        skreen_closed ->
                            lager:info("got skreen_closed, sending closed response to ~s", [BlastId]),
                            Req2 = cowboy_req:reply(200, [{<<"content-type">>, <<"text/plain">>}], <<"session ended">>, Req),
                            {ok, Req2, undefined_state};
                        _ ->
            	            encode_and_send(Req, [NewInstructions])
                    end
	        after 28000 ->
                lager:debug("timed out - sending empty json response"),
                encode_and_send(Req, [])
	        end;
        _ ->
    		encode_and_send(Req, [Instructions])
    end.


encode_and_send(Req, Instructions) ->
	Json = jiffy:encode(Instructions),
        lager:debug("json: ~s", [Json]),            
        Headers = [{<<"content-type">>, <<"text/javascript">>},
                {"content-encoding", "gzip"}
        ],
        lager:debug("sending instructions..."),
        Req2 = cowboy_req:reply(200, Headers, zlib:gzip(Json), Req),
        lager:debug("...sent instructions"),
        {ok, Req2, undefined_state}.

terminate(_, _, _) ->
	lager:debug("handling terminate"),
	ok.

