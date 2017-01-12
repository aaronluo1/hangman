// g++ -std=c++11 Hangman.cpp Game.cpp -o hangman -lboost_regex  
#include "Game.h"
#include <boost/regex.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <unordered_map>
#include <algorithm>
using namespace std;


// iterates through all possibilities and guessed characters to find most common character in 
// possible words that is not already guessed.
char findNextGuess(set<char> const guessed, vector<string> const poss)
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
void trimPoss(vector<string>* poss, char const wrong_char)
{
 	for(vector<string>::iterator it = poss->begin(); it != poss->end(); )
 		if ((*it).find(wrong_char) != string::npos)
  		    it = poss->erase(it);
  		else
  			++it;
}

void trimPoss(vector<string>* poss, string curr)
{
	curr.insert(0,"^");                
	curr.insert(curr.length(),"$"); 
	
	boost::regex expr{curr};
	for(vector<string>::iterator it = poss->begin(); it != poss->end(); )
		if (!boost::regex_match(*it, expr))
			it = poss->erase(it);
		else
			++it;

}

int readDict(unordered_map<int, vector<string>>* lenDic)
{
	int counter = 0;
	string line;
	ifstream myfile ("words_50000.txt");
	if (myfile.is_open())
	{
		while (getline (myfile,line))
			if (lenDic->emplace(make_pair<int, vector<string>>(int(line.length()), vector<string>{string(line)})).second == false)
			{
				(*lenDic)[line.length()].push_back(line);
				counter++;
			}
		myfile.close();
	}
	else 
		cout << "Unable to open file"; 

	return counter;
}

void readDict(vector<string>* l, int len)
{
	string line;
	ifstream myfile ("words_50000.txt");
	if (myfile.is_open())
	{
		while (getline (myfile,line))
			if (line.length() == len)
				l->push_back(line);
		myfile.close();
	}
	else 
		cout << "Unable to open file"; 
}

State play_game(string const word)
{
	Game g (word);
	vector<string> poss;
	readDict(&poss, word.length());
	// k4 c6 a7 b1 s1 p1 d1 m2 l2 t2
	// vector<string> poss;
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
		// cout << guess << endl << g << endl;
		// for (auto w : poss)
		// 	cout << w << " ";
	}
	return g._state;
}

State play_game(string const word, unordered_map<int, vector<string>> lenDic)
{
	Game g (word);
	vector<string> poss = lenDic[word.length()];

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
	/*
	unordered_map tradeoff: with more memory space, creating this unordered_map is faster.
	However, since I am using a small machine, it is faster to read the file for every new word.
	*/
	/*
	set vs list vs vector tradeoff: use sets if characters in dictionary are repeating (there is one repeating
	word, I'm unsure if this was on purpose). I originally used lists since we don't need random access, but for
	OpenMP we need vectors since for loops can not use iterators for OpenMP.
	*/
	unordered_map<int, vector<string>> lenDic;
	int total = readDict(&lenDic);
	if (argc == 2)
		play_game(argv[1]);
	if (argc == 1)
	{
		int wrong = 0; int total = 0;
		typedef chrono::high_resolution_clock Clock;
		auto t1 = Clock::now();

		#pragma omp parallel for collapse(2)
		for (int i = 0; i < lenDic.size(); i++)
			for(int j = 0; j < lenDic[i].size(); ++j)
				if (play_game(lenDic[i][j], lenDic) == LOST)
					#pragma omp atomic
					++wrong;



		/* code from unordered_map tradeoff; runs faster on my machine, but probably not on yours
		string line;
		ifstream myfile ("words_50000.txt");
		if (myfile.is_open())
		{
			while (getline (myfile,line))
			{
				++total;
				if (play_game(w) == LOST)
					++wrong;
			}
			myfile.close();
		}
		else 
			cout << "Unable to open file"; 
		*/

		auto t2 = Clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
		int correct = total-wrong;
		cout << "Number of words tested: " << total << endl
			<< "Number of words guessed correctly: " << correct << endl
			<< "Correct Guesses (\%): " << double(correct/total) << endl
			<< "Time to run: " << duration << " ms" << endl;
	}

	


	return 0;
}
