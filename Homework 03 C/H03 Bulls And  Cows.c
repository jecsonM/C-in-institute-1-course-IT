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

#include <windows.h>

struct Guess
{
	int guessNumber;
	int bulls;
	int cows;
};

struct Game {
	int bestGuess;  // 0000 -> 0204 лучшая догадка
	int knownMasc;    // 0000b -> 0101b какие битовые флаги о том, какие цифры известны
	int* knownComposition; // { 0, 0, 1, 0, 1, 0 } из каких цифр состоит

	int MysterNumber; //загаданное число
	struct Guess* guessHistory;
	int move;
	int* cows;
	int* bulls;
	int possibleAnswers;
	int maxMove;
};

int check_guess(const int secret[], const int guess[], struct Game *game)
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
	game->guessHistory[game->move] = (struct Guess){ guessNumber, bulls, cows };

	free(bullsBySign);
	free(mysteryNumberBySign);


	return 10*bulls + cows;
}

void makeGuess(struct Game* game, int guessNumber)
{

	int mysteryNumber, *cows, *bulls;
	cows = game->cows;
	bulls = game->bulls;
	mysteryNumber = game->MysterNumber;

	int* bullsBySign = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	int* mysteryNumberBySign = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	*cows = 0;



	//считаем bullsBySign и mysteryNumberBySign
	int digitPlace = 1;
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		int mysterySign = ((mysteryNumber / digitPlace) % DIFFRENT_SIGNS_AMOUNT);
		int guessSign = ((guessNumber / digitPlace) % DIFFRENT_SIGNS_AMOUNT);

		mysteryNumberBySign[mysterySign]++;
		if (mysterySign == guessSign)
			bullsBySign[mysterySign]++;

		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}

	*bulls = 0;
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT; i++)
		*bulls += bullsBySign[i];

	digitPlace = 1;
	//count cows 
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		int guessSign = ((guessNumber / digitPlace) % DIFFRENT_SIGNS_AMOUNT);
		int digitInMysteryPlace = 1;
		for (int j = 0; j < MYSTERY_NUMBER_LENGTH; j++)
		{
			int mysterySign = ((mysteryNumber / digitInMysteryPlace) % DIFFRENT_SIGNS_AMOUNT);
			if ((mysterySign == guessSign) && (mysteryNumberBySign[mysterySign] > 0))
			{
				mysteryNumberBySign[mysterySign]--;

				if (bullsBySign[mysterySign] > 0)
					bullsBySign[mysterySign]--;
				else
					(*cows)++;
				break;
			}
			digitInMysteryPlace *= DIFFRENT_SIGNS_AMOUNT;
		}

		
		
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;



	}

	game->guessHistory[game->move] = (struct Guess){guessNumber, *bulls, *cows};

	free(bullsBySign);
	free(mysteryNumberBySign);


	return;
}

void printNumber(int number)
{
	int signs[DIFFRENT_SIGNS_AMOUNT] = { '1', '2', '3', '4', '5', '6' };
	int digitPlace = 1;
	char* num_str = calloc(MYSTERY_NUMBER_LENGTH + 1, sizeof(char));
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		num_str[3 - i] = signs[((number / digitPlace) % DIFFRENT_SIGNS_AMOUNT)];
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}
	printf_s(num_str);
}

void LogMove(const struct Game* game, int toLogRemained)
{

	printf_s("Move %i:", game->move);
	printNumber(
		game->guessHistory[game->move].guessNumber
	);
	if (toLogRemained)
	{
		printf_s("\t  bulls:%i cows:%i\tremained: %i\n", game->guessHistory[game->move].bulls, game->guessHistory[game->move].cows, game->possibleAnswers);
	}
	else
		printf_s("\t  bulls:%i cows:%i\t", game->guessHistory[game->move].bulls, game->guessHistory[game->move].cows);
}

