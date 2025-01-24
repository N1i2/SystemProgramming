#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int Pos(const char* s, const char* sl);
int PosWithPointers(const char* s, const char* sl);
void ShowResul(int resultNumber);

int main() {
	const char* firstWords[10] = {
		/*1*/	"Hello World",
		/*2*/	"nanananannananananananananananananana",
		/*3*/	"le",
		/*4*/	"123456789101112131415161718192021222324252627282930313233343536373839404738465347865378629865293874289735984682937957983247289589634793289478563812678647632784632784983297483972724398723895683274584659239847249865238759867239847897328567328710937374863843",
		/*5*/	"privet",
		/*6*/	"HahaHaha",
		/*7*/	"Juni@r",
		/*8*/	"hello",
		/*9*/	"first",
		/*10*/	""
	};

	const char* secondWords[10] = {
		/*1*/	" Wo",
		/*2*/	"blablabla",
		/*3*/	"leonid",
		/*4*/	"22232",
		/*5*/	"vets",
		/*6*/	"HaH",
		/*7*/	"@",
		/*8*/	"lo",
		/*9*/	"",
		/*10*/	"second"
	};

	for (int i = 0; i < sizeof(firstWords) / sizeof(firstWords[0]); i++) {
		printf("%d)%s\n%s\n",
			i + 1,
			firstWords[i],
			secondWords[i]);

		ShowResul(Pos(firstWords[i], secondWords[i]));
		//ShowResul(PosWithPointers(firstWords[i], secondWords[i]));
	}

	/*
	char firstWorld[256], secondWorld[256];
	int check = 0;

	printf("First String: ");
	fgets(firstWorld, sizeof(firstWorld), stdin);
	printf("Second String: ");
	fgets(secondWorld, sizeof(secondWorld), stdin);
	printf("Number < 0; Simvl == other: ");
	scanf("%d", &check);

	int size = Pos(firstWorld, secondWorld, check);
	printf("%d\n", size);
	*/

	system("pause");
	return 0;
}

int Pos(const char* s, const char* sl) {
	int i = 0, j = 0;

	if (strlen(s) <= 0)
	{
		return -1; //строка пустая
	}
	if (strlen(s) >= 255) {
		return -2; //строка слишком большая
	}
	if (strlen(sl) <= 0) {
		return -3; //подстрока пустая
	}
	if (strlen(sl) > strlen(s)) {
		return -4; // подстрока длиннее строки
	}


	for (i = 0; i <= strlen(s) - strlen(sl); i++) {
		for (j = 0; j < strlen(sl); j++) {
			if (s[i + j] != sl[j]) {
				break;
			}
		}
		if (j == strlen(sl)) {
			return i + 1; //номер первого символа подстроки в строке
		}
	}

	return 0; // случае если в строке нет подстроки
}

void ShowResul(int resultNumber)
{
	const char* failAnsver[5] = { "Substring is longer than string","Substring is empty","String is too long(limit 254)","String is empty","String does not contain substring" };

	if (resultNumber > 0) {
		printf("%d\n\n", resultNumber);
	}
	else {
		printf("%s\n\n", failAnsver[resultNumber + 4]);
	}
}

int PosWithPointers(const char* s, const char* sl) {
	const char* ps = s;
	const char* psl = sl;
	const char* pCurrent;
	const char* pTemp;

	if (strlen(s) <= 0) {
		return -1; // строка пустая
	}
	if (strlen(s) >= 255) {
		return -2; // строка слишком большая
	}
	if (strlen(sl) <= 0) {
		return -3; // подстрока пустая
	}
	if (strlen(sl) > strlen(s)) {
		return -4; // подстрока длиннее строки
	}

	for (ps = s; ps <= s + strlen(s) - strlen(sl); ps++) {
		pCurrent = ps;
		psl = sl;

		while (*pCurrent == *psl && *psl != '\0') {
			pCurrent++;
			psl++;
		}

		if (*psl == '\0') {
			return (ps - s) + 1; // возвращаем 1-индексацию
		}
	}

	return 0; // подстрока не найдена
}

