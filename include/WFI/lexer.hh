#ifndef LEXER_H
#define LEXER_H

#include "token.hh"

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
    Token NextToken();
    bool isLetter(char ch);
    bool isDigit(char ch);
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();
    char peekChar();
};

#endif // LEXER_H