%% Feel free to use, reuse and abuse the code in this file.

-module(scrapes_handler).

-export([init/2]).
-export([terminate/3]).

terminate(_Reason, _Req, _State) ->
        lager:debug("terminate"),
	ok.

init(Req, _Opts) ->
    % if this is a keyframe, wipe state and insert it
    % if not keyframe, insert it
    % find any process waiting for messages
    % send message to them, saying new content available
    {ok, Body, Req2} = cowboy_req:body(Req, [{read_timeout, infinity}, {continue, false}]),
    QsVals = cowboy_req:parse_qs(Req2),
    {_, BlastIdBin} = lists:keyfind(<<"blastId">>, 1, QsVals),
    lager:debug("BlastIdBin: ~s", [BlastIdBin]),
    {_, BlastKey} = lists:keyfind(<<"key">>, 1, QsVals),
    BlastId = binary_to_list(BlastIdBin),
    Recording = os:getenv("SKREEN_IO_RECORDING", "false"),    
    case Recording of
        "true" ->
            Path = BlastId ++ "_scrape_recording.bin",
            Size = size(Body),
            lager:debug("recording!"),
            CurrentMillis = erlang:monotonic_time(milli_seconds),
            file:write_file(Path, [<<CurrentMillis:32/little>>, <<Size:32/little>>, Body], [raw, append]);
        _Else ->
            false
    end,    
    Message = scrape_parser:parse(Body, BlastId, BlastKey),
    %Keyframe = maps:get(<<"keyFrame">>, Message),
    %case Keyframe of
    %    0 ->
            % not keyframe. update via repo;
    %        lager:debug("got update"),
            skreen_repo:update(BlastId, Message),
     %   _ ->
            % keyframe received. update via repo
     %       lager:debug("got keyframe"),
     %       instruction_repo:keyframe(BlastId, Message)
    %end,
    Req3 = cowboy_req:reply(200, [
		{<<"content-type">>, <<"text/plain">>}
	], <<1:8>>, Req2),
    {ok, Req3, _Opts}.

