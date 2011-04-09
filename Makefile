CC=gcc
CXX=g++
#CXXFLAGS =-g -Wall -O2 -DUSE_BGP
CXXFLAGS =-g -Wall -DUSE_BGP
LIBS=-lz 
TARGET=netcarver

main_cpp = main.cpp util.cpp
           
main_o = $(main_cpp:.cpp=.o)

src: $(TARGET)

netcarver: $(main_o) common.h
	$(CXX) $(CXXFLAGS) -o $@ $(main_o) $(LIBS)

%.o: %.cpp common.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) core *.o

dep depend:
	$(CXX) -MM  \
	$(wildcard *.cpp) > .depend

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
# DO NOT DELETE
