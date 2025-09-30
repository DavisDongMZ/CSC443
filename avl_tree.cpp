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

pair<AVLNode*, bool> AVLTree::insert(AVLNode* node, const string& key, const string& value) {
    if (node == nullptr) {
        // didn't put this inside the initialization of the pair because linter detects this as a memory leak
        auto new_leaf = new AVLNode(key, value);
        return {new_leaf, true};
    }

    bool insertion_outcome;
    if (key < node->key) {
        auto res = insert(node->left, key, value);
        node->left = res.first;
        insertion_outcome = res.second;
    }
    else if (key > node->key) {
        auto res = insert(node->right, key, value);
        node->right = res.first;
        insertion_outcome = res.second;
    }
    else {
        // overwrite key value pair with new value
        node->value = value;
        return {node, false};
    }

    // node inserted as child, need to update height and perform rotations
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);
    if (balance > 1 && key < node->left->key)
        return {rightRotate(node), insertion_outcome};
    if (balance < -1 && key > node->right->key)
        return {leftRotate(node), insertion_outcome};
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return {rightRotate(node), insertion_outcome};
    }
    if (balance <-1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return {leftRotate(node), insertion_outcome};
    }

    return {node, insertion_outcome};
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != nullptr)
        current = current->left;
    return current;
}

pair<AVLNode*, bool> AVLTree::deleteNode(AVLNode* root, const string& key) {
    if (root == nullptr) return {nullptr, false};

    bool deletion_outcome;
    if (key < root->key) {
        auto res = deleteNode(root->left, key);
        root->left = res.first;
        deletion_outcome = res.second;
    }
    else if (key > root->key) {
        auto res = deleteNode(root->right, key);
        root->right = res.first;
        deletion_outcome = res.second;
    }
    else {
        // 1 or 0 child
        if (root->left == nullptr || root->right == nullptr) {
            const AVLNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
                deletion_outcome = true;
            }
            else
                *root = *temp;
                deletion_outcome = true;
            delete temp;
        } else {
            const AVLNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->value = temp->value;
            deletion_outcome = true;
            // root deleted, now remove copy from right node
            auto res = deleteNode(root->right, temp->key);
            root->right = res.first;
            // we don't need the deletion outcome from this action since we've already performed a deletion
        }
    }

    if (root == nullptr) return {root, deletion_outcome};

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = balanceFactor(root);

    if (balance > 1 && balanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return {rightRotate(root), deletion_outcome};
    }

    if (balance < -1 && balanceFactor(root->right) <= 0)
        return {leftRotate(root), deletion_outcome};

    if (balance < -1 && balanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return {leftRotate(root), deletion_outcome};
    }

    return {root, deletion_outcome};
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

bool AVLTree::insert(const string& key,const string& value) {
    if (size == max_size) return false;
    auto res = insert(root, key, value);
    root = res.first;
    if (res.second == true) {
        size++;
    }
    return true;
}

void AVLTree::remove(const string& key) {
    auto res = deleteNode(root, key);
    root = res.first;
    if (size > 0 && res.second) size--;
}

bool AVLTree::search(const string& key) const {
    return search(root, key);
}

string AVLTree::inorder() const {
    return inorder(root);
}

int AVLTree::get_size() const {
    return size;
}
