-module(join_handler).

-export([init/2]).
-export([terminate/3]).

terminate(_Reason, _Req, _State) ->
        ok.

init(Req, _Opts) ->
    lager:debug("join_handler init"),
    Skreen = cowboy_req:binding(skreen, Req),
    % TODO: notify host a join is requested, get permission ?
    Result = key_repo:get_key(binary_to_list(Skreen)),
    case Result of
        {ok, Key} ->
            BlasterId = random_name:make(6),
            {IP, _Port} = cowboy_req:peer(Req),
            view_repo:add_view(BlasterId, Skreen, IP),
            JsonResponse = io_lib:format("{\"blastId\":\"~s\", \"blasterId\":\"~s\", \"key\":\"~s\"}~n",
                [Skreen, BlasterId, Key]),
            lager:info("sending response to join: ~s", [JsonResponse]),
            %view_repo:save_view(
            Req2 = cowboy_req:reply(200, [
                {<<"content-type">>, <<"text/javascript">>}
        		], JsonResponse, Req),
            {ok, Req2, _Opts};
        _ ->
            lager:error("could not find key for skreen ~s", [Skreen])
    end.

