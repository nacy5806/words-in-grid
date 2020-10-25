# words-in-grid
Usage: ./words_in_grid.out dict_file.txt

After compiling and initializing the program with a text file of words separated by whitespace (dict_file.txt), the user can then input a string of letters. The program will then print out all of the words in dict_file.txt that can be made with those letters. The reason the program specifies that "uppercase letters will be ignored" is because the dict_files that I used this with only ever had lowercase letters and whitespace; it is coded to be this way. The provided dict_file.txt (nwl2018.txt) is the list of words that are accepted by NASPA, the North American Scrabble Players' Association.

# ASSUMPTIONS
1) All the words in dict_file.txt are lowercase letters; no special characters or uppercase letters, though the code could easily be modified to allow for this.


