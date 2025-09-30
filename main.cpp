#include "avl_tree.h"
#include <cassert>
#include <iostream>
using namespace std;

void test_insert_and_inorder() {
    AVLTree tree;
    tree.insert("30", "thirty");
    tree.insert("20", "twenty");
    tree.insert("40", "forty");
    tree.insert("10", "ten");

    std::string inorder = tree.inorder();
    std::cout << "Inorder after insertions: " << inorder << std::endl;
    // Lexicographic order: "10", "20", "30", "40"
    assert(inorder == " (10, ten)  (20, twenty)  (30, thirty)  (40, forty) ");
}

void test_search() {
    AVLTree tree;
    tree.insert("alpha", "A");
    tree.insert("beta", "B");
    tree.insert("gamma", "G");

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

    tree.remove("05"); // leaf node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting leaf: " << inorder << std::endl;
    assert(inorder == " (10, ten)  (20, twenty) ");
    assert(tree.search("05") == false);
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

    tree.remove("70"); // internal node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting internal: " << inorder << std::endl;
    // Lexicographic order still applies
    assert(inorder == " (20, twenty)  (30, thirty)  (40, forty)  (50, fifty)  (60, sixty)  (80, eighty) ");
    assert(tree.search("70") == false);
}

void test_delete_root() {
    AVLTree tree;
    tree.insert("2", "two");
    tree.insert("1", "one");
    tree.insert("3", "three");

    tree.remove("2"); // root node
    std::string inorder = tree.inorder();
    std::cout << "Inorder after deleting root: " << inorder << std::endl;
    // Lexicographic order: "1", "3"
    assert(inorder == " (1, one)  (3, three) ");
    assert(tree.search("2") == false);
}

void run_avl_tests() {
    test_insert_and_inorder();
    test_search();
    test_delete_leaf();
    test_delete_internal();
    test_delete_root();
    std::cout << "✅ All AVL tree tests passed!" << std::endl;
}

int main() {

    run_avl_tests();

    return 0;
}
