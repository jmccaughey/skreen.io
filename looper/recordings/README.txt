the recording format is a 32 bit time value in milliseconds, 
then a 32 bit value that is the size of the following binary

to play a recording, call 

replay_client:start_replay("localhost", "8080", "../../recordings/recording.bin", <number of times to repeat>).


