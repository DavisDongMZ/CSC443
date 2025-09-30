#ifndef KVDATABASE_AVL_UNIT_TESTS_H
#define KVDATABASE_AVL_UNIT_TESTS_H

#include "avl_tree.h"
#include <cassert>
#include <iostream>

void test_insert_and_inorder() {
    AVLTree tree;
    assert(tree.get_size() == 0);

    tree.insert("30", "thirty");
    tree.insert("20", "twenty");
    tree.insert("40", "forty");
    tree.insert("10", "ten");

    assert(tree.get_size() == 4);

    std::string inorder = tree.inorder();
    std::cout << "Inorder after insertions: " << inorder << std::endl;
    // Lexicographic order: "10", "20", "30", "40"
    assert(inorder == " (10, ten)  (20, twenty)  (30, thirty)  (40, forty) ");
}

void test_search() {
    AVLTree tree;
    assert(tree.get_size() == 0);

    tree.insert("alpha", "A");
    tree.insert("beta", "B");
    tree.insert("gamma", "G");

    assert(tree.get_size() == 3);

    assert(tree.search("alpha") == true);
    assert(tree.search("beta") == true);
    assert(tree.search("gamma") == true);
    assert(tree.search("delta") == false);
}

void test_delete_leaf() {
    AVLTree tree;
    tree.insert("10", "ten");
    tree.insert("20", "twenty");
    tree.insert("05", "five"); // ensure "05" comes before "10"

    assert(tree.get_size() == 3);

    tree.remove("05"); // leaf node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting leaf: " << inorder << std::endl;
    assert(inorder == " (10, ten)  (20, twenty) ");
    assert(tree.search("05") == false);
    assert(tree.get_size() == 2);
}

void test_delete_internal() {
    AVLTree tree;
    tree.insert("50", "fifty");
    tree.insert("30", "thirty");
    tree.insert("70", "seventy");
    tree.insert("20", "twenty");
    tree.insert("40", "forty");
    tree.insert("60", "sixty");
    tree.insert("80", "eighty");

    assert(tree.get_size() == 7);

    tree.remove("70"); // internal node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting internal: " << inorder << std::endl;
    assert(inorder == " (20, twenty)  (30, thirty)  (40, forty)  (50, fifty)  (60, sixty)  (80, eighty) ");
    assert(tree.search("70") == false);
    assert(tree.get_size() == 6);
}

void test_delete_root() {
    AVLTree tree;
    tree.insert("2", "two");
    tree.insert("1", "one");
    tree.insert("3", "three");

    assert(tree.get_size() == 3);

    tree.remove("2"); // root node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting root: " << inorder << std::endl;
    // Lexicographic order: "1", "3"
    assert(inorder == " (1, one)  (3, three) ");
    assert(tree.search("2") == false);
    assert(tree.get_size() == 2);
}

void test_max_size() {
    AVLTree tree(2); // only allow 2 items

    assert(tree.insert("a", "A") == true);
    assert(tree.insert("b", "B") == true);
    assert(tree.get_size() == 2);

    // should fail since max size is 2
    assert(tree.insert("c", "C") == false);
    assert(tree.get_size() == 2);

    tree.remove("a");
    assert(tree.get_size() == 1);

    // now insertion should work again
    assert(tree.insert("c", "C") == true);
    assert(tree.get_size() == 2);
}

void test_insert_overwrite() {
    AVLTree tree;
    assert(tree.get_size() == 0);

    assert(tree.insert("key1", "value1") == true);
    assert(tree.get_size() == 1);

    // inserting the same key should overwrite, not increase size
    assert(tree.insert("key1", "value2") == true);
    assert(tree.get_size() == 1);

    std::string inorder = tree.inorder();
    std::cout << "Inorder after overwrite: " << inorder << std::endl;
    assert(inorder == " (key1, value2) ");
}

void run_avl_tests() {
    test_insert_and_inorder();
    test_search();
    test_delete_leaf();
    test_delete_internal();
    test_delete_root();
    test_max_size();
    test_insert_overwrite();
    std::cout << "✅ All AVL tree tests passed!" << std::endl;
}

#endif