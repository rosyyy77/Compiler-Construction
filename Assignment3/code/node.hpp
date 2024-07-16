#ifndef CSC4180_NODE_HPP
#define CSC4180_NODE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


struct Node {
    std::string name;
    std::vector<Node*> children;

    Node(const std::string s) {
        name=s;
    }

    void append_child(Node* child) {
        this->children.push_back(child);
    }
};

#endif  // CSC4180_NODE_HPP
