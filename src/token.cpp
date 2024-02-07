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
    if (keywords.find(ident) != keywords.end()) {
        return keywords[ident];
    }
    return IDENT;
}
