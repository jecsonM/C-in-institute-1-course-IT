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
	int* move;
	int* cows;
	int* bulls;
};



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

	game->guessHistory[*game->move] = (struct Guess){guessNumber, *bulls, *cows};

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

void LogMove(const struct Game* game)
{

	printf_s("Move %i:",*game->move);
	printNumber(
		game->guessHistory[*game->move].guessNumber
		);
	printf_s("\t  bulls:%i cows:%i\n", game->guessHistory[*game->move].bulls, game->guessHistory[*game->move].cows);
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


//i и j
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

//i и j
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

void makeMove(struct Game* game)
{
	makeGuess(game, game->bestGuess);
	LogMove(game);
	(*game->move)++;
}



int GameLoop(struct Game *game)
{
	game->knownComposition = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));
	game->guessHistory = calloc((int)(MYSTERY_NUMBER_LENGTH * log2(DIFFRENT_SIGNS_AMOUNT) + 2), (sizeof(struct Guess)));
	game->bulls = calloc(1, sizeof(int));
	game->cows = calloc(1, sizeof(int));
	game->move = calloc(1, sizeof(int));

	int won = 0;

	game->MysterNumber = abs(rand()) % 1296; // 6^4 = 1296
	game->MysterNumber = 368;
	

	printf_s("Mystery Number is: ");
	printNumber(game->MysterNumber);
	printf_s("\n");


	//Определяем состав загаданного числа

	//известное кол-во цифр
	int knownCompAmount = 0; 
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT-1; i++)
	{
		game->bestGuess = MakeNumberAllOneSign(i);
		makeMove(game);

		if (game->bulls != 0)
		{
			game->knownComposition[i] = *game->bulls;
			knownCompAmount += *game->bulls;
		}
		if (knownCompAmount >= MYSTERY_NUMBER_LENGTH)
			break;
	}

	//Для последней цифры исключением
	if(MYSTERY_NUMBER_LENGTH > knownCompAmount)
		game->knownComposition[DIFFRENT_SIGNS_AMOUNT - 1] = MYSTERY_NUMBER_LENGTH - knownCompAmount;
	


	//Знаем состав, теперь собираем число
	game->bestGuess = makeLowestNumber(game->knownComposition);


	makeMove(game);
	if (*game->bulls == MYSTERY_NUMBER_LENGTH)
		won = 1;


	
	


	int lastBulls = *game->bulls;
	int indOfFirstUndetermined = -1; //При изменении на 1, -1, запоминаем i неопределённого числа; -1, чтоб было видно, что это ещё не определено
	int indOfSecondUndetermined = -1;
	int bullDiff;
	for (int j = 0; (j < MYSTERY_NUMBER_LENGTH - 2) & !won; j++)
	{
		

		for (int i = 0, nextIndDis = 1; (i < MYSTERY_NUMBER_LENGTH - 1) & !won;  nextIndDis = 1 )
		{
			


			while (
				( (1 << i + nextIndDis) & game->knownMasc) |						//Уже найденные цифры не трогаем
				CheckIfTwoSignsAreSame(game->bestGuess, i, i + nextIndDis) &		//Одинаковые цифры скипаем
				( (i+ nextIndDis) < MYSTERY_NUMBER_LENGTH - 1 )						//Не уходим за пределы числа
			) 
				nextIndDis++;

			game->bestGuess = SwapTwoSigns(game->bestGuess, i, i + nextIndDis); //Перебор пузырьком 

			makeMove(game);
			if (*game->bulls == MYSTERY_NUMBER_LENGTH)
				won = 1;

			bullDiff = *game->bulls - lastBulls;



			switch (bullDiff)
			{

			case -2:
				game->bestGuess = SwapTwoSigns(game->bestGuess, i, i + nextIndDis); //Возвращаем 
			case 2:
				lastBulls = *game->bulls;

				game->knownMasc = game->knownMasc | (1 << i);
				game->knownMasc = game->knownMasc | (1 << i + nextIndDis); //Отмечаем что эти два числа на месте
				break;

			case -1:
				lastBulls = *game->bulls;

				if (indOfFirstUndetermined < 0) //Первый раз возникла неопределённость
				{
					indOfFirstUndetermined = i;
					indOfSecondUndetermined = i + nextIndDis;
				}
				else
				{
					game->bestGuess = SwapTwoSigns(game->bestGuess, i, i + nextIndDis); //Возвращаем
					game->knownMasc = game->knownMasc | (1 << i); //Отмечаем что цифра на месте
					indOfFirstUndetermined = -1;
					lastBulls++;
				}

				break;
			case 1:
			{
				lastBulls = *game->bulls;

				if (indOfFirstUndetermined < 0) //Первый раз возникла неопределённость
				{
					indOfFirstUndetermined = i;
					indOfSecondUndetermined = i + nextIndDis;
				}
				else
				{
					game->knownMasc = game->knownMasc | (1 << i+nextIndDis);

					if (bullDiff == 1) //Неопределённость между i и i+nextInd
					{
						indOfFirstUndetermined = i;
						indOfSecondUndetermined = i+ nextIndDis;
					}
				}

				break;
			}
			case 0:
				lastBulls = *game->bulls;

				if (indOfFirstUndetermined >= 0)
				{
					game->knownMasc = game->knownMasc | (1 << indOfFirstUndetermined);
					indOfFirstUndetermined = -1; //неопределённость разрешена
					/*nextUndeterminedIndDis = 0;*/
				}

				break;


			}

			i = i + nextIndDis;

		}
		//if (indOfFirstUndetermined >= 0) //Если на последней цифре осталась неопределённость
		//{
		//	game->bestGuess = SwapTwoSigns(game->bestGuess, indOfFirstUndetermined, indOfSecondUndetermined); //Возвращаем
		//	if(bullDiff > 0)
		//		game->knownMasc = game->knownMasc | (1 << indOfFirstUndetermined); //Отмечаем что последняя цифра на месте
		//	else
		//		game->knownMasc = game->knownMasc | (1 << indOfSecondUndetermined); //Отмечаем что предпоследняя цифра на месте
		//	indOfFirstUndetermined = -1;
		//	lastBulls++;
		//}

	}
	if (!won) //Ищем два оставшихся не опознанных числа
	{
		int indI, indJ;
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
		makeMove(game);
	}
	



		
		


	free(game->knownComposition);
	free(game->guessHistory);
	free(game->bulls);
	free(game->cows);
	free(game->move);
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
	

	

	int guessNumber = 0;
	
	
	for (int i = 0; i < 15; i++)
	{
		GameLoop(game);
		printf_s("\n\n\n");

	}



	return 0;
}