//Наименьшее из цифр
int makeLowestNumber(int* knownComposition)
{
	int digitPlace = 1;
	int number = 0;
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT; i++)
		{
			if (knownComposition[DIFFRENT_SIGNS_AMOUNT - 1 - i] > 0)
			{
				number += digitPlace *( DIFFRENT_SIGNS_AMOUNT - 1 - i);
				knownComposition[DIFFRENT_SIGNS_AMOUNT - 1 - i]--;
				break;
			}
		}
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}
	return number;
}


int CheckIfTwoSignsAreSame(int number, int indI, int indJ)
{
	
	indI = MYSTERY_NUMBER_LENGTH - indI - 1;
	indJ = MYSTERY_NUMBER_LENGTH - indJ - 1;

	if (indI > indJ)
	{
		int m = indI;
		indI = indJ;
		indJ = m;
	}

	int digitPlace = 1;
	for (int i = 0; i < indI; i++)
	{
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}
	int signI = (number / digitPlace) % DIFFRENT_SIGNS_AMOUNT;

	
	for (int i = indI; i < indJ; i++)
	{
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}
	int signJ = (number / digitPlace) % DIFFRENT_SIGNS_AMOUNT;
	

	return signI == signJ;
}

int SwapTwoSigns(int number, int indI, int indJ)
{
	indI = MYSTERY_NUMBER_LENGTH - indI-1;
	indJ = MYSTERY_NUMBER_LENGTH - indJ-1;

	if (indI > indJ)
	{
		int m = indI;
		indI = indJ;
		indJ = m;
	}

	int digitPlaceI = 1;
	for (int i = 0; i < indI; i++)
	{
		digitPlaceI *= DIFFRENT_SIGNS_AMOUNT;
	}
	int signI = (number / digitPlaceI) % DIFFRENT_SIGNS_AMOUNT;
	int IwDecade = digitPlaceI * signI;

	int digitPlaceJ = digitPlaceI;
	for (int i = indI; i < indJ; i++)
	{
		digitPlaceJ *= DIFFRENT_SIGNS_AMOUNT;
	}
	int signJ = (number / digitPlaceJ) % DIFFRENT_SIGNS_AMOUNT;
	int JwDecade = digitPlaceJ * signJ;

	number = number - JwDecade;
	number = number - IwDecade;
	number += signI * digitPlaceJ;
	number += signJ * digitPlaceI;

	return number;
}

void makeMove(struct Game* game, int toLogRemained)
{
	makeGuess(game, game->bestGuess);

	
	if (toLogRemained)
		game->possibleAnswers--;
	if (*game->bulls == 4) game->possibleAnswers = 1;
	
	LogMove(game, toLogRemained);
	(game->move)++;
}

int IsAlreadyGuessed(struct Game* game, int guessNumber)
{
	int isAlreadyGuessed = 0;
	for (int i = 0; i < game->move & !isAlreadyGuessed; i++)
	{
		isAlreadyGuessed |= game->guessHistory->guessNumber == guessNumber;
	}

	return isAlreadyGuessed;
}

int CalculatePossibilities(struct Game *game, int alreadyChecked)
{
	int knownAmount = 0;
	
	for (int i = 0; i < alreadyChecked; i++) 
		knownAmount += game->knownComposition[i];
	

	int freePos = MYSTERY_NUMBER_LENGTH - knownAmount,
		unknownSigns = DIFFRENT_SIGNS_AMOUNT - alreadyChecked;

	int possibilities = 1;
	for (int i = 2; i < MYSTERY_NUMBER_LENGTH+1; i++) // possiblities = MYSTERY_NUMBER_LENGTH!
	{
		possibilities *= i;
	}

	for (int i = 0; i < alreadyChecked; i++)
	{
		for (int j = 2; j < game->knownComposition[i] +1; j++) 
		{
			possibilities = possibilities / j; //posibilities = posibilities / (knownComposition[i])!
		}
	}
	for (int i = 2; i < freePos+1; i++)
	{
		possibilities = possibilities / i; // posibilities / freePos!
	}

	possibilities *= (int)powl(unknownSigns, freePos); //Домножаем на оставшиеся неизв. числа


	return possibilities;
}

