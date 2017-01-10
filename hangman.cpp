// g++ -std=c++11 hangman.cpp -o hangman

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

unordered_map<int, vector<string>> lenDic;


enum State { PLAYING, WON, LOST };
enum Result { GUESS_EXISTS, GUESS_DNE, ERROR };

class Game {
	private:
		string _word;
	public:
		string _curr;
		vector<char> _failed;
		vector<char> _guessed;
		State _state;

		Result guess_letter(char c);
		friend ostream& operator<< (ostream& out, const Game& obj); 
};

ostream& operator<< (ostream& out, const Game& obj)
{
    out << "Word: " << obj._word;
    out << "Curr: " << obj._curr;
    out << "Failed: ";
    for (auto const& letter: obj._failed)
    	out << letter;
    out << "State: " << obj._state;    
    return out;
}

// Result guess_letter(char c) 
// {

// }

// Game::Game(string word) :
// 					_word (word),
// 					_state (PLAYING)
// {
// 	_curr.assign(word.size(), ".");
// }

// iterates through all possibilities and guessed characters to find most common character in 
// possible words that is not already guessed.
char findNextGuess(vector<char> guessed, vector<string> poss)
{
	unsigned int totalCounts[256] = {0};

	for (auto const& word: poss)
	{
		// letters should only count once per word
		// eg: in "shah", the h should only count once.
		set<char> string_set;
		for (auto& c : word)
			string_set.insert(c);
		
		for (auto& c : string_set)
			if (find(guessed.begin(), guessed.end(), c) == guessed.end())
				totalCounts[(unsigned char)c] = 1;
	}

	int max = 0;
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
	
	// for (auto& item: lenDic)
	// 	cout << item.first << ": " << item.second[0] << endl;
	// cout << lenDic.size();
}

int main()
{
	// readDict();


	vector<char> guessed = {'a', 'c'};
	vector<string> poss = {"bat", "cat", "chh"};

	cout << findNextGuess(guessed, poss);
}