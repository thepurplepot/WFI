#include <iostream>
#include "repl.hh"

int main() {
    std::cout << "Hello! This is the Fletchlang programming language!" << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;
    repl::Start();
    return 0;
}