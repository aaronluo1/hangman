#ifndef GAME_H
#define GAME_H
#include <string>
#include <vector>
#include <iostream>
#include <set>

enum State { PLAYING, WON, LOST };
enum Result { GUESS_EXISTS, GUESS_DNE, ERROR };

class Game {
	private:
		std::string _word;
	public:
		std::string _curr;
		std::set<char> _failed;
		std::set<char> _guessed;
		State _state;

		Game(std::string s);
		std::string get_word();
		Result guess_letter(char c);
		friend std::ostream& operator<< (std::ostream& out, const Game& obj); 
};

#endif