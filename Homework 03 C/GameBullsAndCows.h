#pragma once

#ifndef GAME_BULLS_AND_COWS
#define GAME_BULLS_AND_COWS

#ifndef MYSTERY_NUMBER_LENGTH
#define MYSTERY_NUMBER_LENGTH 4
#endif

#ifndef DIFFRENT_SIGNS_AMOUNT
#define DIFFRENT_SIGNS_AMOUNT 6
#endif

struct GuessBullsAndCows
{
    int guessNumber;
    int bullsAndCows;
};

struct GameBullsAndCows {
    int formattedSecret;
    struct GuessBullsAndCows* guessHistory;
    int move;
    int* possibleAnswers;
    int possibleAnswersAmount;
    int currentPossibleAnswersAmount;
};

int GameBullsAndCowsLoop(struct GameBullsAndCows* game, int secret);

#endif