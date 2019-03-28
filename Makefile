CXX = g++
CPPFLAGS = -g -Wall -std=c++11 

all: scrabble

scrabble: Bag.cpp ConsolePrinter.cpp Player.cpp Board.cpp Dictionary.cpp Move.cpp Game.cpp scrabble.cpp CPU.cpp CPUMaxTiles.cpp CPUMaxScore.cpp Trie.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@