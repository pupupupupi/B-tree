#include "B+.h"

int main() {
    BPlusTree tree;
    
    // 插入操作
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    tree.insert(15);
    tree.insert(30);

    tree.insert(27);
    tree.insert(34);
    tree.insert(35);
    tree.insert(85);
    tree.insert(40);


    // tree.insert(21);
    // tree.insert(24);
    // tree.insert(12);
    // tree.insert(19);
    // tree.insert(32);
    
    // 显示树
    std::cout << "Tree after insertions: ";
    tree.display();
    
    // 搜索操作
    std::cout << "Search 15: " << (tree.search(15) ? "Found" : "Not Found") << std::endl;
    std::cout << "Search 25: " << (tree.search(25) ? "Found" : "Not Found") << std::endl;
    
    // 删除操作
    tree.remove(15);
    std::cout << "Tree after removing 15: ";
    tree.display();
    
    // 更新操作
    tree.update(20, 25);
    std::cout << "Tree after updating 20 to 25: ";
    tree.display();
    
    return 0;
}