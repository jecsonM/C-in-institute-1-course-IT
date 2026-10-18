#pragma once

#ifndef GAME_ONLY_BULLS
#define GAME_ONLY_BULLS

#define MYSTERY_NUMBER_LENGTH 4
#define DIFFRENT_SIGNS_AMOUNT 6

struct GuessBullsOnly
{
    int guessNumber;
    int bulls;
    int cows;
};

struct GameBullsOnly {
    int bestGuess;
    int knownMasc;
    int* knownComposition;

    int MysterNumber;
    struct GuessBullsOnly* guessHistory;
    int move;
    int* cows;
    int* bulls;
    int possibleAnswers;
    int maxMove;
};

int GameOnlyBullsLoop(struct GameBullsOnly* game, int mysteryNumber);

#endif