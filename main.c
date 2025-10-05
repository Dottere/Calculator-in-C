// Standard libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Necessary to print hungarian accented characters
#include <wchar.h>
#include <locale.h>


typedef struct {
    int operand;
    double num1;
    double num2;
} calculation;

typedef struct {
    double result;
    int errorCode;
} result;

typedef struct {
    const wchar_t *welcome;
    const wchar_t *result;
    const wchar_t *missingOperand;
    const wchar_t *zeroDivision;
    const wchar_t *moduloError;
} StringArray;

typedef struct {
    wchar_t hu[126];
    wchar_t de[146];
    wchar_t en[135];
} translations;

enum Operand {
    ADDITION = 1,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    POW,
    MOD
};

enum errCodes {
    NoError = 0,
    ZeroDivision = 101,
    MissingOperand,
    ModuloError,
    Miscellaneous
};

// Prototypes

calculation evaluateOperation(char *operation);
result doCalculation(calculation calculation);
StringArray *setupLanguage(void);
void checkForErrors(StringArray stringArray);

// Prototypes

/*Terminal calculator app*/
int main(void) {
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    const StringArray *stringArray = setupLanguage();

    checkForErrors(*stringArray);

}

calculation evaluateOperation(char *operation) {
    const char operations[] = "+-*/xX^%";

    char operationWithoutWhitespace[99];

    // Take the input string which can have any number of whitespace and remove those
    const size_t lenWithSpace = strlen(operation);
    int cleanIndex = 0;
    for (int dirtyIndex = 0; dirtyIndex < lenWithSpace; dirtyIndex++) {
        if (operation[dirtyIndex] == ',') {
            operation[dirtyIndex] = '.';
        }
        if (operation[dirtyIndex] != ' ') {
            operationWithoutWhitespace[cleanIndex++] = operation[dirtyIndex];
        }
    }
    operationWithoutWhitespace[cleanIndex] = '\0';
    calculation Calc = {-1};
    bool run = true;

    for (int i = 0; i <= cleanIndex && run; i++) {
        if (strchr(operations, operationWithoutWhitespace[i])) {
            char *operandIndex;
            const double num1 = strtod(operationWithoutWhitespace, &operandIndex);
            const double num2 = strtod(operandIndex+1, NULL);
            Calc.num1 = num1;
            Calc.num2 = num2;

            switch (operationWithoutWhitespace[i]) {
                case '+':
                    Calc.operand = ADDITION;
                    break;
                case '-':
                    Calc.operand = SUBTRACTION;
                    break;
                case '*':
                case 'x':
                case 'X':
                    Calc.operand = MULTIPLICATION;
                    break;
                case '/':
                    Calc.operand = DIVISION;
                    break;
                case '^':
                    Calc.operand = POW;
                    break;
                case '%':
                    Calc.operand = MOD;
                    break;
                default:
                    Calc.operand = MissingOperand;
                    break;
            }
            run = false;
        }
    }
    return Calc;
}

result doCalculation(const calculation calculation) {
    result result = {.errorCode = NoError};

    switch (calculation.operand) {
        case 1:
            result.result = calculation.num1 + calculation.num2;
            return result;
        case 2:
            result.result = calculation.num1 - calculation.num2;
            return result;
        case 3:
            result.result = calculation.num1 * calculation.num2;
            return result;
        case 4:
            if (calculation.num2 == 0) {
                result.result = 0;
                result.errorCode = ZeroDivision;
                return result;
            }
            printf("%f %f", calculation.num1, calculation.num2);
            result.result = calculation.num1 / calculation.num2;
            return result;
        case 5:
            result.result = pow(calculation.num1, calculation.num2);
            return result;
        case 6:
            if (calculation.num1 != trunc(calculation.num1) || calculation.num2 != trunc(calculation.num2)) {
                result.errorCode = ModuloError;
                result.result = 0;
                return result;
            }
            const int num1 = floor(calculation.num1);
            const int num2 = floor(calculation.num2);
            result.result = num1 % num2;
            return result;

        default:
            result.result = -1;
            result.errorCode = Miscellaneous;
            return result;
    }
}

