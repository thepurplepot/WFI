#include <iostream>
#include <string>
#include "repl.hh"
#include "lexer.hh"
#include "parser.hh"

void repl::Start() {
    while(true) {
        std::string input;
        std::cout << repl::PROMPT;
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        Lexer* l = new Lexer(input);
        Parser* p = new Parser(l);

        Program* program = p->parseProgram();
        if (p->getErrors().size() > 0) {
            printParserErrors(p->getErrors());
            continue;
        }

        std::cout << program->string() << std::endl;
    }
}

void repl::printParserErrors(std::vector<std::string> errors) {
    std::cout << "Woops! We ran into some monkey business here!" << std::endl;
    std::cout << " parser errors:" << std::endl;
    for (auto err : errors) {
        std::cout << err << std::endl;
    }
}