RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -std=c++11 -I ./../..
CXXFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= -static-libgcc
LDLIBS=/usr/local/jansson/lib/libjansson.a -lcurl


