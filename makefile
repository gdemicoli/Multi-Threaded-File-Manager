CXX = g++

CXXFLAGS = -Wall -Werror -std=c++11 -pthread

TARGETS = mmcopier mscopier

MM_SRC = mmcopier.cpp
MS_SRC = mscopier.cpp zem.cpp

all: $(TARGETS)

mmcopier: $(MM_SRC)
	$(CXX) $(CXXFLAGS) -o mmcopier $(MM_SRC)

mscopier: $(MS_SRC)
	$(CXX) $(CXXFLAGS) -o mscopier $(MS_SRC)

clean:
	rm -f $(TARGETS) *.o
