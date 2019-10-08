-module(loop_register).

-export([register_server/2]).
-export([try_blast_id/2]).

register_server(Server, Size) ->
    request_register(Server, 0, Size).    

request_register(Server, Try, Size) ->
    BlastId = random_name:make(Size),
    Key = random_name:make_uuid(),
    BlasterId = random_name:make(Size),
    lager:info("try # ~w with ~s", [Try + 1, BlastId]),
    Result = try_blast_id(BlastId, Server),
    % TODO: should the register server possess the key?
    if
        Result == 0, Try < 10 ->
            lager:info("trying again"),
            request_register(Server, Try + 1, Size);
        true ->
            ImageDir = io_lib:format("/tmp/skreen_images/~s/~s/", [BlastId, Key]),
            lager:info("attempting to create directory ~s", [ImageDir]),
            filelib:ensure_dir(ImageDir),
            key_repo:add_key(BlastId, Key),
            {Result, BlasterId, Key}
    end.

try_blast_id(BlastId, Server) ->
    RegisterServer = os:getenv("SKREEN_IO_REGISTER_SERVER", "skreen.io"),
    Query = 
        io_lib:format("http://~s/registerServer?blast_id=~s&server=~s", 
        [
        RegisterServer,
         BlastId, Server
        ])
        ,
    lager:info("url: ~s", [Query]),
    Result = httpc:request(lists:flatten(Query)),
    case Result of
        {ok, {{_Version, 200, _ReasonPhrase}, _Headers, _Body}} ->
            lager:info("registerServer success"),
            BlastId;
        {ok, {{_Version, 409, _ReasonPhrase}, _Headers, _Body}} ->
            lager:error("registerServer failed"),
            0;
        _ ->
            lager:error("registerServer failed"),
            0
    end.

