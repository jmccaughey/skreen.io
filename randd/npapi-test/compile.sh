g++ npapi-test.cpp -c -fPIC -Wall -DOS_LINUX=1 -I"third_party" -I"third_party/npapi" -m64 -O3 && g++ -shared -W1,-soname,npapi-test-64.so -o npapi-test-64.so npapi-test.o -m64 -O3
