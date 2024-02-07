#include "token.hh"

Token::Token(token_t type, std::string literal) {
    this->type = type;
    this->literal = literal;
}

Token::~Token() {
}

token_t Token::getType() {
    return this->type;
}

std::string Token::getLiteral() {
    return this->literal;
}

token_t Token::lookupIdent(std::string ident) {
    if (token::keywords.find(ident) != token::keywords.end()) {
        return token::keywords[ident];
    }
    return token::IDENT;
}
