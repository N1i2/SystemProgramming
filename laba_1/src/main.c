#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int Pos(const char* s, const char* sl, int howCheck);
int CheckUseSimvl(char s, char sl);
int CheckUseNumber(char s, char sl);

int main() {
    const char* firstWords[10] = {
        /*1*/	"Hello World",
        /*2*/	"nanananannananananananananananananana",
        /*3*/	"le",
        /*4*/	"12345678910111213141516171819202122232425262728293031323334353637383940",
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

    const char* howFindName[] = { "Simvle: ", "Number: " };

    for (int i = 0; i < sizeof(firstWords) / sizeof(firstWords[0]); i++) {
        for (int j = 0; j > -2; j--) {
            printf("%d) %s\n%s\n%s\n%d\n",
                i + 1,
                howFindName[abs(j)],
                firstWords[i],
                secondWords[i],
                Pos(firstWords[i], secondWords[i], j));
        }
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

int Pos(const char* s, const char* sl, int howCheck) {
    int i = 0;
    int checkUseSimvl = (howCheck >= 0), sameSimvl;
    if (strlen(s) >= 255 || strlen(sl) >= 255)
    {
        return i;
    }

    if (strlen(s) <= 0 || strlen(sl) <= 0 || strlen(sl) > strlen(s)) {
        return i;
    }

    for (int hasSame = 0; i < strlen(s); i++) {
        if (checkUseSimvl) {
            sameSimvl = CheckUseSimvl(s[i], sl[hasSame]);
        }
        else {
            sameSimvl = CheckUseNumber(s[i], sl[hasSame]);
        }

        if (sameSimvl) {
            hasSame++;
        }
        else {
            i -= hasSame;
            hasSame = 0;
        }

        if (hasSame == strlen(sl)) {
            return i - hasSame + 2;
        }
    }

    return 0;
}

int CheckUseSimvl(char s, char sl) {
    return (s == sl);
}

int CheckUseNumber(char s, char sl) {
    return (s == sl);
}