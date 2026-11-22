#include "cipher4.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include <windows.h>

#define TEXT_SIZE 1500
#define ALPHABET_SIZE 26 
//'z' - 'a'+1 | 122-97 +1 = 2, ну или просто 26 б. в англ. алфавите

static double LETTER_FREQUENCIES[26] = {
	8.2, 1.5, 2.8, 4.3, 12.7, 2.2, 2.0, 6.1, 
	7.0, 0.15, 0.77, 4.0, 2.4, 6.7, 7.5, 1.9, 
	0.095, 6.0, 6.3, 9.1, 2.8, 0.98, 2.4, 0.15, 
	2.0, 0.074
};

int check_text(const char text[])
{
	for (int i = 0; i < TEXT_SIZE; i++)
	{
		if (!text[i]) // '\0'
			return 1;
		if (text[i] < 'a' || text[i] > 'z')
			return 0;
	}

	return 0;
};


int keyIndexer(int flatI, int keySize)
{
	int bigPeriod = 2 * keySize - 2;
	if (!bigPeriod) return 0;

	int j = flatI % bigPeriod;
	return j < keySize ? j : bigPeriod - j;
}

void encryprDecrypt(const char text[], const char key[], char out[], int toEncrypt)
{
	if (!check_text(key) || !check_text(text))
		return 1;

	int keySize = 0;
	for (int i = 0; i < TEXT_SIZE; i++)
		if (key[i] == '\0')
		{
			keySize = i;
			break;
		};

	char* keyDiffs = malloc(keySize);
	if (toEncrypt)
		for (int i = 0; i < keySize; i++)
			keyDiffs[i] = key[i] - 'a';
	else
		for (int i = 0; i < keySize; i++)
			keyDiffs[i] =  'a'+ ALPHABET_SIZE - key[i];


	for (int i = 0; i < TEXT_SIZE; i++)
	{
		if (text[i] == '\0')
		{
			out[i] = '\0';
			break;
		}

		int j = keyIndexer(i, keySize);

		out[i] = 'a' + ((keyDiffs[j] + (text[i] - 'a')) % ALPHABET_SIZE);
	}

	free(keyDiffs);
}

void encrypt(const char text[], const char key[], char out[])
{
	encryprDecrypt(text, key, out, 1);
};

void decrypt(const char text[], const char key[], char out[])
{
	encryprDecrypt(text, key, out, 0);
};

void count_letters(const char text[], int cnt[26])
{
	for (int i = 0; i < 26; i++) cnt[i] = 0;

	int i = 0;
	while (text[i] != '\0')
	{
		cnt[text[i] - 'a']++;
		i++;
	}
};
//char crack_caesar(const char text[])
//{
//	if (!check_text(text))
//		return 1;
//
//	int cnt[26];
//	count_letters(text, cnt);
//	double frequencies[26] = { 0 };
//	{
//		int amount = 0;
//		for (int i = 0; i < 26; i++) amount += cnt[i];
//		for (int i = 0; i < 26; i++) frequencies[i] = cnt[i]*100.0 /amount;
//	}
//	
//
//	double distances[26] = { 0 };
//	{
//		int minDisInd = 0;
//		double minDistance = DBL_MAX;
//
//		for (int s = 0; s < 26; s++)
//		{
//			for (int c = 0; c < 26; c++)
//			{
//				distances[s] += pow(frequencies[c] - LETTER_FREQUENCIES[(c - s) % 26], 2);
//			}
//
//			if (distances[s] < minDistance)
//			{
//				minDistance = distances[s];
//				minDisInd = s;
//			}
//		}
//
//
//		return 'a' + minDisInd;
//	}
//};


char crack_caesar(const char text[])
{
	if (!check_text(text))
		return 1;

	int cnt[26];
	count_letters(text, cnt);
	double frequencies[26] = { 0 };
	{
		int amount = 0;
		for (int i = 0; i < 26; i++) amount += cnt[i];
		for (int i = 0; i < 26; i++) frequencies[i] = cnt[i]*100.0 /amount;
	}

	double similiarities[26] = { 0 },
		distances[26] = { 0 };

	char* charPeaks = malloc(13);


	{
		double maxSimilarity = 0;
		double minDistance = DBL_MAX;

		for (int s = 0; s < 26; s++)
		{
			for (int c = 0; c < 26; c++)
			{
				similiarities[s] += cnt[c] * LETTER_FREQUENCIES[(26 + c - s) % 26];
				distances[s] += pow(frequencies[c] - LETTER_FREQUENCIES[(26 +c - s) % 26], 2);
			}

			if (similiarities[s] > maxSimilarity)
				maxSimilarity = similiarities[s];
			if (distances[s] < minDistance)
				minDistance = distances[s];
		}



		for (int i = 0; i < 26 - 1; i++)
		{

			if (
				(distances[i] < distances[i + 1])
				&& (similiarities[i] > (maxSimilarity * 0.5) )
				&& (distances[i] < (minDistance * 1.15) )
				)
			{
				return 'a' + i;
			}


		}
		return 'z';
	}
};


