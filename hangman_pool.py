from collections import defaultdict, Counter
from enum import Enum
from timeit import default_timer as timer
from multiprocessing import Value, Pool
from ctypes import c_int
import re, argparse, signal

# global dictionary of words (key: length, value: list of words)
lenDict = defaultdict(list)

class State(Enum):
    PLAYING = 1
    WON = 2
    LOSS = 3

class Result(Enum):
    GUESS_EXISTS = 4
    GUESS_DNE = 5
    ERROR = 6

def _initProcess(x):
    wrong = x

# represents the status of the game
class Game:
    def __init__(self, inWord):
        self._word = list(inWord)
        self._curr = ['.'] * len(inWord)     # some list that represents a regex object
        self._fail = []                      # some list of failed letters
        self._guessed = []                   # some list of guessed letters
        self._state = State.PLAYING

    # guess a certain letter.
    # updates fail or curr accordingly
    def guess_letter(self, guess):
        if guess in self._guessed:
            print "YOU HAVE ALREADY GUESSED THIS LETTER"
            return Result.ERROR
        if self._state is not State.PLAYING:
            print "GAME OVER"
            return Result.ERROR

        self._guessed.append(guess)
        found = False

        for counter, letter in enumerate(self._word):
            if letter is guess:
                self._curr[counter] = guess
                found = True

        if found is False:
            self._fail.append(guess)
            if len(self._fail) >= 7:
                self._state = State.LOSS
                #print "YOU HAVE FAILED IN GUESSING THE WORD:", "".join(self._word)
            return Result.GUESS_DNE
        else:
            if '.' not in self._curr:
                self._state = State.WON
                #print "YOU HAVE SUCCEEDED IN GUESSING THE WORD:", "".join(self._word)
            return Result.GUESS_EXISTS

    def __str__(self):
        return "Word: {0}; Curr: {1}; Fail: {2}; Guessed: {3}".format("".join(self._word), "".join(self._curr), self._fail, self._guessed)


# read text input dictionary, get lenDict.
# note: same working folder
def readDictionary():
    text_file = open('words_50000.txt', 'r')
    lines = text_file.read().splitlines()
    total = 0
    for word in lines:
        l = len(word)
        lenDict[l].append(word)
        total += 1
    return total

# iterate through lenDict, find most common letter
def findNextGuess(guessed, dic):
    count = Counter()
    for words in dic:
          for letters in set(words):
              count[letters]+=1
    for guess in count.most_common():
        if guess[0] not in guessed:
            return guess[0]

# run regex match on current list to trim possible solutions list
def trimPoss(curr, poss, missed=None):
    if missed is None:
        pattern = "^" + "".join(curr) + "$"
        searchRegex = re.compile(pattern).search
        return [l for l in poss for m in (searchRegex(l),) if m]
    else:
        return [word for word in poss if missed not in word]

# number of possibilities is less than the number guesses,
# guess words, we will win.
# note: using this function actually makes the overall running time slower.
def guessWords(game, poss):
    for word in poss:
        if word == "".join(game._word):
            game._state = State.WON
            return True
        else:
            game._guessed.append(word)
            if len(game._guessed) >= 7:
                game._state = State.LOSS
                return False

# Hangman solver
def solve(word):
    game = Game(word)
    poss = lenDict[len(game._word)]

    while game._state is State.PLAYING:
        guess = findNextGuess(game._guessed, poss)
        res = game.guess_letter(guess)

        if res is Result.GUESS_EXISTS:
            poss = trimPoss(game._curr, poss)
            # de facto game won
            # this branch is not used because it will increase the running time
            # if len(poss) <= 7-len(game._guessed):
            #     guessWords(game, poss)
            #     break
        elif res is Result.GUESS_DNE:
            poss = trimPoss(game._curr, poss, guess)
        else:
            break
    return game._state

# prints statistics as outlined by requirements
def printStats(time, correct, total):
    print "Number of words tested:", total
    print "Number of words guessed correctly:", correct
    print "Correct Guesses (%):", float(correct)/total * 100
    print "Time to run:", time


def thread_worker(word):
    if solve(word) is State.LOSS:
        wrong.value += 1

def signal_handler(signal, frame):
    print 'You have tried to exit the program! Exiting process pools...'
    sys.exit(0)


# main function
if __name__ == "__main__":
    # saves to global variable so we only read the file once
    total = readDictionary()

    # parser for optional string input that solves a specific word
    # if argument does not exist, run through whole dictionary.
    parser = argparse.ArgumentParser(description='Hangman Solver.')
    parser.add_argument('-w', default=None, help='Specific input for Hangman.')
    args = parser.parse_args()

    if args.w is not None:
        solve(args.w)
    else:
        start = timer()
        wrong = Value(c_int)
        signal.signal(signal.SIGINT, signal_handler)
        pool = Pool(initializer=_initProcess,initargs=(wrong,))
        for k, v in lenDict.iteritems():
            pool.map(thread_worker, v)
        pool.close()
        pool.join()
        end = timer()
        printStats(end - start, total - wrong.value, total)


