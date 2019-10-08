%% Feel free to use, reuse and abuse the code in this file.

-module(images_handler).

-export([init/2]).
-export([terminate/3]).

%init(_Transport, Req, []) ->
 %       {ok, Req, undefined}.

terminate(_Reason, _Req, _State) ->
        lager:debug("terminate"),
        ok.

init(Req, _Opts) ->
    lager:debug("entered init"),
    Skreen = cowboy_req:binding(skreen, Req),
    Image = cowboy_req:binding(image, Req),
    Key = cowboy_req:binding(key, Req),
    Path = "/tmp/skreen_images/" ++ binary_to_list(Skreen) ++ "/" ++ binary_to_list(Key) ++ "/" ++ binary_to_list(Image),
    % TODO: block '..' style directory traversal    
    lager:debug("images_handler: ~s", [Path]),
    {ok, RawFile} = file:open(Path, [raw, read, binary]),
    FileSize = filelib:file_size(Path),
    F = fun (Socket, Transport) ->
        Transport:sendfile(Socket, RawFile),
        file:close(RawFile)  
    end,
    Req2 = cowboy_req:set_resp_body_fun(FileSize, F, Req),
    {ok, cowboy_req:reply(200, [{<<"cache-control">>, <<"max-age=86400">>}], Req2), _Opts}.
    
