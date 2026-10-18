//Что должна делать программа
//Программа играет в «быки и коровы».Одна сторона загадывает последовательность из четырёх цифр,
//другая называет догадки, и после каждой догадки сообщается, насколько она близка к загаданному.
//Правило, по которому строится этот ответ, у каждого варианта своё и описано ниже.
//Уровень 1. Загаданное записано прямо в тексте программы.Программа в цикле принимает догадки, после
//каждой печатает ответ по правилу варианта и заканчивает работу, когда догадка совпала с загаданным,
//сообщив, сколько попыток ушло.
//Уровень 2. Загадывает программа, причём в каждой партии по - новому.Догадку, которая не подходит под
//правила варианта, программа не засчитывает и просит ввести другую.После каждого хода на экран
//выводится таблица всех сделанных догадок с ответами на них.Если за двенадцать попыток игрок не
//угадал, программа печатает загаданное и заканчивает партию.
//Уровень 3. Программа играет сама с собой : загадывает последовательность, а затем отгадывает её,
//показывая каждый свой шаг.На каждом ходу она печатает свою догадку, ответ на неё и сколько
//последовательностей ещё подходят под все полученные ответы.Когда подходящая остаётся одна,
//программа называет её и сообщает, сколько ходов понадобилось, после чего сразу начинает новую партию
//с новым загаданным.Партии идут одна за другой без остановки, пока программу не закроют.Игрок здесь
//ничего не вводит и просто смотрит, как список подходящих раз за разом сжимается до единственной
//последовательности.
//
//Вариант 4 (мастермайнд).Загаданы четыре знака от 1 до 6, повторы разрешены.Ответ — быки и коровы,
//как в первом варианте, но считать коров труднее : если знак встречается в догадке чаще, чем в загаданном,
//лишние вхождения не засчитываются.Для загаданного 1 2 2 3 догадка 1 1 1 1 даёт 1 быка и 0 коров,
//догадка 1 2 3 4 — 2 и 1, а догадка 2 2 1 1 — 1 быка и 2 коровы, хотя единиц в ней две.Функция такая же, как
//в первом варианте.

#define MYSTERY_NUMBER_LENGTH 4
#define DIFFRENT_SIGNS_AMOUNT 6

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GameOnlyBulls.h"
#include "GameBullsAndCows.h"

#include <windows.h>


int check_guess(const int secret[], const int guess[], struct GameBullsOnly *game)
{
	int bulls =0, cows=0;
	


	int* bullsBySign = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	int* mysteryNumberBySign = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	



	//считаем bullsBySign - цифра на своём месте и mysteryNumberBySign состав загадки
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		mysteryNumberBySign[secret[i]]++;
		if (secret[i] == guess[i])
			bullsBySign[secret[i]]++;

	}

	//Считаем быков
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT; i++)
		bulls += bullsBySign[i];

	
	//Считаем коров. Если 
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		for (int j = 0; j < MYSTERY_NUMBER_LENGTH; j++)
		{
			if ((guess[j] == secret[i]) && (mysteryNumberBySign[secret[j]] > 0))
			{
				mysteryNumberBySign[secret[j]]--;

				if (bullsBySign[secret[j]] > 0)
					bullsBySign[secret[j]]--;
				else
					cows++;

				break;
			}
		}


	}

	int decade = 1;
	int guessNumber = 0;
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		guessNumber += decade * guess[MYSTERY_NUMBER_LENGTH-i-1];
	}
	game->guessHistory[game->move] = (struct GuessBullsOnly){ guessNumber, bulls, cows };

	free(bullsBySign);
	free(mysteryNumberBySign);


	return 10*bulls + cows;
}


int main()
{
	srand((unsigned)time(NULL));


	struct GameBullsOnly* game = calloc(1, sizeof(struct GameBullsOnly));

	
	///ПРОВЕРКИ ФУНКЦИЙ ИЗ ДЗ
	game->guessHistory = calloc((int)(DIFFRENT_SIGNS_AMOUNT + MYSTERY_NUMBER_LENGTH * (MYSTERY_NUMBER_LENGTH - 1)), (sizeof(struct GuessBullsOnly)));

	int secret[] = { 1,2,2,3 };
	int guess[] = { 1,1,1,1},
		guess2[] = {1,2,3,4},
		guess3[] = { 2,2,1,1 };
	printf_s("secret\tguess\n");
	printf_s("{ 1,2,2,3 }\t{ 1,1,1,1 }\t%i\n", check_guess(secret, guess, game) );
	printf_s("{ 1,2,2,3 }\t{ 1,2,3,4 }\t%i\n", check_guess(secret, guess2, game));
	printf_s("{ 1,2,2,3 }\t{ 2,2,1,1 }\t%i\n", check_guess(secret, guess3, game));

	free(game->guessHistory);

	struct GameBullsAndCows* gameBullsAndCows = calloc(1, sizeof(struct GameBullsAndCows));

	int possibleGuessesAmount = (int)(powl(DIFFRENT_SIGNS_AMOUNT, MYSTERY_NUMBER_LENGTH));

	for (int i = 0; i < 5; i++)
	{
		int secret = abs(rand() % possibleGuessesAmount);

		///Своя разработка
		printf_s("PLAY #%i\n\n", i+1);
		GameOnlyBullsLoop(game, secret);
		printf_s("\n\n\n");
		
		///По рекомендации
		GameBullsAndCowsLoop(gameBullsAndCows,secret);
		printf_s("\n\n\n");
		Sleep(2000);

	}



	printf_s("PREPARE TO SEE ALL POSSIBLE PLAYS");
	for (size_t i = 0; i < 3; i++)
	{
		printf_s(".");
		Sleep(2000);
	}
	printf_s("\n");


	for (int i = 0; i < 1296; i++) //1 296 = 6^4
	{
		GameBullsAndCowsLoop(gameBullsAndCows, i);
		printf_s("\n\n\n");



		if (i == 648)
		{
			printf_s("Half the way");
			for (size_t i = 0; i < 3; i++)
			{
				printf_s(".");
				Sleep(2000);
			}
		}

	}

	
	Sleep(4000);
	printf_s("My way:\n");

	for (int i = 0; i < 1296; i++) //1 296 = 6^4
	{
		GameOnlyBullsLoop(game, i);
		printf_s("\n\n\n");



		if (i == 648)
		{
			printf_s("Half the way");
			for (size_t i = 0; i < 3; i++)
			{
				printf_s(".");
				Sleep(2000);
			}
		}

	}


	return 0;
}