int GameLoop(struct Game* game, int mysteryNumber)
{
	game->knownComposition = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	game->guessHistory = calloc((int)(DIFFRENT_SIGNS_AMOUNT + MYSTERY_NUMBER_LENGTH * (MYSTERY_NUMBER_LENGTH-1)), (sizeof(struct Guess)));
	game->bulls = calloc(1, sizeof(int));
	game->cows = calloc(1, sizeof(int));

	game->move = 0;
	game->knownMasc = 0;
	game->possibleAnswers = (int)powl(DIFFRENT_SIGNS_AMOUNT, MYSTERY_NUMBER_LENGTH);

	int won = 0;

	
	game->MysterNumber = mysteryNumber; 
	


	printf_s("Mystery Number is: ");
	printNumber(game->MysterNumber);
	printf_s("\t remained answers: %i", game->possibleAnswers);
	printf_s("\n");


	//Определяем состав загаданного числа


	//известное кол-во цифр
	int knownCompAmount = 0;
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT - 1; i++)
	{
		game->bestGuess = MakeNumberAllOneSign(i);
		makeMove(game,0);

		if (game->bulls != 0)
		{
			game->knownComposition[i] = *game->bulls;
			knownCompAmount += *game->bulls;
		}
		if (i == DIFFRENT_SIGNS_AMOUNT - 2)
		{
			game->knownComposition[DIFFRENT_SIGNS_AMOUNT - 1] = MYSTERY_NUMBER_LENGTH - knownCompAmount;
			knownCompAmount += game->knownComposition[DIFFRENT_SIGNS_AMOUNT - 1];
			i++;
		}

		game->possibleAnswers = CalculatePossibilities(game, i+1);
		printf_s("remained: %i\n", game->possibleAnswers);
		if (knownCompAmount >= MYSTERY_NUMBER_LENGTH)
			break;
	}

	

	//Знаем состав, теперь собираем число
	game->bestGuess = makeLowestNumber(game->knownComposition);

	makeMove(game,1);

	if (*game->bulls == MYSTERY_NUMBER_LENGTH)
		won = 1;



	
		
	//Нужно начать с числа bull % 2 ==0
	for (
		int i = 0, nextIndDis = 1;
		(i + nextIndDis < MYSTERY_NUMBER_LENGTH) & *game->bulls % 2 != 0;
		i = i + nextIndDis, nextIndDis = 1
		)
	{
		while (
			( ((1 << i + nextIndDis) & game->knownMasc) |						//Уже найденные цифры не трогаем
			CheckIfTwoSignsAreSame(game->bestGuess, i, i + nextIndDis) ) &		//Одинаковые цифры скипаем
			(i + nextIndDis < MYSTERY_NUMBER_LENGTH - 1)						//Не уходим за пределы числа
		)
			nextIndDis++;

		game->bestGuess = SwapTwoSigns(game->bestGuess, i, i + nextIndDis); //Перебор пузырьком 
		if (IsAlreadyGuessed(game, game->bestGuess)) //Не повторяться, если уже спрашивал число
			continue;

		makeMove(game, 1);

		if (*game->bulls >= MYSTERY_NUMBER_LENGTH)
			won = 1;

	}
	int baseGuess = game->bestGuess; //Если 2 быка, то число в 1 перестановке от загаданного, если 0, то в 1-ой ил 2-ух 
	int baseBulls = *game->bulls;
	int lastBulls = *game->bulls;
	int bullDiff;
	
	

	for (int j = 0; (j < MYSTERY_NUMBER_LENGTH-1) & !won; j++)
	{
		for (
			int i = j+1;
			(i < MYSTERY_NUMBER_LENGTH) & !won;
			i++
			)
		{



			while ((1 << i) & game->knownMasc) //Уже найденные цифры не трогаем
				i++;

			if (i >= MYSTERY_NUMBER_LENGTH) 			//Не уходим за пределы числа
				break;

			game->bestGuess = SwapTwoSigns(baseGuess, j, i); //Перебор пузырьком 

			if (IsAlreadyGuessed(game, game->bestGuess)) //Не повторяться, если уже спрашивал число
				continue;
			makeMove(game, 1);

			if (*game->bulls >= MYSTERY_NUMBER_LENGTH)
				won = 1;

			bullDiff = *game->bulls - lastBulls;
			lastBulls = *game->bulls;

			if (!won)
			{
				switch (bullDiff)
				{
				case -2:
					game->bestGuess = SwapTwoSigns(game->bestGuess, j, i);
					lastBulls += 2;
				case 2:
					game->knownMasc |= 1 << j;
					game->knownMasc |= 1 << i;
					

					if (lastBulls + 2 >= MYSTERY_NUMBER_LENGTH)
						won = 1;
					break;
				case 1:
				case -1:
					
					//Плохая база при baseBulls=0
					if (!baseBulls)
					{
						while (*game->bulls % 2 != 0)
						{
							for (int swapInd = 0; swapInd < MYSTERY_NUMBER_LENGTH - 1 & !baseBulls; swapInd++)
								baseGuess = SwapTwoSigns(baseGuess, swapInd, swapInd + 1);

							game->bestGuess = baseGuess;
							makeMove(game, 1);
							if (*game->bulls >= MYSTERY_NUMBER_LENGTH)
								won = 1;
							lastBulls = *game->bulls;
						}
						baseBulls = *game->bulls;
						i = j;
					}
					

					break;
				}
			}

		}
	}
	

	
	


	if (lastBulls < MYSTERY_NUMBER_LENGTH)
	{
		int indI = -1, indJ = -1;
		for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
			if (!(1 << i & game->knownMasc)) //Если 0 на позиции
			{
				indI = i;
				break;
			}

		for (int i = indI + 1; i < MYSTERY_NUMBER_LENGTH; i++)
			if (!(1 << i & game->knownMasc))
			{
				indJ = i;
				break;
			}
			game->bestGuess = SwapTwoSigns(game->bestGuess, indI, indJ);
		makeMove(game, 1);
	}
	


	



	if (game->maxMove < game->move)
		game->maxMove = game->move;
		
	

	free(game->knownComposition);
	free(game->guessHistory);
	free(game->bulls);
	free(game->cows);
	return;
}





