#include "token.hh"

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
        return token::keywords.at(ident);
    }
    return token::IDENT;
}
