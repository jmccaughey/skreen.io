PROJECT = eventsource
DEPS = lager cowboy jiffy 
dep_jiffy = git https://github.com/davisp/jiffy master
%dep_jsx = git https://github.com/talentdeficit/jsx master
dep_cowboy = git https://github.com/jmccaughey/cowboy master
%dep_cowboy = git https://github.com/ninenines/cowboy master
dep_lager = git https://github.com/basho/lager master
include erlang.mk

# Compile flags
ERLC_COMPILE_OPTS= +'{parse_transform, lager_transform}'

# Append these settings
ERLC_OPTS += $(ERLC_COMPILE_OPTS)
TEST_ERLC_OPTS += $(ERLC_COMPILE_OPTS)

