#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/lib.h"

int main() {
    Node* root = NULL;
    int working = 1;
    Node** answers = malloc(sizeof(Node*));
    int answers_size = 0;
    while(working) {
        int menu = 0;
        int clear = 0;
        char path[100];
        printf("Menu:\n1 - Play\n2 - Show tree\n3 - Load\n4 - Save\n5 - Exit\n");
        while(!scanf("%d", &menu) || menu < 1 || menu > 5) {
            printf("Invalid option!\n");
            rewind(stdin);
        }

        switch (menu) {
            case 1:
                if(root != NULL)
                {
                    if(answers_size > 0) {
                        printf("Clear history?\n");
                        scanf("%99s", path);
                        if(strcmp(path, "yes") == 0)
                        {
                            free(answers);
                            answers = malloc(sizeof(Node*));
                            answers_size = 0;
                        }
                    }
                    play(root, &answers, &answers_size);
                }
                else
                    printf("\033[31mError! Database not loaded\033[0m\n");
                break;
            case 2:
                if(root != NULL)
                    print_tree(root, 0, answers, answers_size);
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
        clearmemory(root);
    }
    return 0;
}