#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lib.h"

Node* createNode(const char* question) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->question = strdup(question);
    node->yes = NULL;
    node->no = NULL;
    return node;
}

void insert(Node* node, const char* question, const char* answer) {
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

void save(Node* root, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if(fp != NULL) {
        saveToFile(fp, root);
        fclose(fp);
    }
}

int getend(const char* line) {
    int c = 0;
    int end = 0;
    for(int i = 0; i < (int)strlen(line); i++) {
        if(line[i] == '{')
            c++;
        if(line[i] == '}') {
            c--;
            if(c == 0) {
                end = i;
                break;
            }
        }
    }
    return end;
}

Node* parse_node(const char* line);

void parse(const char* yesStart, const char* tocheck, const char* line, Node** node) {
    if (yesStart != NULL && strcmp(tocheck,"yes") == 0) {
        int end = getend(yesStart);

        char* yesEnd = calloc(end-5, sizeof(char));
        strncpy(yesEnd, yesStart+6, end - 5);
        if (yesEnd != NULL) {
            (*node)->yes = parse_node(yesEnd);
            free(yesEnd);
        }
        char* afterYes = calloc(strlen(line)-end-2, sizeof(char));
        strncpy(afterYes, yesStart+end+2, strlen(line)-end-2);
        const char* noStart = strstr(afterYes, "\"no\":{");
        free(afterYes);
        if (noStart != NULL) {
            end = getend(noStart);
            char* noEnd = calloc(end-4, sizeof(char));
            strncpy(noEnd, noStart+5, end - 4);
            if (noEnd != NULL) {
                (*node)->no = parse_node(noEnd);
                free(noEnd);
            }
        }
    }
    else {
        const char* noStart = strstr(line, "\"no\":{");
        if (noStart != NULL) {
            int end = getend(noStart);
            char* noEnd = calloc(end-5, sizeof(char));
            strncpy(noEnd, noStart+5, end - 5);
            if (noEnd != NULL) {
                (*node)->no = parse_node(noEnd);
                free(noEnd);
            }
        }
    }
}

Node* parse_node(const char* line) {
    if(line == NULL)
        return NULL;
    const char* questionStart = strstr(line, "\":\"");
    if (questionStart == NULL) {
        return NULL;
    }
    questionStart += 3;
    const char* questionEnd = strchr(questionStart, '\"');
    if (questionEnd == NULL) {
        return NULL;
    }
    char* question = strndup(questionStart, questionEnd - questionStart);
    Node* node = createNode(question);
    free(question);
    const char* yesStart = strstr(line, "\"yes\":{");
    char* tocheck = strndup(questionEnd+3, 3);
    parse(yesStart,tocheck,line,&node);
    free(tocheck);
    return node;
}

Node* loadFromFile(FILE* fp) {
    char* line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        return NULL;
    }

    return parse_node(line);
}

Node* load(const char* filename) {
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