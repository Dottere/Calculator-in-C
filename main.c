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

// Prototypes

/*Terminal calculator app*/
int main(void) {
    setlocale(LC_ALL, "");


    char operation[99];

    wprintf(L"###Számológép###");

    printf("\n>"); fgets(operation, 99, stdin);


    bool run = true;
    while (run) {
        const calculation CalcObject = evaluateOperation(operation);
        if (CalcObject.operand == MissingOperand) {
            wprintf(L"Kötelező megadni egy operandust! Ezek lehetnek: \n+ összeadás"
                "\n- kivonás\n* szorzás\n/ osztás\n^ hatványozás\n%% maradékos osztás\n>");
            fgets(operation, 99, stdin);
            continue;
        }

        const result CalcResult = doCalculation(CalcObject);
        if (CalcResult.errorCode == ZeroDivision) {
            wprintf(L"A nullával való osztás nem értelmezhető!\n>");
            fgets(operation, 99, stdin);
            continue;
        }

        if (CalcResult.errorCode == ModuloError) {
            wprintf(L"A maradékos osztás csak egész számokkal működik!\n>");
            fgets(operation, 99, stdin);
            continue;
        }

        if (CalcResult.errorCode == Miscellaneous)
            return 1;

        wprintf(L"Ennek a számolásnak az eredménye: %g", CalcResult.result);
        run = false;
    }

    return 0;
}

calculation evaluateOperation(char *operation) {
    const char operations[] = "+-*/xX^%";

    char operationWithoutWhitespace[99];

    // Take the input string which can have any number of whitespace and remove those
    const size_t lenWithSpace = strlen(operation);
    int cleanIndex = 0;
    for (int dirtyIndex = 0; dirtyIndex < lenWithSpace; dirtyIndex++) {
        if (operation[dirtyIndex] == '.') {
            operation[dirtyIndex] = ','; // convert dot to comma
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