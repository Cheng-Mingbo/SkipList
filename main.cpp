#include <iostream>
#include <memory>
#include <random>
#include <limits>
#include <chrono>

const int MAX_LEVEL = 32;
const float PROBABILITY = 0.5;

class Node {
  public:
    int key;
    int value;
    std::vector<std::shared_ptr<Node>> forward;
    
    Node(int key, int value, int level) : key(key), value(value), forward(level, nullptr) {}
};

class SkipList {
  public:
    SkipList() : level(0), header(std::make_shared<Node>(-1, -1, MAX_LEVEL)) {}
    
    static int randomLevel() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution d(PROBABILITY);
        int newLevel = 1;
        while (d(gen) && newLevel < MAX_LEVEL) {
            newLevel++;
        }
        return newLevel;
    }
    
    
    void insert(int key, int value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL, nullptr);
        std::shared_ptr<Node> current = header;
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; i++) {
                update[i] = header;
            }
            level = newLevel;
        }
        std::shared_ptr<Node> newNode = std::make_shared<Node>(key, value, newLevel);
        for (int i = 0; i < newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }
    
    int search(int key) {
        std::shared_ptr<Node> current = header;
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current != nullptr && current->key == key) {
            return current->value;
        }
        return -1;
    }
    
    void remove(int key) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL, nullptr);
        std::shared_ptr<Node> current = header;
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        current = current->forward[0];
        if (current != nullptr && current->key == key) {
            for (int i = 0; i < level; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }
            while (level > 1 && header->forward[level - 1] == nullptr) {
                level--;
            }
        }
    }
  
  private:
    int level;
    std::shared_ptr<Node> header;
};

void testSkipListPerformance(int numOperations) {
    SkipList skipList;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 插入操作
    for (int i = 0; i < numOperations; i++) {
        int key = dis(gen);
        int value = dis(gen);
        skipList.insert(key, value);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Insertion time: " << duration << "ms" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    
    // 查找操作
    for (int i = 0; i < numOperations; i++) {
        int key = dis(gen);
        skipList.search(key);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Search time: " << duration << "ms" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    
    // 删除操作
    for (int i = 0; i < numOperations; i++) {
        int key = dis(gen);
        skipList.remove(key);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Deletion time: " << duration << "ms" << std::endl;
}


int main() {
    SkipList skipList;
    
    skipList.insert(1, 10);
    skipList.insert(3, 30);
    skipList.insert(5, 50);
    skipList.insert(7, 70);
    skipList.insert(9, 90);
    skipList.insert(11, 110);
    
    // 查找操作
    std::cout << "Searching for key 3: " << skipList.search(3) << std::endl; // 输出：30
    std::cout << "Searching for key 9: " << skipList.search(9) << std::endl; // 输出：90
    std::cout << "Searching for key 6: " << skipList.search(6) << std::endl; // 输出：-1 (表示未找到)
    
    // 删除操作
    skipList.remove(3);
    std::cout << "Searching for key 3 after removing: " << skipList.search(3) << std::endl; // 输出：-1 (表示未找到)
    
    // 插入新元素
    skipList.insert(6, 60);
    std::cout << "Searching for key 6 after inserting: " << skipList.search(6) << std::endl; // 输出：60
    
    int numOperations = 1000000; // 可根据需要调整操作次数
    testSkipListPerformance(numOperations);
    
    return 0;
}

