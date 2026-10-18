#include "GameBullsAndCows.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//Для самих чисел
static const int BitAmount = 
    (DIFFRENT_SIGNS_AMOUNT <= 2) ? 1 :
    (DIFFRENT_SIGNS_AMOUNT <= 4) ? 2 :
    (DIFFRENT_SIGNS_AMOUNT <= 8) ? 3 : 4;
static const int BinaryModule = 
	(DIFFRENT_SIGNS_AMOUNT <= 2) ? 2 :
    (DIFFRENT_SIGNS_AMOUNT <= 4) ? 4 :
    (DIFFRENT_SIGNS_AMOUNT <= 8) ? 8 : 16;

//Для BullsAndCows
static const int BitAmountBySign =
	(MYSTERY_NUMBER_LENGTH <= 2) ? 2 :
	(MYSTERY_NUMBER_LENGTH <= 4) ? 3 :
	(MYSTERY_NUMBER_LENGTH <= 8) ? 4 : 5;
static const int BinaryModuleBySign = 
	(MYSTERY_NUMBER_LENGTH <= 2) ? 4 :
	(MYSTERY_NUMBER_LENGTH <= 4) ? 8 :
	(MYSTERY_NUMBER_LENGTH <= 8) ? 16 : 32;




///ЗАДАТЬ ВОПРОС ПО ИСКЛЮЧЕНИЯМ!!!!!! В случае, если в int не будет помещаться число

static int getSignInNumber(const int *number, int i)
{
    return (*number >> i * BitAmount) % BinaryModule;
}

static int getNumberBySign(int *numberBySigns, int i)
{ 
	return (*numberBySigns >> (i * BitAmountBySign)) % BinaryModuleBySign;
}

//Из 6-ой в бинарный массив
static int formatNumberToBinaryMas(int number) 
{
    int formattedNumber = 0;

	int digitPlace = 1;
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		int mysterySign = ((number / digitPlace) % DIFFRENT_SIGNS_AMOUNT);
		
		formattedNumber += mysterySign << (i * BitAmount);
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}
    
    return formattedNumber;
}



static void printNumberBySign(int formattedNumberBySign, int amount)
{
	for (int i = amount-1; i >= 0; i--)
		printf_s("%i", getNumberBySign(&formattedNumberBySign, i));
}

static int getNumber(int formattedNumber)
{
	int number = 0;
	for (int i = MYSTERY_NUMBER_LENGTH-1; i >= 0 ; i--)
	{
		number += (formattedNumber >> (i * BitAmount)) % BinaryModule;
	}
	return number;
}

static void printNumber(int formattedNumber)
{
	for (int i = MYSTERY_NUMBER_LENGTH-1; i >= 0; i--)
		printf_s("%i", getSignInNumber(&formattedNumber, i));
}


static int checkGuess(int formattedSecret, int formattedGuess)
{
	int bullsAndCows =0;

	int bullsBySignBinary = 0,
		guessBySignBinary =0,
		secretBySignBinary = 0; 
	
	//считаем bullsBySign и secretBySignBinary
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		int secretSign = getSignInNumber(&formattedSecret, i);
		int guessSign = getSignInNumber(&formattedGuess, i);

		secretBySignBinary +=  1 << (secretSign * BitAmountBySign);
		guessBySignBinary += 1 << (guessSign * BitAmountBySign);

		if (secretSign == guessSign)
			bullsBySignBinary += 1 << (secretSign * BitAmountBySign);;
	}

	//Считаем bulls 
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT; i++)
	{
		bullsAndCows += (bullsBySignBinary >> (i * BitAmountBySign)) % BinaryModuleBySign;
	}

	//Считаем Cows
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT; i++)
	{
		bullsAndCows += 
			( 
				min( getNumberBySign(&guessBySignBinary,i), getNumberBySign(&secretBySignBinary,i))
				- getNumberBySign(&bullsBySignBinary,i) 
			) << BitAmountBySign;
	}
	return bullsAndCows;
}

static void LogMove(struct GameBullsAndCows* game)
{
	printf_s("m: %i:\t g:", game->move);
	printNumber(game->guessHistory[game->move].guessNumber);
	printf_s("\t cb:");
	printNumberBySign(game->guessHistory[game->move].bullsAndCows, 2);
	printf_s(":\t remained: %i\n", game->currentPossibleAnswersAmount);
}

static int MakeMove(struct GameBullsAndCows* game, int formattedGuess)
{
	int bullsAndCows = checkGuess(game->formattedSecret, formattedGuess);

	game->guessHistory[game->move] = (struct GuessBullsAndCows){ formattedGuess, bullsAndCows };

	if (bullsAndCows == MYSTERY_NUMBER_LENGTH) //Победа
	{
		game->currentPossibleAnswersAmount = 1;
		LogMove(game);
		return 1; 
	}



	
	if (!game->possibleAnswers) //Если память не выделена
	{
		game->possibleAnswers = calloc(game->possibleAnswersAmount, sizeof(int)); // проверили сразу же
		for (int i = 1; i < game->possibleAnswersAmount; i++)
		{
			game->possibleAnswers[i] = formatNumberToBinaryMas(i);
		}
	}

	game->possibleAnswers[getNumber(formattedGuess)] = 0; //Вычёркиваем проверенный

	game->currentPossibleAnswersAmount = 0;
	for (int i = 1; i < game->possibleAnswersAmount; i++)
	{
		if (!game->possibleAnswers[i])
			continue;

		if (checkGuess(formattedGuess, game->possibleAnswers[i]) == bullsAndCows)
		{
			game->currentPossibleAnswersAmount++;
		}
		else
		{
			game->possibleAnswers[i] = 0; //вычёркиваем
		}
	}
	
	LogMove(game);

	if (game->currentPossibleAnswersAmount == 1) //Победа
	{
		printf_s("Last remained: \t");
		int lastNumber;
		for (int i = 0; i < game->possibleAnswersAmount; i++)
			if (game->possibleAnswers[i])
				lastNumber = game->possibleAnswers[i];
		printNumber(lastNumber);
		return 1;
	}

	game->move++;

	return 0; // Игра продолжается
}

int GameBullsAndCowsLoop(struct GameBullsAndCows* game, int secret)
{
	
	game->guessHistory = calloc((int)(DIFFRENT_SIGNS_AMOUNT + MYSTERY_NUMBER_LENGTH * (MYSTERY_NUMBER_LENGTH - 1)), (sizeof(struct GuessBullsAndCows)));
	
	game->formattedSecret = formatNumberToBinaryMas(secret);
	game->move = 0;
	game->possibleAnswersAmount = (int)powl(DIFFRENT_SIGNS_AMOUNT, MYSTERY_NUMBER_LENGTH); //6^4
	

	printf_s("mystery number: ");
	printNumber(game->formattedSecret);
	printf_s("\tpossible guesses: %i\n", game->possibleAnswersAmount);

	game->currentPossibleAnswersAmount = game->possibleAnswersAmount;
	int won = MakeMove(game, 0);
	
	while (!won)
	{
		int someAliveGuess;
		for (int i = 0; i < game->possibleAnswersAmount; i++)
		{
			if (game->possibleAnswers[i])
			{
				someAliveGuess = game->possibleAnswers[i];
				break;
			}
		}
		won = MakeMove(game, someAliveGuess);
	}


	free(game->possibleAnswers);
	game->possibleAnswers = NULL;
	free(game->guessHistory);
	return 0;
	
}