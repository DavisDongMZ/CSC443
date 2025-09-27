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
    static int height(const AVLNode* node);
    static int balanceFactor(const AVLNode* node);

    static AVLNode* rightRotate(AVLNode* y);

    static AVLNode* leftRotate(AVLNode* x);

    static AVLNode* insert(AVLNode* node, const string& key, const string& value);
    static AVLNode* minValueNode(AVLNode* node);

    static AVLNode* deleteNode(AVLNode* node, const string& key);

    bool search(const AVLNode *root, const string& key);

    static string inorder(AVLNode* root);
public:
    AVLTree()
        : root(nullptr) {}
    // ~AVLTree();
    void insert(const string& key, const string& value);
    void remove(const string& key);
    bool search(const string& key);
    string inorder() const;
};


#endif //KVDATABASE_AVL_TREE_H