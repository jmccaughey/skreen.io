-module(loop_test).

-export([get_result/0]).

get_result() ->
    Number = get_result(0),
    Number.    

get_result(Try) ->
    lager:info("try # ~w", [Try]),
    Number = get_number(Try),
    if
        Number == 0 ->
            get_result(Try + 1);
        true ->
            Number
    end.

get_number(Try) when Try > 10 -> 200;
get_number(_) -> 0.
    
