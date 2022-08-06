#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

/* Глубина стека */
#define STKDPTH 32 

/* Глобальные переменные стека*/
int scount = -1;
int stack[STKDPTH];

/* Значения, возвращаемые функцией parse */
#define VAL  0  /* В стек занесено новое значение */
#define ADD  1  /* Сложение */
#define SUB  2  /* Вычитание */
#define MUL  3  /* Умножение */
#define DIV  4  /* Деление */
#define SOF -1  /* Переполнение стека */
#define SUF -2  /* В стеке недостаточно операндов */
#define UNK -3  /* Неопознанное значение */

int pop(char* postfix, int* j) {
    if (postfix[*j - 1] == ' ') {
        *j -= 1;
    }
    postfix[*j] = ' ';
    *j += 1;
    postfix[*j] = stack[scount];
    *j += 1;
    postfix[*j] = ' ';
    *j += 1;
    return stack[scount--];
}

void push(char c) {
    stack[++scount] = c;
}

void pop_breket(char* postfix, int* j) {
    char c;
    while (1) {
        c = stack[scount];
        if (c == '(') {
            scount--;
            break;
        }
        else {
            pop(postfix, j);
        }
    }
}

void pop_stack(char* postfix, int* j) {
    while (scount != -1) {
        postfix[*j] = ' ';
        *j += 1;
        postfix[*j] = stack[scount--];
        *j += 1;
    }
    postfix[*j] = 0;
}

void infix_to_postfix(char* infix, char* postfix) {

    int c = 1;
    int check = 0;
    int i = 0;
    int j = 0;

    while (c != '\0') {

        c = infix[i++];

        // Если элемент число - записываем его в postfix
        if ((c > 47) && (c < 58)) {
            postfix[j++] = c;
        }

        // Если входящий элемент оператор (+, -, *, /) то проверяем
        else {

            switch (c) {

            case '+':

                //Если стек(STACK) пуст или содержит левую скобку в вершине(SCOUNT), то добавляем(PUSH) входящий оператор в стек(STACK).
                if ((scount == -1) || (stack[scount] == '(')) {
                    postfix[j++] = ' ';
                    push(c);
                }

                else {

                    /* Если входящий оператор имеет более низкий или равный приоритет, чем вершине(SCOUNT),      */
                    /* выгружаем POP в postfix, пока не увидите оператор с меньшим приоритетом или левую скобку  */
                    /* на вершине(SCOUNT), затем добавьте(PUSH) входящий оператор в стек(STACK).                 */

                    while (1) {
                        check = pop(postfix, &j);

                        if (check == '+' || check == '-' || stack[scount] == '(') {
                            break;
                        }
                    }
                    push(c);
                }

                break;

            case '-':

                if ((scount == -1) || (stack[scount] == '(')) {
                    postfix[j++] = ' ';
                    push(c);
                }

                else {

                    while (1) {

                        check = pop(postfix, &j);

                        if (check == '+' || check == '-' || stack[scount] == '(') {
                            break;
                        }
                    }
                    push(c);
                }
                break;

            case 'x':

                if ((scount == -1) || (stack[scount] == '(') || (stack[scount] == '+') || (stack[scount] == '-')) {
                    postfix[j++] = ' ';
                    push(c);
                }

                else {

                    while (1) {

                        check = pop(postfix, &j);

                        if (check == '+' || check == '-' || stack[scount] == '(' || check == 'x' || check == '/') {
                            break;
                        }
                    }
                    push(c);
                }
                break;

            case '/':

                if ((scount == -1) || (stack[scount] == '(') || (stack[scount] == '+') || (stack[scount] == '-')) {
                    postfix[j++] = ' ';
                    push(c);
                }

                else {

                    while (1) {

                        check = pop(postfix, &j);

                        if (check == '+' || check == '-' || stack[scount] == '(' || check == 'x' || check == '/') {
                            break;
                        }
                    }
                    push(c);
                }
                break;

                // Если входящий элемент является левой скобкой, поместите (PUSH) его в стек (STACK)
            case '(':
                push(c);
                break;

                /* Если входящий элемент является правой скобкой, выгружаем стек (POP) и   */
                /* добавляем его элементы в postfix, пока не увидите левую круглую скобку. */
                /* Удалите найденную скобку из стека (STACK).                              */

            case ')':
                pop_breket(postfix, &j);
                j--;
                break;

            default:
                break;
            }
        }
    }

    pop_stack(postfix, &j);

}

int parse(char* s) {

    // Распознаем знаки арифметических операций
    switch (*s) {
    case '-':
        if (scount >= 1) {
            return(SUB);
        }
        else return(SUF);

    case '+':
        if (scount >= 1) {
            return(ADD);
        }
        else return(SUF);

    case 'x':
        if (scount >= 1) {
            return(MUL);
        }
        else return(SUF);

    case '/':
        if (scount >= 1) {
            return(DIV);
        }
        else return(SUF);
    }

    return(VAL);
}

int culc_postfix(char* postfix) {

    char temp[30];
    char* ptr = postfix;

    int i = 0;

    while (1) {

        if (*(ptr-1) == 0) {
            return 0;
        }

        // Пытаемся распознать текущий аргумент как число или символ арифметической операции
        switch (parse(ptr)) {

        // Считаный символ - цифра
        case VAL:

            // Отчищаем строку temp
            strcpy_s(temp, "");

            // Проверяем есть ли за этой цифрой пробел
            while (1) {

                // Если есть пробел, то начинаем закидывать это число в строку темп
                if (*ptr != ' ') {
                    temp[i++] = *ptr++;
                }

                // Если нет пробела, значит заканчиваем цикл
                else {
                    break;
                }

            }

            // Перекидываем temp в стэк
            stack[++scount] = atoi(temp);
            i = 0;
            break;

        // Вычисляем
        case ADD:
            stack[scount - 1] = stack[scount - 1] + stack[scount];
            scount--;
            ptr++;
            break;

        case SUB:
            stack[scount - 1] = stack[scount - 1] - stack[scount];
            scount--;
            ptr++;
            break;

        case MUL:
            stack[scount - 1] = stack[scount - 1] * stack[scount];
            scount--;
            ptr++;
            break;

        case DIV:
            if (stack[scount] != 0) {
                stack[scount - 1] = stack[scount - 1] / stack[scount];
                scount--;
                ptr++;
                break;
            }
            else {
                printf("Деление на ноль!\n");
                return(1);
            }

        case SOF:
            printf("Переполнение стека!\n");
            return(1);

            ptr++;
        }
    }
}

int main() {

    setlocale(LC_ALL, ".1251");

    char infix[30];
    char postfix[30];

    scanf_s("%s", infix, 30);

    infix_to_postfix(infix, postfix);

    printf("\n%s\n", postfix);

    culc_postfix(postfix);

    printf("\n%d\n", stack[0]);

    return(0);
}
