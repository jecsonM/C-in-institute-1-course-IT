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
	int knownNumber;  // 0000 -> 0204 лучшая догадка
	int knownMasc;    // 0000b -> 0101b какие битовые флаги о том, какие цифры известны
	int* knownComposition; // {0, ..., 0} -> { 0, 0, 1, 0, 1, 0 } из каких цифр состоит

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


	//count cows 
	for (int i = 0; i < MYSTERY_NUMBER_LENGTH; i++)
	{
		int mysterySign = ((mysteryNumber / digitPlace) % DIFFRENT_SIGNS_AMOUNT);
		int guessSign = ((guessNumber / digitPlace) % DIFFRENT_SIGNS_AMOUNT);

		if (mysterySign == guessSign && (mysteryNumberBySign[mysterySign]  > 0) )
		{
			mysteryNumberBySign[mysterySign]--;

			if(bullsBySign[mysterySign] > 0)
				bullsBySign[mysterySign]--;
			else
 				(*cows)++;
		}
		
		digitPlace *= DIFFRENT_SIGNS_AMOUNT;
	}

	game->guessHistory[*game->move] = (struct Guess){guessNumber, *bulls, *cows};



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

int GameLoop(struct Game *game)
{
	game->MysterNumber = abs(rand()) % 1296; // 6^4 = 1296
	printf_s("Mystery Number is: ");
	printNumber(game->MysterNumber);
	printf_s("\n");


	//Определяем состав загаданного числа
	int knownCompAmount = 0;
	for (int i = 0; i < DIFFRENT_SIGNS_AMOUNT-1; i++)
	{
		makeGuess(game, MakeNumberAllOneSign(i));
		LogMove(game);

		(*game->move)++;
		if (game->bulls != 0)
		{
			game->knownComposition[i] = *game->bulls;
			knownCompAmount += *game->bulls;
		}
		if (knownCompAmount >= MYSTERY_NUMBER_LENGTH)
			break;
	}
	if(MYSTERY_NUMBER_LENGTH > knownCompAmount)
		game->knownComposition[DIFFRENT_SIGNS_AMOUNT - 1] = MYSTERY_NUMBER_LENGTH - knownCompAmount;
	
	///////ТУТ БУДЕТ ПЕРЕБО ПО СОСТАВУ ЧИСЛА
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



	

	int* bulls = calloc(1, sizeof(int));
	int *cows = calloc(1, sizeof(int));
	int *move = calloc(1,sizeof(int));
	struct Guess *guessHistory = calloc( (int)(MYSTERY_NUMBER_LENGTH * log2(DIFFRENT_SIGNS_AMOUNT) +2), (sizeof(struct Guess))); 
	int* knownComposition = calloc(DIFFRENT_SIGNS_AMOUNT, sizeof(int));

	struct Game* game = calloc(1, sizeof(struct Game));
	game->knownComposition = knownComposition;
	game->guessHistory = guessHistory;
	game->bulls = bulls;
	game->cows = cows;
	game->move = move;

	

	int guessNumber = 0;
	
	

	GameLoop(game);


	return 0;
}