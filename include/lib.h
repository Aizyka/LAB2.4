typedef struct Node Node;

struct Node {
    char* question;
    Node* yes;
    Node* no;
};

Node* createNode(const char* question);
void insert(Node* node, const char* question, const char* answer);
void play(Node* node, Node*** answers, int* answersSize);
void save(Node* root, const char* filename);
Node* load(const char* filename);
void printTree(struct Node* root, int level, Node** answers, int answersLen);
void clearMemory(Node* node);