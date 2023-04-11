typedef struct Node Node;

struct Node {
    char* question;
    Node* yes;
    Node* no;
};

Node* createNode(char* question);
void insert(Node* node, char* question, char* answer);
void play(Node* node, Node*** answers, int* answers_size);
void save(Node* root, char* filename);
Node* load(char* filename);
void print_tree(struct Node* root, int level, Node** answers, int answers_len);
void clearmemory(Node* node);