char* crack_caesar_with_options(const char text[])
{
	if (!check_text(text))
		return 1;

	int cnt[26];
	count_letters(text, cnt);

	double similiarities[26] = { 0 };

	char *charPeaks = malloc(13);

	
	{
		double maxSimilarity = 0;
		for (int s = 0; s < 26; s++)
		{
			for (int c = 0; c < 26; c++)
			{
				similiarities[s] += cnt[c] * LETTER_FREQUENCIES[(c - s) % 26];
			}
			if (similiarities[s] > maxSimilarity)
				maxSimilarity = similiarities[s];
		}

		int peakInd = 0;
		int ascending = 1 ;
		for (int i = 0; i < 26-1; i++)
		{
			
			if (ascending && (similiarities[i] > similiarities[i + 1]) && (similiarities[i] > maxSimilarity * 0.8))
			{
				charPeaks[peakInd] = 'a'+i;
				peakInd++;
				ascending = 0;
				continue;
			}

			if (!ascending && (similiarities[i] < similiarities[i + 1]))
			{
				ascending = 1;
				continue;
			}

		}
		if (ascending && (similiarities[25] > maxSimilarity * 0.8))
		{
			charPeaks[peakInd] = 'a' + 26-1;
			peakInd++;
		}

		

		
		
		charPeaks[peakInd] = '\0';
		
	}

	return charPeaks;

};

void crack_vigenere(const char text[], int keylen, char key[])
{
	int smallerTextsSize = TEXT_SIZE / (keylen-1) + 1;
	char **smallerTexts = malloc(keylen*sizeof(char*));
	int* smallerTextInds = calloc(keylen, sizeof(int));

	for (int i = 0; i < keylen; i++)
		smallerTexts[i] = malloc(smallerTextsSize);
	

	for (int i = 0; i < TEXT_SIZE; i++)
	{
		if (!text[i])
		{
			for (int j = 0; j < keylen; j++) 
				smallerTexts[j][smallerTextInds[j]] = '\0';
			break;
		}
		int j = keyIndexer(i, keylen);
		smallerTexts[j][ smallerTextInds[j] ] = text[i];
		smallerTextInds[j]++;
	}

	for (int i = 0; i < keylen; i++)
		key[i] = crack_caesar(smallerTexts[i]);


	for (int i = 0; i < keylen; i++)
		free(smallerTexts[i]);
	free(smallerTextInds);
	free(smallerTexts);
};


