#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>

const int ORDER = 4; // B+树的阶数，可以根据需要调整

struct Node {
    bool isLeaf;
    std::vector<int> keys;
    std::vector<Node*> children;
    Node* next; // 用于叶子节点的链接

    Node(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
private:
    Node* root;
    
    Node* findLeaf(int key);
    void splitChild(Node* parent, int index);
    void insertNonFull(Node* node, int key);
    void removeFromLeaf(Node* leaf, int key);
    void removeFromNonLeaf(Node* node, int index);
    int getPred(Node* node, int index);
    int getSucc(Node* node, int index);
    void borrowFromPrev(Node* node, int index);
    void borrowFromNext(Node* node, int index);
    void merge(Node* node, int index);
    void fill(Node* node, int index);
    
public:
    BPlusTree() : root(nullptr) {}
    void insert(int key);
    bool search(int key);
    void remove(int key);
    void remove_key_value(Node* node, int key);
    void update(int oldKey, int newKey);
    void display();
    int size();
};