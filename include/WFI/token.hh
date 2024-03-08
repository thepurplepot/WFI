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
    Token(token_t type, std::string literal) : type(type), literal(literal) {};
    ~Token();
    token_t getType();
    std::string getLiteral();
    static token_t lookupIdent(std::string ident);
};

namespace token {
    static const token_t ILLEGAL = "ILLEGAL";
    static const token_t EOF_ = "EOF";
    // Identifiers + literals
    static const token_t IDENT = "IDENT"; // add, foobar, x, y, ...
    static const token_t INT = "INT"; // 1343456
    // Operators
    static const token_t ASSIGN = "=";
    static const token_t PLUS = "+";
    static const token_t MINUS = "-";
    static const token_t BANG = "!";
    static const token_t ASTERISK = "*";
    static const token_t SLASH = "/";
    static const token_t LT = "<";
    static const token_t GT = ">";
    static const token_t EQ = "==";
    static const token_t NOT_EQ = "!=";
    // Delimiters
    static const token_t COMMA = ",";
    static const token_t SEMICOLON = ";";
    static const token_t LPAREN = "(";
    static const token_t RPAREN = ")";
    static const token_t LBRACE = "{";
    static const token_t RBRACE = "}";
    // Keywords
    static const token_t FUNCTION = "FUNCTION";
    static const token_t LET = "LET";
    static const token_t TRUE = "TRUE";
    static const token_t FALSE = "FALSE";
    static const token_t IF = "IF";
    static const token_t ELSE = "ELSE";
    static const token_t RETURN = "RETURN";

    static const std::map<std::string, token_t> keywords = {
        {"fn", FUNCTION},
        {"let", LET},
        {"true", TRUE},
        {"false", FALSE},
        {"if", IF},
        {"else", ELSE},
        {"return", RETURN},
    };
} // namespace Token