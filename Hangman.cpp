// g++ -std=c++11 Hangman.cpp Game.cpp -o hangman
#include "Game.h"
#include <iostream>
#include <fstream>
#include <set>
#include <unordered_map>
#include <algorithm>
using namespace std;

unordered_map<int, vector<string>> lenDic;


// iterates through all possibilities and guessed characters to find most common character in 
// possible words that is not already guessed.
char findNextGuess(set<char> guessed, vector<string> poss)
{
	unsigned int totalCounts[256] = {0};

	for (auto const& word: poss)
	{
		// letters should only count once per word
		// eg: in "shah", the h should only count once.
		set<char> string_set;
		for (auto c : word)
			string_set.insert(c);

		for (auto c : string_set)
			if (guessed.find(c) == guessed.end())
				totalCounts[(unsigned char)c]++;
	}

	unsigned int max = 0;
	char nextGuess;
	for (int i = 0; i < 256; i++)
		if (totalCounts[i] > max)
		{
			max = totalCounts[i];
			nextGuess = (char)i;
		}

	return nextGuess;
}

void readDict()
{
	string line;
	ifstream myfile ("words_50000.txt");
	if (myfile.is_open())
	{
		while (getline (myfile,line))
			if (lenDic.emplace(make_pair<int, vector<string>>(int(line.length()), vector<string>{string(line)})).second == false)
				lenDic[line.length()].push_back(line);
		myfile.close();
	}
	else 
		cout << "Unable to open file"; 
}

int main()
{
	// readDict();
	Game* g = new Game("back");

	cout << *g;
	g->guess_letter('a');
	cout << *g; 


	cout << endl << "obj: " << *g;

	delete g;
	// findNextGuess(guessed, possible);

	return 0;
}
