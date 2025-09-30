//
// Created by Zekun Liu on 2025-09-23.
// Adapted from geeksforgeeks.org
//

#include <algorithm>
#include "avl_tree.h"
using namespace std;

int AVLTree::height(const AVLNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

int AVLTree::balanceFactor(const AVLNode* node) {
    if (node == nullptr) return 0;
    return height(node->left) - height(node->right);
}

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* t2 = x->right;

    // rotate
    x->right = y;
    y->left = t2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

AVLNode* AVLTree::insert(AVLNode* node, const string& key, const string& value) {
    if (node == nullptr) return new AVLNode(key, value);

    if (key < node->key)
        node->left = insert(node->left, key, value);
    else if (key > node->key)
        node->right = insert(node->right, key, value);
    else {
        // overwrite key value pair with new value
        node->value = value;
        return node;
    }

    // node inserted as child, need to update height and perform rotations
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance <-1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != nullptr)
        current = current->left;
    return current;
}

AVLNode* AVLTree::deleteNode(AVLNode* root, const string& key) {
    if (root == nullptr) return nullptr;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // 1 or 0 child
        if ((root->left == nullptr) || (root->right == nullptr)) {
            const AVLNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            }
            else
                *root = *temp;
            delete temp;
        } else {
            const AVLNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->value = temp->value;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    if (root == nullptr) return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = balanceFactor(root);

    if (balance > 1 && balanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && balanceFactor(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && balanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

string AVLTree::inorder(AVLNode* root) {
    if (root != nullptr) {
        const string left = inorder(root->left);
        const string right = inorder(root->right);
        return left + " (" + root->key + ", " + root->value + ") " + right;
    }
    return "";
}

bool AVLTree::search(const AVLNode* root, const string& key) {
    if (root == nullptr) return false;
    if (root->key == key) return true;
    if (key < root->key) return search(root->left, key);
    return search(root->right, key);
}

void AVLTree::insert(const string& key,const string& value) {
    root = insert(root, key, value);
}

void AVLTree::remove(const string& key) {
    root = deleteNode(root, key);
}

bool AVLTree::search(const string& key) const {
    return search(root, key);
}

string AVLTree::inorder() const {
    return inorder(root);
}


