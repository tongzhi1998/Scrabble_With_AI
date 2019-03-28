README for scrabble configuration:
1) init.txt is the file that you can edit to specify the initial state of the board. I leave it empty but if you wish to have some specific initial state, you can indicate blank space by putting a "." in the board and a letter in the board if you'd like to specify a letter in a tile.
2)standard-conf.txt is the file that specify the game's rule. There are five parameters within it:
1. Handsize: This determines the number of tiles in each player's hand. Note that in standard scrabble game, the handsize is 7.
2. Tiles: This determines the available tiles in the bag. It reads the file in standard-scrabble-setup. Goto that folder to change the file in it is you wish to change the configuration.
3. Board: This determines the size and the initial bonus tiles' position in the baord. Again, it reads the file in standard-scrabble-conf, and you can change the file within that folder accordingly.
4. Seed: Just the random seed for this game :D
5. Dictionary: This determines the set of legal words. Again, you can change it in standard-scrabble-setup folder.