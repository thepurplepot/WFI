#include <iostream>
#include <string>
#include "repl.hh"
#include "lexer.hh"


void repl::Start() {
    while(true) {
        std::string input;
        std::cout << repl::PROMPT;
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        Lexer l = Lexer(input);
        for(Token tok = l.NextToken(); tok.getType() != token::EOF_; tok = l.NextToken()) {
            std::cout << "Type: " << tok.getType() << " Literal: " << tok.getLiteral() << std::endl;
        }

    }
}