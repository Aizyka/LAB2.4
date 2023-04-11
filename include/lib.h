typedef struct Node Node;

struct Node {
    char* question;
    Node* yes;
    Node* no;
};

Node* createNode(const char* question);
void insert(Node* node, const char* question, const char* answer);
void play(Node* node, Node*** answers, int* answers_size);
void save(Node* root, const char* filename);
Node* load(const char* filename);
void print_tree(struct Node* root, int level, Node** answers, int answers_len);
void clearmemory(Node* node);