void checkForErrors(StringArray stringArray) {

    char operation[99];

    wprintf(L"%ls", stringArray.welcome);

    wprintf(L"\n> "); fgets(operation, 99, stdin);
    bool run = true;
    while (run) {
        const calculation CalcObject = evaluateOperation(operation);
        if (CalcObject.operand == MissingOperand) {
            wprintf(L"%ls ", stringArray.missingOperand);
            fgets(operation, 99, stdin);
            continue;
        }

        const result CalcResult = doCalculation(CalcObject);
        if (CalcResult.errorCode == ZeroDivision) {
            wprintf(L"%ls ", stringArray.zeroDivision);
            fgets(operation, 99, stdin);
            continue;
        }

        if (CalcResult.errorCode == ModuloError) {
            wprintf(L"%ls ", stringArray.moduloError);
            fgets(operation, 99, stdin);
            continue;
        }

        if (CalcResult.errorCode == Miscellaneous)
            exit(1);

        wprintf(L"%ls " " %g\n\n", stringArray.result, CalcResult.result);
        run = false;
    }
}

StringArray* setupLanguage(void) {
    static StringArray chosenLanguage;

    const struct { const wchar_t *hu, *de, *en; } translations[] = {
        {.hu = L"A magyar nyelvet választottad!\n\nSzámológép\n",
            .de = L"Sie haben die deutsche Sprache gewählt!\n\nTaschenrechner\n",
            .en = L"You chose English!\n\nCalculator\n"},


        {.hu = L"Ennek a számolásnak az eredménye:",
            .de = L"Das Ergebnis dieser Berechnung:",
            .en = L"The result of this calculation:"},

        {.hu = L"Kötelező megadni egy operandust! Ezek lehetnek:\n+ Összeadás \n- Kivonás\n* Szorzás\n/ Osztás\n^ Hatványozás\n%% Maradékos osztás\n>",
            .de = L"Es muss ein Operand angegeben werden! Diese können sein:\n+ Addition\n- Subtraktion\n* Multiplikation\n/ Division\n^ Potenzierung\n%% Modulo-Division\n>",
            .en = L"You must provide an operand! These can be:\n+ Addition\n- Subtraction\n* Multiplication\n/ Division\n^ Raising to power\n%% Modulo-Division\n>"},

        {.hu = L"A nullával való osztás nem megengedett!\n>",
            .de = L"Division durch Null ist nicht erlaubt!\n>",
            .en = L"Division by zero is not allowed!\n>"},

            {.hu = L"A maradékos osztás csak egész számokkal megengedett!\n>",
            .de = L"Modulo-Division ist nur mit Ganzzahlen erlaubt!\n>",
            .en = L"Modulo-Division is only allowed with integers!\n>"},
    };

    long lang = 0;
    int valid = 0;

    do {
        wprintf(L"\n\nVálassz egy nyelvet/Wähle eine Sprache/Choose a language:\n1 Magyar\n2 Deutsch\n3 English\n> ");
        char langChar[3];
        fgets(langChar, sizeof(langChar), stdin);
        lang = strtol(langChar, NULL, 10);

        if (lang >= 1 && lang <= 3)
            valid = 1;
    } while (!valid);

    if (lang == 1) {
        chosenLanguage.welcome        = translations[0].hu;
        chosenLanguage.result         = translations[1].hu;
        chosenLanguage.missingOperand = translations[2].hu;
        chosenLanguage.zeroDivision   = translations[3].hu;
        chosenLanguage.moduloError    = translations[4].hu;
    } else if (lang == 2) {
        chosenLanguage.welcome        = translations[0].de;
        chosenLanguage.result         = translations[1].de;
        chosenLanguage.missingOperand = translations[2].de;
        chosenLanguage.zeroDivision   = translations[3].de;
        chosenLanguage.moduloError    = translations[4].de;
    } else {
        chosenLanguage.welcome        = translations[0].en;
        chosenLanguage.result         = translations[1].en;
        chosenLanguage.missingOperand = translations[2].en;
        chosenLanguage.zeroDivision   = translations[3].en;
        chosenLanguage.moduloError    = translations[4].en;
    }

    return &chosenLanguage;
}