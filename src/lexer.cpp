#include "lexer.hh"

Lexer::Lexer(std::string input) {
    this->input = input;
    this->position = 0;
    this->readPosition = 0;
    this->ch = input[0];
    this->readChar();
}

Lexer::~Lexer() {
}

void Lexer::readChar() {
    if (this->readPosition >= this->input.length()) {
        this->ch = 0;
    } else {
        this->ch = this->input[this->readPosition];
    }
    this->position = this->readPosition;
    this->readPosition += 1;
}

Token Lexer::NextToken() {
    Token tok;
    this->skipWhitespace();
    switch (this->ch) {
    case '=':
        tok = Token(ASSIGN, {this->ch});
        break;
    case ';':
        tok = Token(SEMICOLON, {this->ch});
        break;
    case '(':
        tok = Token(LPAREN, {this->ch});
        break;
    case ')':
        tok = Token(RPAREN, {this->ch});
        break;
    case ',':
        tok = Token(COMMA, {this->ch});
        break;
    case '+':
        tok = Token(PLUS, {this->ch});
        break;
    case '{':
        tok = Token(LBRACE, {this->ch});
        break;
    case '}':
        tok = Token(RBRACE, {this->ch});
        break;
    case 0:
        tok = Token("EOF", "");
        break;
    default:
        if(isLetter(this->ch)) {
            std::string literal = this->readIdentifier();
            std::string type = Token::lookupIdent(literal);
            tok = Token(type, literal);
            return tok;
        } else if (isdigit(this->ch)) {
            std::string literal = this->readNumber();
            tok = Token(INT, literal);
            return tok;
        } else {
            tok = Token(ILLEGAL, {this->ch});
        }
    }
    this->readChar();
    return tok;
}

bool Lexer::isLetter(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool Lexer::isDigit(char ch) {
    return '0' <= ch && ch <= '9';
}

std::string Lexer::readIdentifier() {
    int position = this->position;
    while (isLetter(this->ch)) {
        this->readChar();
    }
    return this->input.substr(position, this->position - position);
}

std::string Lexer::readNumber() {
    int position = this->position;
    while (isDigit(this->ch)) {
        this->readChar();
    }
    return this->input.substr(position, this->position - position);
}

void Lexer::skipWhitespace() {
    while (this->ch == ' ' || this->ch == '\t' || this->ch == '\n' || this->ch == '\r') {
        this->readChar();
    }
}

char Lexer::peekChar() {
    if (this->readPosition >= this->input.length()) {
        return 0;
    } else {
        return this->input[this->readPosition];
    }
}