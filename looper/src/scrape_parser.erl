-module(scrape_parser).
-export([parse/3]).

parse(<<Keyframe:32/integer, Width:32/integer, Height:32/integer, MouseX:32/integer, MouseY:32/integer, TileCount:32/integer, Rest/binary>>, BlastId, BlastKey) ->
	lager:debug("keyframe: ~w width: ~w height: ~w tile count: ~w~n", [Keyframe, Width, Height, TileCount]),
    Scrapes = scrapes(Rest, BlastId, BlastKey),
    % convert list of scrapes to map with key x_y
    ScrapeMap = scrape_list_to_map(Scrapes),
	#{keyFrame =>  Keyframe, w => Width, h => Height, tiles => ScrapeMap, mX => MouseX, mY => MouseY}.

scrapes(<<Name:32/binary, Length:32/integer, Xpos:32/integer, Ypos:32/integer, _Width:32/integer, _Height:32/integer, SpriteX:32/integer, 
		SpriteY:32/integer, Image:Length/binary, Rest/binary>>, BlastId, BlastKey) ->
	if 0 =:= Length ->
		noop;
		true -> write_to_fs(Image, BlastId, BlastKey, Name)
	end,
	[#{hash => binary:copy(Name), x => Xpos, y => Ypos,	sX => SpriteX, sY => SpriteY} | scrapes(Rest, BlastId, BlastKey)];
scrapes(<<>>, _BlastId, _BlastKey) ->
	[]. 
	
write_to_fs(Data, BlastId, BlastKey, Name) ->
        lager:debug("BlastId:~w", [BlastId]),
	TempFilename = "/tmp/skreen_images/" ++ BlastId ++ "/" ++ binary_to_list(BlastKey) ++ "/" ++ binary_to_list(Name),
	lager:debug("temp file name ~s", [TempFilename]),
	{ok, File} = file:open(TempFilename, [raw, write]),
	file:write(File, Data),
	file:close(File).

scrape_list_to_map(Scrapes) ->
	%ScrapeMap = #{},
	lists:foldl(
		fun(Scrape, ScrapeMap) -> 
			Name = list_to_binary(io_lib:format("~w~s~w", [maps:get(x, Scrape), "_", maps:get(y, Scrape)])),
			TrimmedScrape = #{ sX => maps:get(sX, Scrape), sY => maps:get(sY, Scrape), h => maps:get(hash, Scrape)},
			maps:put(Name, TrimmedScrape, ScrapeMap)
		end, 
		#{}, Scrapes).
