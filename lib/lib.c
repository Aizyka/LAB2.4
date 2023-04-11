#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lib.h"

Node* createNode(char* question) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->question = strdup(question);
    node->yes = NULL;
    node->no = NULL;
    return node;
}

void insert(Node* node, char* question, char* answer) {
    if (strcmp(answer, "yes") == 0) {
        if (node->yes == NULL) {
            node->yes = createNode(question);
        } else {
            insert(node->yes, question, answer);
        }
    } else if (strcmp(answer, "no") == 0) {
        if (node->no == NULL) {
            node->no = createNode(question);
        } else {
            insert(node->no, question, answer);
        }
    }
}

void play(Node* node, Node*** answers, int* answers_size) {
    if(node->yes == NULL) {
        printf("Это %s?\n", node->question);
    }
    else {
        printf("%s\n", node->question);
    }
    char response[10];
    scanf("%s", response);
    if (strcmp(response, "yes") == 0) {
        *answers = realloc(*answers, (*answers_size + 1)*sizeof(Node*));
        (*answers)[*answers_size] = node;
        (*answers_size)++;
        if(node->yes != NULL) {
            play(node->yes, answers, answers_size);
        }
        else {
            printf("Я угадал!\n");
        }
    } else if (strcmp(response, "no") == 0) {
        if(node->no != NULL) {
            play(node->no, answers, answers_size);
        }
        else {
            printf("Я проиграл. Что это было?\n");
            char newObject[100];
            char newQuestion[100];
            scanf("%s", newObject);
            printf("Напишите вопрос, который можно задать, чтобы отличить %s от %s:\n", newObject, node->question);
            scanf(" %[^\n]s", newQuestion);
            char temp[100];
            strcpy(temp, node->question);
            free(node->question);
            node->question = strdup(newQuestion);
            if (strcmp(response, "no") == 0) {
                node->yes = createNode(newObject);
                node->no = createNode(temp);
            } else {
                node->yes = createNode(temp);
                node->no = createNode(newObject);
            }
        }
    }
}

void saveToFile(FILE* fp, Node* node) {
    if (node == NULL) {
        return;
    }
    fprintf(fp, "{");
    fprintf(fp, "\"question\":\"%s\"", node->question);
    if (node->yes != NULL) {
        fprintf(fp, ",\"yes\":");
        saveToFile(fp, node->yes);
    }
    if (node->no != NULL) {
        fprintf(fp, ",\"no\":");
        saveToFile(fp, node->no);
    }
    fprintf(fp, "}");
}

void save(Node* root, char* filename) {
    FILE* fp = fopen(filename, "w");
    saveToFile(fp, root);
    fclose(fp);
}

Node* parse_node(char* line) {
    char* questionStart = strstr(line, "\":\"");
    if (questionStart == NULL) {
        return NULL;
    }
    questionStart += 3;
    char* questionEnd = strchr(questionStart, '\"');
    if (questionEnd == NULL) {
        return NULL;
    }
    char* question = strndup(questionStart, questionEnd - questionStart);
    Node* node = createNode(question);
    char* yesStart = strstr(line, "\"yes\":{");
    if (yesStart != NULL && strcmp(strndup(questionEnd+3, 3),"yes") == 0) {
        int end = strlen(yesStart);
        int c = 0;
        for(int i = 0; i < strlen(yesStart); i++) {
            if(yesStart[i] == '{')
                c++;
            if(yesStart[i] == '}') {
                c--;
                if(c == 0) {
                    end = i;
                    break;
                }
            }
        }
        char* yesEnd = calloc(end-6, sizeof(char));
        strncpy(yesEnd, yesStart+6, end - 5);
        if (yesEnd != NULL) {
            node->yes = parse_node(yesEnd);
        }
        char* afterYes = calloc(strlen(line)-end-2, sizeof(char));
        strncpy(afterYes, yesStart+end+2, strlen(line)-end-2);
        char* noStart = strstr(afterYes, "\"no\":{");
        if (noStart != NULL) {
            int end = strlen(noStart);
            int c = 0;
            for(int i = 0; i < strlen(noStart); i++) {
                if(noStart[i] == '{')
                    c++;
                if(noStart[i] == '}') {
                    c--;
                    if(c == 0) {
                        end = i;
                        break;
                    }
                }
            }
            char* noEnd = calloc(end-5, sizeof(char));
            strncpy(noEnd, noStart+5, end - 4);
            if (noEnd != NULL) {
                node->no = parse_node(noEnd);
            }
        }
    }
    else {
        char* noStart = strstr(line, "\"no\":{");
        if (noStart != NULL) {
            int end = strlen(noStart);
            int c = 0;
            for(int i = 0; i < strlen(noStart); i++) {
                if(noStart[i] == '{')
                    c++;
                if(noStart[i] == '}') {
                    c--;
                    if(c == 0) {
                        end = i;
                        break;
                    }
                }
            }
            char* noEnd = calloc(end-5, sizeof(char));
            strncpy(noEnd, noStart+5, end - 5);
            if (noEnd != NULL) {
                node->no = parse_node(noEnd);
            }
        }
    }
    return node;
}

Node* loadFromFile(FILE* fp) {
    char* line;
    size_t len;
    if (getline(&line, &len, fp) == -1) {
        return NULL;
    }

    return parse_node(line);
}

Node* load(char* filename) {
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("\033[31mError! File not found\033[0m\n");
        return NULL;
    }
    Node* root = loadFromFile(fp);
    fclose(fp);
    return root;
}

void print_tree(struct Node* root, int level, Node** answers, int answers_len) {
    if (root == NULL) {
        return;
    }
    // print the question at this node
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    int founded = 0;
    for(int i = 0; i < answers_len; i++) {
        if(answers[i] == root) {
            founded = 1;
            break;
        }
    }
    if(founded)
        printf("\033[32m= %s\033[0m\n", root->question);
    else
        printf("\033[31m- %s\033[0m\n", root->question);
    // print the yes and no subtrees recursively
    print_tree(root->yes, level+1, answers, answers_len);
    print_tree(root->no, level, answers, answers_len);
}

void clearmemory(Node* node) {
    if(node->question != NULL)
        free(node->question);
    if(node->yes != NULL) {
        clearmemory(node->yes);
    }
    if(node->no != NULL) {
        clearmemory(node->no);
    }
    free(node);
}