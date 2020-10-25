#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>

/*

	You take a collection of letters that looks something like this:
	R U U W
	Y G D H
	T A R O
	D I I K

	and output all of the words that can be made using the letters from that collection.

	First, the grid is converted to a hash table, with indices being int(char letter) and values at those
	indices being how much of that letter is in the grid.

	The "word tree" structure is pretty simple; every path down the tree (starting from the root, not necessarily
	ending at a leaf) represents a collection of letters that can be built that either A) is a word, or B) is the
	start of a word. The idea was to use the "make recursive guesses" structure used in stuff like sudoku solvers,
	but only make guesses that could lead to a word. As an example, the program will never guess that the beginning
	of a word in nwl2018.txt is "lt", because there isn't a word like that in that file.

	There are 26 different trees, one for each letter of the alphabet.

	As an example, the tree for the letter L might look something like this:

						L
					/		\
					a       i
				   / \      / \
				   d n      o  n

	A DFS would generate the words / starts of words LAD, LAN, LIO, and LIN. We only
	go to a lower-level node if we have 1 or more of that letter available to use.

	As we search, we see if the nodes we're on are the end of words; this means that we can build a word
	from the letters in the grid. We keep track of these nodes that are ends of words, then at the end of our
	search print out the words.

	This program can be used to verify:

		In the sample grid above, there are 651 constructible words that are in nwl2018.txt.
		There are 38597 words in nwl2018.txt that can be constructed using all of the letters of the alphabet no more than once.
*/

class LetterNode {
	public:
		LetterNode(char l, bool word) { letter = l; endOfWord = word; parent = NULL; }
		bool isEndOfWord() const { return endOfWord; }
		char getLetter() const { return letter; }
		void addChild(LetterNode* child) { children.push_back(child); }
		void setParent(LetterNode* p) { parent = p; }
		void setEndOfWord(bool set) { endOfWord = set; }
		std::list<LetterNode*>::iterator getBeginningChildren() { return children.begin(); }
		std::list<LetterNode*>::iterator getEndChildren() { return children.end(); }

		friend std::ostream& operator<<(std::ostream& out, const LetterNode* ln);
		
	private:
		char letter;
		bool endOfWord;
		LetterNode* parent;
		std::list<LetterNode*> children;
};


std::ostream& operator<<(std::ostream& out, const LetterNode* ln) {
	// Prints the word from the tree, printing out everything from the parent upwards, then printing out ln's letter.
	if (ln != NULL) {
		out << ln -> parent << ln -> letter; // recursion is cool
	}

	return out;
}

void findWord(int* lettersAvailable, LetterNode* currentNode, std::list<LetterNode*>& knownWords) {
	// Standard DFS through the word trees, adding every node we can reach that's the end of a word
	// If a letter is available, then recurse.

	if (currentNode -> isEndOfWord()) knownWords.push_back(currentNode);

	for (std::list<LetterNode*>::iterator child_itr = currentNode -> getBeginningChildren(); 
		child_itr != currentNode -> getEndChildren(); child_itr++) 
	{
		if (lettersAvailable[int((*child_itr) -> getLetter()) - 97] > 0) {
			lettersAvailable[int((*child_itr) -> getLetter()) - 97] -= 1;
			findWord(lettersAvailable, *child_itr, knownWords);
			lettersAvailable[int((*child_itr) -> getLetter()) - 97] += 1;
		}
	}

}

// Driver function for the DFS
void findAllWords(const std::string& grid, LetterNode** wordTree) {

	int* letters = new int[26];
	for (int i = 0; i < 26; i++) letters[i] = 0;

	for (unsigned int i = 0; i < grid.size(); i++) {
		// create a "letter count"; letters[x] represents how many of letter x we have in grid
		letters[int(grid[i]) - 97] += 1;
	}

	std::list<LetterNode*> words;
	for (int i = 0; i < 26; i++) {
		// using all 26 possible starting points, start the DFS;
		// store all encountered LetterNodes that are the end of words in
		// the words list
		if (letters[i] > 0) {
			letters[i] -= 1;
			findWord(letters, wordTree[i], words);
			letters[i] += 1;
		}
	}

	// print out all the words, 6 words per line
	std::cout << words.size() << " words found:" << std::endl;
	int wordsOnLine = 0;
	for (std::list<LetterNode*>::iterator itr = words.begin(); itr != words.end(); itr++) {
		std::cout << *itr;
		
		if (wordsOnLine < 5) { std::cout << "\t"; wordsOnLine++; }
		else { std::cout << std::endl; wordsOnLine = 0; }
	}

	std::cout << std::endl;
	delete [] letters;
}

// Takes in the dictionary file, returns the word tree for that dictionary file.
// Assumes that all the words in the file are entirely lowercase letters.
LetterNode** makeWordTree(std::ifstream& infile) {
	std::cout << "Constructing word tree..." << std::endl;

	LetterNode** wordTree = new LetterNode*[26]; // 26 letters of the alphabet
	for (int i = 0; i < 26; i++) {
		// initialize the first layer of the word tree: individual lowercase letters. ascii values: [97, 122]
		wordTree[i] = new LetterNode(char(i + 97), false);
	}

	std::string word;
	while (infile >> word) {
		// add word to the word tree

		// find the starting point of the word
		LetterNode* currentNode = wordTree[int(word[0]) - 97];
		for (unsigned int i = 1; i < word.size(); i++) {
			// Check if word[i] is already a child of currentNode;
			// if it is, move to that node. if not, create a new node
			bool isAlreadyChild = false;
			for (std::list<LetterNode*>::iterator itr = currentNode -> getBeginningChildren(); 
					itr != currentNode -> getEndChildren(); itr++) 
			{
				if ((*itr) -> getLetter() == word[i]) {
					currentNode = *itr;
					isAlreadyChild = true;
					break;
				}

			}

			if (!isAlreadyChild) {
				// create a new child node with the required letter
				LetterNode* newNode = new LetterNode(word[i], false);
				newNode -> setParent(currentNode);
				currentNode -> addChild(newNode);

				currentNode = newNode;
			}
		}

		currentNode -> setEndOfWord(true);
	}

	std::cout << "Word tree successfully constructed." << std::endl;
	return wordTree;
}

// Destructor for a tree
void destroyNode(LetterNode* node) {
	for (std::list<LetterNode*>::iterator itr = node -> getBeginningChildren(); itr != node -> getEndChildren(); itr++) {
		destroyNode(*itr);
	}

	delete node;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		std::cout << "ERROR: No input file specified! Usage: ./main.exe <input-file>" << std::endl;
		return 1;
	}

	std::ifstream infile(argv[1]);
	if (!infile) {
		std::cout << "ERROR: Bad input file \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	LetterNode** tree = makeWordTree(infile);

	std::string word;
	std::cout << "Enter a group of letters (uppercase letters will be ignored) (STOP to stop): ";
	std::cin >> word;

	while (word != "STOP") {
		findAllWords(word, tree);
		std::cout << "Enter a lowercase group of letters (uppercase letters will be ignored) (STOP to stop): ";
		std::cin >> word;
	} 

	for (int i = 0; i < 26; i++) destroyNode(tree[i]);
	delete [] tree;

	return 0;
}