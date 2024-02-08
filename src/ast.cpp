#include "ast.hh"

std::string Program::token_literal() {
    if (this->statements->size() > 0) {
        return this->statements->at(0)->token_literal();
    } else {
        return "";
    }
}

std::string LetStatement::token_literal() {
    return this->token.getLiteral();
}

std::string LetStatement::statement_node() {
    return "LetStatement";
}

std::string Identifier::token_literal() {
    return this->token.getLiteral();
}

std::string Identifier::expression_node() {
    return "Identifier";
}