int MakeNumberAllOneSign(int sign)
{
	int decade = 1;
	int NumberAllOneSign = 0;
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		NumberAllOneSign += sign * decade;
		decade *= DIFFRENT_SIGNS_AMOUNT;
	}
	return NumberAllOneSign;
}

int main()
{
	srand((unsigned)time(NULL));


	struct Game* game = calloc(1, sizeof(struct Game));
	game->guessHistory = calloc((int)(DIFFRENT_SIGNS_AMOUNT + MYSTERY_NUMBER_LENGTH * (MYSTERY_NUMBER_LENGTH - 1)), (sizeof(struct Guess)));
	///ПРОВЕРКИ ФУНКЦИЙ ИЗ ДЗ
	
	int secret[] = { 1,2,2,3 };
	int guess[] = { 1,1,1,1},
		guess2[] = {1,2,3,4},
		guess3[] = { 2,2,1,1 };
	printf_s("secret\tguess\n");
	printf_s("{ 1,2,2,3 }\t{ 1,1,1,1 }\t%i\n", check_guess(secret, guess, game) );
	printf_s("{ 1,2,2,3 }\t{ 1,2,3,4 }\t%i\n", check_guess(secret, guess2, game));
	printf_s("{ 1,2,2,3 }\t{ 2,2,1,1 }\t%i\n", check_guess(secret, guess3, game));

	free(game->guessHistory);

	///Своя разработка
	
	for (int i = 0; i < 5; i++)
	{
		printf_s("PLAY #%i\n\n", i+1);
		GameLoop(game, abs(rand()) % (int)(powl(DIFFRENT_SIGNS_AMOUNT, MYSTERY_NUMBER_LENGTH)) );
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
		GameLoop(game, i);
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