int main()
{
	
	char text[TEXT_SIZE-99] = "attackatdawn",
		encryptedText[TEXT_SIZE-99] = "",
		key[TEXT_SIZE-99] = "lemon";

	printf_s("Is \"Attack at Dawn\" valid? : %d \n", check_text("Attack at Dawn"));
	printf_s("Is \"%s\" valid? : %d \n\n\n", text, check_text(text));


	encrypt(text, key, encryptedText);

	printf_s("Was:\n \"%s\"\nEncrypted:\n\"%s\"\t\tshould be \"lxfopymxoeib\"\n\n", text, encryptedText);


	decrypt(encryptedText, key, text);
	printf_s("Was:\n \"%s\"\nDecrypted:\n\"%s\"\t\tshould be \"attackatdawn\"\n\n", encryptedText, text);


	printf_s("------PART 2-----\n\n");

	{

		char key_char[2] = { 'a', '\0' };
		key_char[0] = crack_caesar(CAESAR);

		decrypt(CAESAR, key_char, text);


		printf_s("Was:\n \"%s\"\n", CAESAR);

		int amount = 0;
		int aAmount = 0;
		int eAmount = 0;

		for (int i = 0; i < TEXT_SIZE; i++)
		{
			if (!CAESAR[i])
			{
				amount = i;
				break;
			}
			switch (CAESAR[i])
			{
			case 'a':
				aAmount++;
				break;
			case 'e':
				eAmount++;
				break;
			}
		}

		printf_s("Characters: %d\ta: %d\te: %d\n", amount, aAmount, eAmount);
		printf_s("\nDecrypted:\n\"%s\"\nwith: \"%s\"\n ", text, key_char);

		/*printf_s("------BRUTEFORCE-----\n\n");
		for (char i = 0; i < 26; i++)
		{
			key_char[0] = 'a' + i;
			decrypt(CAESAR, key_char, text);
			printf_s("Decrypted:\n\"%s\"\n with: \"%s\"\n\n", text, key_char);
		}
		printf_s("------BRUTEFORCE-----\n\n");*/

		/*printf_s("------crack_caesar_with_options-----\n\n");
		char* keyOptions = crack_caesar_with_options(CAESAR);
		for (int i = 0; i < 13; i++)
		{
			if (!keyOptions[i])
				break;
			key_char[0] = keyOptions[i];

			decrypt(CAESAR, key_char, text);

			printf_s("Decrypted:\n\"%s\"\n with: \"%s\"\n\n", text, key_char);
		}
		printf_s("------crack_caesar_with_options-----\n\n");*/
	}

	{
		//Мой пример из интернета
		char text2[] = "hifreditsbeenawhilesincewehavebeenintouchhowhasyoursemesterbeeniwanttosendanemailupdatetoyouletyouknowhowthingshavebeengoingduringmysemesterabroadhereinmalagaspainivealreadybeenhereforsixweeksandifeellikeiamfinallyadaptingtothecultureimalsospeakingthelanguagemorefluentlyiarrivedduringthefirstweekofseptembertheweatherhasbeenveryniceeventhoughitsoctoberitsstillrathersunnyandwarminfactiwenttothebeachandswaminthemediterraneanseaearliertodayiamlivingwithaverywelcominghostfamilyihavemyownprivatebedroombutweeatbreakfastlunchanddinnertogetheronsundaysweeatabighomecookedpaellaforlunchinspainlunchisusuallythebiggestmealofthedayitsalsoverycommonforthepeopletotakeamiddaynaprightafterabigmealiamactuallyjustwakingupfrommynaprightnowonweekdaysitakeclassesatthelocaluniversitythereimetseveralnativespanishspeakerstheyhavebeenverykindandpatientwithmeatfirstistruggledtocomprehendtheirspanishbutnowiunderstandmostofourconversationstheyhavecommentedthatmyspanishhasimprovedalotsincewefirstmetnowiammoreconfidenttousethelanguageinotherplaceslikestoresandrestaurantsiamsogladthatidecidedtospendthesemesterhereinspainwehaveanextendedweekendcomingupsoagroupofmyfriendsandiaregoingtotraveltofranceforfourdaysitssoeasyandinexpensivetotravelinternationallyineuropeiloveitilookforwardtohearingfromyousoonlikeisaiddonthesitatetostayintouchmoreoftenperhapsyoucouldevencometovisitwhatdoyouthink";
		char key_char[2] = { 'y', '\0' };
		encrypt(text2, key_char, encryptedText);

		printf_s("\n\nEncrypted:\n\"%s\"\n\n ", encryptedText);

		key_char[0] = crack_caesar(encryptedText);

		decrypt(encryptedText, key_char, text2);
		printf_s("with: \"%s\"\nDecrypted:\n\"%s\"\n ", key_char, text2);

	}




	printf_s("------PART 3-----\n\n");

	srand((unsigned)time(NULL));

	char key_vigenere[5] = "aaaa";
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < KEY_LEN; j++) key_vigenere[j] = rand() % ALPHABET_SIZE + 'a';

		printf_s("key generated: \"%s\"\n ", key_vigenere);
		encrypt(SAMPLE, key_vigenere, encryptedText);

		crack_vigenere(encryptedText, KEY_LEN, key_vigenere);

		decrypt(encryptedText, key_vigenere, text);

		printf_s("with: \"%s\"\nDecrypted:\n\"%s\"\n\n", key_vigenere, text);
	}


	printf_s("------DECRYPT-----\n\n");
	{
		for (int j = 0; j < KEY_LEN; j++) key_vigenere[j] = rand() % ALPHABET_SIZE + 'a';

		crack_vigenere(VIGENERE, KEY_LEN, key_vigenere);
		decrypt(VIGENERE, key_vigenere, text);

		printf_s("with: \"%s\"\nDecrypted:\n\"%s\"\n\n", key_vigenere, text);
	}

}