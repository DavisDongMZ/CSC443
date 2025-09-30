//
// Created by Zekun Liu on 2025-09-23.
// Adapted from geeksforgeeks.org
//

#ifndef KVDATABASE_AVL_TREE_H
#define KVDATABASE_AVL_TREE_H

#include <string>
#include <algorithm>
#include <utility>
using namespace std;

class AVLNode {
public:
    string key;
    string value;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(string k, string v)
        : key(std::move(k))
        , value(std::move(v))
        , left(nullptr)
        , right(nullptr)
        , height(1) {}
};

class AVLTree {
    AVLNode* root;
    int size;
    int max_size;
    static int height(const AVLNode* node);
    static int balanceFactor(const AVLNode* node);

    static AVLNode* rightRotate(AVLNode* y);

    static AVLNode* leftRotate(AVLNode* x);

    static pair<AVLNode*, bool> insert(AVLNode* node, const string& key, const string& value);
    static AVLNode* minValueNode(AVLNode* node);

    static pair<AVLNode*, bool> deleteNode(AVLNode* node, const string& key);

    static bool search(const AVLNode *root, const string& key);

    static string inorder(AVLNode* root);
public:
    AVLTree()
        : root(nullptr)
        , size(0)
        , max_size(numeric_limits<int>::max()) {}
    explicit AVLTree(const int max)
        : root(nullptr)
        , size(0)
        , max_size(max){}
    // ~AVLTree();
    bool insert(const string& key, const string& value);
    void remove(const string& key);
    [[nodiscard]] bool search(const string& key) const;
    [[nodiscard]] string inorder() const;
    [[nodiscard]] int get_size() const;
};

#endif //KVDATABASE_AVL_TREE_H
