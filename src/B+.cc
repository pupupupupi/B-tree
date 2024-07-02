#include "B+.h"

Node* BPlusTree::findLeaf(int key) {
    Node* current = root;
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->keys.size() && key >= current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }
    return current;
}

void BPlusTree::splitChild(Node* parent, int index) {
    Node* child = parent->children[index];
    Node* newChild = new Node(child->isLeaf);
    
    parent->keys.insert(parent->keys.begin() + index, child->keys[ORDER / 2]);
    parent->children.insert(parent->children.begin() + index + 1, newChild);
    
    newChild->keys.assign(child->keys.begin() + ORDER / 2, child->keys.end());
    child->keys.erase(child->keys.begin() + ORDER / 2, child->keys.end());
    
    if (!child->isLeaf) {
        newChild->children.assign(child->children.begin() + ORDER / 2, child->children.end());
        child->children.erase(child->children.begin() + ORDER / 2, child->children.end());
    } else {
        newChild->next = child->next;
        child->next = newChild;
    }
}

void BPlusTree::insertNonFull(Node* node, int key) {
    int i = node->keys.size() - 1;
    
    if (node->isLeaf) {
        node->keys.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == ORDER - 1) {
            splitChild(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void BPlusTree::insert(int key) {
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
    } else {
        if (root->keys.size() == ORDER - 1) {
            Node* newRoot = new Node();
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
            insertNonFull(root, key);
        } else {
            insertNonFull(root, key);
        }
    }
}

bool BPlusTree::search(int key) {
    if (root == nullptr) return false;
    Node* leaf = findLeaf(key);
    return std::binary_search(leaf->keys.begin(), leaf->keys.end(), key);
}

void BPlusTree::removeFromLeaf(Node* leaf, int key) {
    int index = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key) - leaf->keys.begin();
    if (index < leaf->keys.size() && leaf->keys[index] == key) {
        leaf->keys.erase(leaf->keys.begin() + index);
    }
}

void BPlusTree::removeFromNonLeaf(Node* node, int index) {
    int key = node->keys[index];
    
    if (node->children[index]->keys.size() >= ORDER / 2) {
        int pred = getPred(node, index);
        node->keys[index] = pred;
        remove_key_value(node->children[index], pred);
    }
    else if (node->children[index + 1]->keys.size() >= ORDER / 2) {
        int succ = getSucc(node, index);
        node->keys[index] = succ;
        remove_key_value(node->children[index + 1], succ);
    }
    else {
        merge(node, index);
        remove_key_value(node->children[index], key);
    }
}

int BPlusTree::getPred(Node* node, int index) {
    Node* curr = node->children[index];
    while (!curr->isLeaf) {
        curr = curr->children[curr->children.size() - 1];
    }
    return curr->keys[curr->keys.size() - 1];
}

int BPlusTree::getSucc(Node* node, int index) {
    Node* curr = node->children[index + 1];
    while (!curr->isLeaf) {
        curr = curr->children[0];
    }
    return curr->keys[0];
}

void BPlusTree::borrowFromPrev(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index - 1];
    
    child->keys.insert(child->keys.begin(), node->keys[index - 1]);
    
    if (!child->isLeaf) {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }
    
    node->keys[index - 1] = sibling->keys.back();
    sibling->keys.pop_back();
}

void BPlusTree::borrowFromNext(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];
    
    child->keys.push_back(node->keys[index]);
    
    if (!child->isLeaf) {
        child->children.push_back(sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }
    
    node->keys[index] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());
}

void BPlusTree::merge(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];
    
    child->keys.push_back(node->keys[index]);
    
    child->keys.insert(child->keys.end(), sibling->keys.begin(), sibling->keys.end());
    if (!child->isLeaf) {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
    } else {
        child->next = sibling->next;
    }
    
    node->keys.erase(node->keys.begin() + index);
    node->children.erase(node->children.begin() + index + 1);
    
    delete sibling;
}

void BPlusTree::fill(Node* node, int index) {
    if (index != 0 && node->children[index - 1]->keys.size() >= ORDER / 2) {
        borrowFromPrev(node, index);
    }
    else if (index != node->children.size() - 1 && node->children[index + 1]->keys.size() >= ORDER / 2) {
        borrowFromNext(node, index);
    }
    else {
        if (index != node->children.size() - 1) {
            merge(node, index);
        }
        else {
            merge(node, index - 1);
        }
    }
}

void BPlusTree::remove_key_value(Node* node, int key) {
    int index = std::lower_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
    
    if (node->isLeaf) {
        if (index < node->keys.size() && node->keys[index] == key) {
            removeFromLeaf(node, key);
        }
        return;
    }
    
    if (index < node->keys.size() && node->keys[index] == key) {
        removeFromNonLeaf(node, index);
    }
    else {
        bool lastChild = (index == node->children.size());
        
        if (node->children[index]->keys.size() < ORDER / 2) {
            fill(node, index);
        }
        
        if (lastChild && index > node->keys.size()) {
            remove_key_value(node->children[index - 1], key);
        }
        else {
            remove_key_value(node->children[index], key);
        }
    }
}

void BPlusTree::remove(int key) {
    if (root == nullptr) return;
    
    remove_key_value(root, key);
    
    if (root->keys.empty()) {
        if (root->isLeaf) {
            delete root;
            root = nullptr;
        }
        else {
            Node* temp = root;
            root = root->children[0];
            delete temp;
        }
    }
}

void BPlusTree::update(int oldKey, int newKey) {
    remove(oldKey);
    insert(newKey);
}

void BPlusTree::display() {
    if (root == nullptr) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }

    std::queue<std::pair<Node*, int>> q;
    q.push({root, 0});
    int lastLevel = -1;

    while (!q.empty()) {
        Node* node = q.front().first;
        int level = q.front().second;
        q.pop();

        if (level != lastLevel) {
            std::cout << std::endl << "Level " << level << ": ";
            lastLevel = level;
        }

        std::cout << "[ ";
        for (size_t i = 0; i < node->keys.size(); ++i) {
            std::cout << node->keys[i];
            if (i != node->keys.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "] ";

        if (!node->isLeaf) {
            for (Node* child : node->children) {
                q.push({child, level + 1});
            }
        }
    }
    std::cout << std::endl;

    // 打印叶子节点链表
    std::cout << "Leaf nodes: ";
    Node* leaf = root;
    while (!leaf->isLeaf) {
        leaf = leaf->children[0];
    }
    while (leaf != nullptr) {
        std::cout << "[ ";
        for (size_t i = 0; i < leaf->keys.size(); ++i) {
            std::cout << leaf->keys[i];
            if (i != leaf->keys.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "] -> ";
        leaf = leaf->next;
    }
    std::cout << "null" << std::endl;
}

int BPlusTree::size() {
    if (root == nullptr) return 0;
    int count = 0;
    Node* current = root;
    while (!current->isLeaf) {
        current = current->children[0];
    }
    while (current != nullptr) {
        count += current->keys.size();
        current = current->next;
    }
    return count;
}