#include <iostream>
#include <string>
#include "repl.hh"
#include "lexer.hh"
#include "parser.hh"
#include "evaluator.hh"

void repl::Start() {
    object::Environment *env = new object::Environment();

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

        object::Object *evaluated = evaluator::eval(program, env);
        if (evaluated != nullptr) {
            std::cout << evaluated->inspect() << std::endl;
        }
    }
}

void repl::printParserErrors(std::vector<std::string> errors) {
    std::cout << "Woops! We ran into some monkey business here!" << std::endl;
    std::cout << " parser errors:" << std::endl;
    for (auto err : errors) {
        std::cout << err << std::endl;
    }
}