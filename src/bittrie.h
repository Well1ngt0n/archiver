#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <iostream>

const size_t BIN_ALPHABET_SIZE = 2;

template <typename T>
struct Node {
    Node *next[BIN_ALPHABET_SIZE];
    bool is_terminal = false;
    T value;
    Node();
    explicit Node(T symbol);
};

template <typename T>
class BinTrie {
private:
    Node<T> *root_;
    Node<T> *cur_;
    void Dfs(Node<T> *node, std::vector<bool> &cur_path, std::map<T, std::vector<bool>> &paths);
    void DeleteNode(Node<T> *node);

public:
    explicit BinTrie(Node<T> *root);
    BinTrie();
    ~BinTrie();
    void InitTraversal();
    bool NextInTraversal(bool symbol);
    T GetTerminal();
    std::map<T, std::vector<bool>> GetAllPaths();
    void Add(const std::vector<bool> &path, T value);
    void ChangeRoot(Node<T> *node);
};

template <typename T>
bool operator<(Node<T> a, Node<T> b) {
    return a.value < b.value;
}

template <typename T>
Node<T>::Node() {
    is_terminal = false;
    for (size_t i = 0; i < BIN_ALPHABET_SIZE; i++) {
        next[i] = nullptr;
    }
    value = T();
}

template <typename T>
Node<T>::Node(T symbol) : value(symbol) {
    for (size_t i = 0; i < BIN_ALPHABET_SIZE; i++) {
        next[i] = nullptr;
    }
    is_terminal = true;
}

template <typename T>
BinTrie<T>::BinTrie() {
    root_ = new Node<T>();
}

template <typename T>
void BinTrie<T>::ChangeRoot(Node<T> *node) {
    DeleteNode(root_);
    root_ = node;
}

template <typename T>
void BinTrie<T>::DeleteNode(Node<T> *node) {
    if (node == nullptr) {
        return;
    }
    for (size_t i = 0; i < BIN_ALPHABET_SIZE; i++) {
        if (node->next[i] != nullptr) {
            DeleteNode(node->next[i]);
        }
    }
    if (node != nullptr) {
        delete node;
    }
    node = nullptr;
}

template <typename T>
BinTrie<T>::~BinTrie() {
    DeleteNode(root_);
}

template <typename T>
BinTrie<T>::BinTrie(Node<T> *root) : root_(root) {
}

template <typename T>
void BinTrie<T>::InitTraversal() {
    cur_ = root_;
}

template <typename T>
bool BinTrie<T>::NextInTraversal(bool symbol) {
    if (cur_ == nullptr) {
        return true;
    }
    if (cur_->next[symbol] != nullptr) {
        cur_ = cur_->next[symbol];
        return cur_->is_terminal;
    } else {
        cur_ = nullptr;
        return false;
    }
}

template <typename T>
T BinTrie<T>::GetTerminal() {
    if (cur_ == nullptr) {
        return T();
    }
    if (cur_->is_terminal) {
        return cur_->value;
    } else {
        return T();
    }
}

template <typename T>
void BinTrie<T>::Dfs(Node<T> *node, std::vector<bool> &cur_path, std::map<T, std::vector<bool>> &paths) {
    if (node == nullptr) {
        return;
    }
    if (node->is_terminal) {
        paths[node->value] = cur_path;
        return;
    }
    for (size_t bit = 0; bit < BIN_ALPHABET_SIZE; bit++) {
        if (node->next[bit] != nullptr) {
            cur_path.push_back(bit);
            Dfs(node->next[bit], cur_path, paths);
            cur_path.pop_back();
        }
    }
}

template <typename T>
std::map<T, std::vector<bool>> BinTrie<T>::GetAllPaths() {
    std::map<T, std::vector<bool>> paths;
    std::vector<bool> cur_path;
    if (root_ != nullptr) {
        Dfs(root_, cur_path, paths);
    }
    return paths;
}

template <typename T>
void BinTrie<T>::Add(const std::vector<bool> &path, T value) {
    Node<T> *node = root_;
    for (size_t i = 0; i < path.size(); i++) {
        if (node->next[path[i]] == nullptr) {
            node->next[path[i]] = new Node<T>();
        }
        node = node->next[path[i]];
    }
    node->is_terminal = true;
    node->value = value;
}
