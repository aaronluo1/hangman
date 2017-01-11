// g++ -std=c++11 Hangman.cpp Game.cpp -o hangman -lboost_regex  
#include "Game.h"
#include <boost/regex.hpp>
#include <chrono>
#include <iostream>
#include <list>
#include <fstream>
#include <set>
#include <unordered_map>
#include <algorithm>
using namespace std;


// iterates through all possibilities and guessed characters to find most common character in 
// possible words that is not already guessed.
char findNextGuess(set<char> const guessed, list<string> const poss)
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
void trimPoss(list<string>* poss, char const wrong_char)
{
	
	boost::regex expr{string(1, wrong_char)};
	for(list<string>::iterator it = poss->begin(); it != poss->end(); ++it)
		if (boost::regex_match(*it, expr))
		{
		    it = poss->erase(it);
		    --it;
		}

}

void trimPoss(list<string>* poss, string curr)
{
	curr.insert(0,"^");                
	curr.insert(curr.length(),"$"); 
	
	boost::regex expr{curr};
	for(list<string>::iterator it = poss->begin(); it != poss->end(); ++it)
		if (!boost::regex_match(*it, expr))
		{
		    it = poss->erase(it);
		    --it;
		}
}

unordered_map<int, list<string>> readDict()
{
	unordered_map<int, list<string>> lenDic;
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

	return lenDic;
}

State play_game(string const word, unordered_map<int, list<string>> lenDic)
// State play_game(string const word)
{
	Game g (word);
	list<string> poss = lenDic[word.length()];
	// k4 c6 a7 b1 s1 p1 d1 m2 l2 t2
	// list<string> poss;
	// poss.push_back("back");
	// poss.push_back("sack");
	// poss.push_back("pack");
	// poss.push_back("deck");
	// poss.push_back("mace");
	// poss.push_back("lace");
	// poss.push_back("mate");
	// poss.push_back("late");


	while (g._state == PLAYING)
	{
		char guess = findNextGuess(g._guessed, poss);
		Result r = g.guess_letter(guess);
		if (r == GUESS_EXISTS)
			trimPoss(&poss, g._curr);
		else if (r == GUESS_DNE)
			trimPoss(&poss, guess);
		else
			break;
	}
	return g._state;
}


int main(int argc, char *argv[])
{
	unordered_map<int, list<string>> lenDic = readDict();
	if (argc == 2)
		play_game(argv[1], lenDic);
	if (argc == 1)
	{
		int wrong = 0; int total = 0;
		typedef chrono::high_resolution_clock Clock;
		auto t1 = Clock::now();
		for (int i = 0; i < lenDic.size(); i++)
		{
			total += lenDic[i].size();
			for (auto const w : lenDic[i])
				if (play_game(w, lenDic) == LOST)
					wrong ++;
		}
		auto t2 = Clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t2).count();
		int correct = total-wrong;
		cout << "Number of words tested: " << total << endl
			<< "Number of words guessed correctly: " << correct << endl
			<< "Correct Guesses (\%): " << double(correct/total) << endl
			<< "Time to run: " << duration << endl;
	}

	


	return 0;
}
