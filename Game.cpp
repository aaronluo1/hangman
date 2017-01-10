#include "Game.h"
#include <algorithm>
using namespace std;

ostream& operator<< (ostream& out, const Game& obj)
{
    out << "Word: " << obj._word << endl;
    out << "Curr: " << obj._curr << endl;
    out << "Guessed: ";
    for (auto const& letter: obj._guessed)
    	out << letter;
    out << endl << "Failed: ";
	for (auto const& letter: obj._failed)
    	out << letter;
    out << endl << "State: " << obj._state  << endl;    
    return out;
}


Game::Game(string word) :
					_word (word),
					_state (PLAYING)
{
	_curr.assign(word.size(), '.');
}

string Game::get_word() 
{
	return this->_word;
}

Result Game::guess_letter(char c) 
{
	if (this->_guessed.find(c) != this->_guessed.end())
	{
		cout << "YOU HAVE ALREADY GUESSED THIS LETTER" << endl;
		return ERROR;
	}
	if (this->_state != PLAYING)
	{
		cout << "GAME OVER" << endl;
		return ERROR;
	}

	this->_guessed.insert(c);
	bool found = false;

	int index = 0;
	string word = this->get_word();
	for (auto it = word.begin(); it != word.end(); ++it, ++index) 
	{
		string new_letter(1, c);
		if (*it == c)
		{
			// cout << "debug: " << *it << c << endl;
			found = true;
			this->_curr.replace(index, new_letter.length(), new_letter);
		}
	}

	if (!found)
	{
		this->_failed.insert(c);
		if (this->_failed.size() >= 7)
		{
			cout << "YOU HAVE FAILED IN GUESSING THE WORD: " << this->get_word() << endl;
			this->_state = LOST;
		}
		return GUESS_DNE;
	}	
	else
	{
		size_t found = this->_curr.find('.');
		if (found == string::npos)
			this->_state = WON;
		return GUESS_EXISTS;
	}
}