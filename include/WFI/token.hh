#ifndef TOKEN_HH
#define TOKEN_HH

#include <string>
#include <map>

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

static std::string ILLEGAL = "ILLEGAL";
//static std::string EOF = "EOF";
// Identifiers + literals
static std::string IDENT = "IDENT"; // add, foobar, x, y, ...
static std::string INT = "INT"; // 1343456
// Operators
static std::string ASSIGN = "=";
static std::string PLUS = "+";
// Delimiters
static std::string COMMA = ",";
static std::string SEMICOLON = ";";
static std::string LPAREN = "(";
static std::string RPAREN = ")";
static std::string LBRACE = "{";
static std::string RBRACE = "}";
// Keywords
static std::string FUNCTION = "FUNCTION";
static std::string LET = "LET";

static std::map<std::string, std::string> keywords = {
    {"fn", FUNCTION},
    {"let", LET},
};

#endif // TOKEN_HH