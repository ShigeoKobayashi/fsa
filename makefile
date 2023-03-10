DLL_SOURCES=blockio.cpp debug.cpp directory.cpp dllmain.cpp error.cpp fileio.cpp fsa.cpp header.cpp memory.cpp sat.cpp stdafx.cpp stream.cpp
HEADERS=stream.h blockio.h debug.h error.h fileio.h fsa.h directory.h header.h memory.h sat.h stdafx.h
TEST_SOURCES=tans.c

lib:
	g++ -std=c++1y -o libfsa.so -shared -fPIC -fvisibility=hidden $(DLL_SOURCES)
link:
	gcc -L./ -o tans tans.c -lfsa
#  make run ARGS='r tans.db'
run:
	LD_LIBRARY_PATH=. ./tans ${ARGS}
