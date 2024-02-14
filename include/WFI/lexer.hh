#include "token.hh"
#pragma once

class Lexer {
private:
    /* data */
    std::string input;
    int position;
    int readPosition;
    char ch;
public:
    Lexer(std::string input);
    ~Lexer();
    void readChar();
    Token nextToken();
    bool isLetter(char ch);
    bool isDigit(char ch);
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();
    char peekChar();
};