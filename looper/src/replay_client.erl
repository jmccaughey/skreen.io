-module(replay_client).

-export([start_replay/4]).

start_replay(Host, Port, Filename, 0) ->
    start_replay(Host, Port, Filename);
start_replay(Host, Port, Filename, Times) ->
    start_replay(Host, Port, Filename),
    start_replay(Host, Port, Filename, Times-1).

start_replay(Host, Port, Filename) ->
    {ok, Bin} = file:read_file(Filename),
    URL = io_lib:format("http://~s:~s/createAndStartBlast", [Host, Port]),
    R = httpc:request(lists:flatten(URL)),
    {ok, {{"HTTP/1.1", _ReturnCode, _State}, _Head, Body}} = R,
    [{Bodyparts}] = jiffy:decode(Body),
    BlastId = proplists:get_value(<<"blastId">>, Bodyparts),
    BlasterId = proplists:get_value(<<"id">>, Bodyparts),
    Key = proplists:get_value(<<"key">>, Bodyparts),
    PostURL = io_lib:format("http://~s:~s/scrapes?blastId=~s&blasterId=~s&key=~s", [Host, Port, BlastId, BlasterId, Key]),
    lager:info("replay_client url:~s", [PostURL]),
    replay(PostURL, Bin).

replay(URL, Data) ->
    application:start(inets),
    ok = replay(URL, Data, 0).

replay(URL, Data, 0) ->
    <<Millis:32/little, Size:32/little, Post:Size/binary, Rest/binary>> = Data,
    post(URL, Post, 0),
    replay(URL, Rest, Millis);
replay(_URL, <<>>, _LastMillis) ->
    ok;
replay(URL, Data, LastMillis) ->
    <<Millis:32/little, Size:32/little, Post:Size/binary, Rest/binary>> = Data,
    post(URL, Post, Millis - LastMillis),
    replay(URL, Rest, Millis).

post(URL, Data, Wait) ->
    lager:info("do wait ~w and post ~w bytes here", [Wait, size(Data)]),
    timer:sleep(Wait),
    Method = post,
    Header = [],
    Type = "application/json",
    HTTPOptions = [],
    Options = [],
    R = httpc:request(Method, {lists:flatten(URL), Header, Type, Data}, HTTPOptions, Options),
    {ok, {{"HTTP/1.1", _ReturnCode, _State}, _Head, _Body}} = R,
    ok.
