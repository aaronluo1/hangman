// g++ -std=c++11 Hangman.cpp Game.cpp -o hangman -lboost_regex  
#include "Game.h"
#include <boost/regex.hpp>
#include <iostream>
#include <list>
#include <fstream>
#include <set>
#include <unordered_map>
#include <algorithm>
using namespace std;

unordered_map<int, list<string>> lenDic;


// iterates through all possibilities and guessed characters to find most common character in 
// possible words that is not already guessed.
char findNextGuess(set<char> guessed, list<string> poss)
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

// we guessed the wrong character; iterate through possibilities, remove impossible elements
void trimPoss(list<string>* poss, char wrong_char)
{
	for(list<string>::iterator it = poss->begin(); it != poss->end(); it++)
		if ((*it).find('k') != string::npos)
		    it = poss->erase(it);
}

void trimPoss(list<string>* poss, string curr)
{
	curr.insert(0,"^");                
	curr.insert(curr.length(),"$"); 
	
	// boost::regex expr{curr};
	// boost::smatch what;
	// for (auto const& word : poss)
	// 	if (boost::regex_search(word, what, expr))
	boost::regex expr{curr};
	for(list<string>::iterator it = poss->begin(); it != poss->end(); ++it)
		if (!boost::regex_match(*it, expr))
		{
		    it = poss->erase(it);
		    --it;
		}

}

void readDict()
{
	string line;
	ifstream myfile ("words_50000.txt");
	if (myfile.is_open())
	{
		while (getline (myfile,line))
			if (lenDic.emplace(make_pair<int, list<string>>(int(line.length()), list<string>{string(line)})).second == false)
				lenDic[line.length()].push_back(line);
		myfile.close();
	}
	else 
		cout << "Unable to open file"; 
}

int main(int argc, char *argv[])
{
	// readDict();

	// Game* g = new Game("back");
	// cout << *g;
	// g->guess_letter('a');
	// cout << *g; 
	// delete g;

	// findNextGuess(guessed, possible);

	list<string> l;
	l.push_back("back");
	l.push_back("asdf");
	l.push_back("qwer");
	l.push_back("sack");
	l.push_back("stack");
	l.push_back("mat");
	l.push_back("daft");
	l.push_back("mace");
	l.push_back("kept");
	trimPoss(&l, ".ack");

	for (auto const& item : l)
		cout << item << " ";
	


	return 0;
}
