OBJS = main.o
CXX = g++
CFLAGS = -Wall -c
LFLAGS = -Wall
LIBS = -lpthread
binaries = ITC

main : $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) -o $(binaries)

main.o : main.cpp TestReadWrite.hpp TestWaitMessage.hpp TestDestroyItc.hpp ITC.h
	$(CXX) $(CFLAGS) $(INC) main.cpp

clean:
	rm -f $(binaries) *.o
