CXX = g++
CPPFLAGS = -g -Wall -std=c++11 

all: bin/scrabble

bin/scrabble: bin/Bag.o bin/ConsolePrinter.o bin/Player.o bin/Board.o bin/Dictionary.o bin/Move.o bin/Game.o bin/CPU.o bin/CPUMaxTiles.o bin/CPUMaxScore.o bin/Trie.o src/scrabble.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

bin/Bag.o: lib/Tile.h lib/Bag.h src/Bag.cpp
	$(CXX) $(CPPFLAGS) -c src/Bag.cpp -o $@

bin/Trie.o: lib/Trie.h src/Trie.cpp
	$(CXX) $(CPPFLAGS) -c src/Trie.cpp -o $@

bin/Dictionary.o: lib/Dictionary.h src/Dictionary.cpp
	$(CXX) $(CPPFLAGS) -c src/Dictionary.cpp -o $@

bin/Player.o: lib/Tile.h lib/Player.h src/Player.cpp
	$(CXX) $(CPPFLAGS) -c src/Player.cpp -o $@

bin/Move.o: lib/Tile.h lib/Move.h bin/Bag.o bin/Player.o bin/Dictionary.o src/Move.cpp
	$(CXX) $(CPPFLAGS) -c src/Move.cpp -o $@

bin/Board.o: lib/Tile.h lib/Square.h lib/Board.h bin/Dictionary.o bin/Move.o src/Board.cpp
	$(CXX) $(CPPFLAGS) -c src/Board.cpp -o $@

bin/ConsolePrinter.o: lib/ConsolePrinter.h bin/Board.o bin/Player.o src/ConsolePrinter.cpp
	$(CXX) $(CPPFLAGS) -c src/ConsolePrinter.cpp -o $@

bin/CPU.o: lib/CPU.h lib/Tile.h bin/Player.o bin/Dictionary.o bin/Board.o bin/Move.o bin/Trie.o bin/Bag.o src/CPU.cpp
	$(CXX) $(CPPFLAGS) -c src/CPU.cpp -o $@

bin/CPUMaxScore.o: lib/CPUMaxScore.h bin/CPU.o src/CPUMaxScore.cpp
	$(CXX) $(CPPFLAGS) -c src/CPUMaxScore.cpp -o $@

bin/CPUMaxTiles.o: lib/CPUMaxTiles.h bin/CPU.o src/CPUMaxTiles.cpp
	$(CXX) $(CPPFLAGS) -c src/CPUMaxTiles.cpp -o $@

bin/Game.o: lib/Game.h lib/Exceptions.h bin/CPU.o bin/ConsolePrinter.o src/Game.cpp
	$(CXX) $(CPPFLAGS) -c src/Game.cpp -o $@

clean:
	rm bin/*.o bin/scrabble

