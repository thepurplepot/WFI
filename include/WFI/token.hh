#include <string>
#include <map>
#pragma once

typedef std::string token_t;

class Token {
private:
    /* data */
    token_t type;
    std::string literal;
public:
    Token() = default;
    Token(token_t type, std::string literal);
    ~Token();
    token_t getType();
    std::string getLiteral();
    static token_t lookupIdent(std::string ident);
};

namespace token {
    static token_t ILLEGAL = "ILLEGAL";
    static token_t EOF_ = "EOF";
    // Identifiers + literals
    static token_t IDENT = "IDENT"; // add, foobar, x, y, ...
    static token_t INT = "INT"; // 1343456
    // Operators
    static token_t ASSIGN = "=";
    static token_t PLUS = "+";
    static token_t MINUS = "-";
    static token_t BANG = "!";
    static token_t ASTERISK = "*";
    static token_t SLASH = "/";
    static token_t LT = "<";
    static token_t GT = ">";
    static token_t EQ = "==";
    static token_t NOT_EQ = "!=";
    // Delimiters
    static token_t COMMA = ",";
    static token_t SEMICOLON = ";";
    static token_t LPAREN = "(";
    static token_t RPAREN = ")";
    static token_t LBRACE = "{";
    static token_t RBRACE = "}";
    // Keywords
    static token_t FUNCTION = "FUNCTION";
    static token_t LET = "LET";
    static token_t TRUE = "TRUE";
    static token_t FALSE = "FALSE";
    static token_t IF = "IF";
    static token_t ELSE = "ELSE";
    static token_t RETURN = "RETURN";

    static std::map<std::string, token_t> keywords = {
        {"fn", FUNCTION},
        {"let", LET},
        {"true", TRUE},
        {"false", FALSE},
        {"if", IF},
        {"else", ELSE},
        {"return", RETURN},
    };
} // namespace Token