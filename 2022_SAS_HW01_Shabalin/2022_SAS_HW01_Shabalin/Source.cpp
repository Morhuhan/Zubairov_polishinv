#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

/* ������� ����� */
#define STKDPTH 32 

/* ���������� ���������� �����*/
int scount = -1;
int stack[STKDPTH];

/* ��������, ������������ �������� parse */
#define VAL  0  /* � ���� �������� ����� �������� */
#define ADD  1  /* �������� */
#define SUB  2  /* ��������� */
#define MUL  3  /* ��������� */
#define DIV  4  /* ������� */
#define SOF -1  /* ������������ ����� */
#define SUF -2  /* � ����� ������������ ��������� */
#define UNK -3  /* ������������ �������� */

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

        // ���� ������� ����� - ���������� ��� � postfix
        if ((c > 47) && (c < 58)) {
            postfix[j++] = c;
        }

        // ���� �������� ������� �������� (+, -, *, /) �� ���������
        else {

            switch (c) {

            case '+':

                //���� ����(STACK) ���� ��� �������� ����� ������ � �������(SCOUNT), �� ���������(PUSH) �������� �������� � ����(STACK).
                if ((scount == -1) || (stack[scount] == '(')) {
                    postfix[j++] = ' ';
                    push(c);
                }

                else {

                    /* ���� �������� �������� ����� ����� ������ ��� ������ ���������, ��� �������(SCOUNT),      */
                    /* ��������� POP � postfix, ���� �� ������� �������� � ������� ����������� ��� ����� ������  */
                    /* �� �������(SCOUNT), ����� ��������(PUSH) �������� �������� � ����(STACK).                 */

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

                // ���� �������� ������� �������� ����� �������, ��������� (PUSH) ��� � ���� (STACK)
            case '(':
                push(c);
                break;

                /* ���� �������� ������� �������� ������ �������, ��������� ���� (POP) �   */
                /* ��������� ��� �������� � postfix, ���� �� ������� ����� ������� ������. */
                /* ������� ��������� ������ �� ����� (STACK).                              */

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

    // ���������� ����� �������������� ��������
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

        // �������� ���������� ������� �������� ��� ����� ��� ������ �������������� ��������
        switch (parse(ptr)) {

        // �������� ������ - �����
        case VAL:

            // �������� ������ temp
            strcpy_s(temp, "");

            // ��������� ���� �� �� ���� ������ ������
            while (1) {

                // ���� ���� ������, �� �������� ���������� ��� ����� � ������ ����
                if (*ptr != ' ') {
                    temp[i++] = *ptr++;
                }

                // ���� ��� �������, ������ ����������� ����
                else {
                    break;
                }

            }

            // ������������ temp � ����
            stack[++scount] = atoi(temp);
            i = 0;
            break;

        // ���������
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
                printf("������� �� ����!\n");
                return(1);
            }

        case SOF:
            printf("������������ �����!\n");
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
