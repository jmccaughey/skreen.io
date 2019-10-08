-module(listservers_handler).

-export([init/2]).

% TODO: discover dynamic list of servers based on load

init(Req, []) ->
    lager:info("serving server list"),
    Req2 = cowboy_req:reply(200, [
        {<<"content-type">>, <<"text/plain">>}
    ], <<"west.skreen.io\nskreen.io\n">>, Req),
    {ok, Req2, []}.    
