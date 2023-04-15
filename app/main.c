#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/lib.h"

void clearHistory(Node*** answers, int* answersSize) {
    char answ[100];
    printf("Clear history?\n");
    scanf("%99s", answ);
    if(strcmp(answ, "yes") == 0)
    {
        free(*answers);
        *answers = malloc(sizeof(Node*));
        *answersSize = 0;
    }
}

int main() {
    Node* root = NULL;
    int working = 1;
    Node** answers = malloc(sizeof(Node*));
    int answersSize = 0;
    while(working) {
        int menu = 0;
        char path[100];
        printf("\nMenu:\n1 - Play\n2 - Show tree\n3 - Load\n4 - Save\n5 - Exit\n");
        while(!scanf("%d", &menu) || menu < 1 || menu > 5) {
            printf("Invalid option!\n");
            rewind(stdin);
        }

        switch (menu) {
            case 1:
                if(answersSize > 0) {
                    clearHistory(&answers,&answersSize);
                }
                if(root != NULL)
                {
                    play(root, &answers, &answersSize);
                }
                else
                    printf("\033[31mError! Database not loaded\033[0m\n");
                break;
            case 2:
                if(root != NULL)
                    printTree(root, 0, answers, answersSize);
                else
                    printf("\033[31mError! Database not loaded\033[0m\n");
                break;
            case 3:
                printf("Input database: ");
                scanf("%99s", path);
                root = load(path);
                break;
            case 4:
                printf("Output database: ");
                scanf("%99s", path);
                save(root, path);
                break;
            default:
                working = 0;
                break;
        }
    }
    if(root != NULL) {
        clearMemory(root);
    }
    free(answers);
    return 0;
}