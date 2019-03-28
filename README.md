Name: Tong Zhi
Email: tongzhi@usc.edu

Compilation Method:
1)To compile, just type make.
2)If you wish to compile manually, then you can type the following command:
g++ -g -Wall -std=c++11 Bag.cpp ConsolePrinter.cpp Player.cpp Board.cpp Dictionary.cpp Move.cpp Game.cpp scrabble.cpp CPU.cpp CPUMaxTiles.cpp CPUMaxScore.cpp Trie.cpp

Detail Description:
1)This project is done by @CSCI104 course staff and me. There is a bunch of skeletion codes for me to start with, and I clearly indicate all of them in each files.
2)The game can be played among different players, including AIs. There are two kinds of AIs, one always uses the maximum amount of tiles, and the other one always score the maximum amount of points in each turn. The first AI is more fun to play against.
3)You can adjust the configuration of the game through the files in the scrabble_configs folder. There is another README.md in that folder which might give a better explanation of what to expect.


