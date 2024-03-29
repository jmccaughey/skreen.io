-module(random_name).

-export([make/1, make_uuid/0]).

make_name(0, Acc) ->
   Acc;
make_name(N, Acc) ->
   make_name(N - 1, [random:uniform(10) + 47 | Acc]).

make(N) ->
   <<A:32, B:32, C:32>> = crypto:rand_bytes(12),
   random:seed({A,B,C}),
   make_name(N, []).

make_uuid() ->
    % from https://github.com/afiskon/erlang-uuid-v4/blob/master/src/uuid.erl
    <<A:32, B:16, C:16, D:16, E:48>> = crypto:rand_bytes(16),
    Str = io_lib:format("~8.16.0b-~4.16.0b-4~3.16.0b-~4.16.0b-~12.16.0b", 
                        [A, B, C band 16#0fff, D band 16#3fff bor 16#8000, E]),
    list_to_binary(Str).
