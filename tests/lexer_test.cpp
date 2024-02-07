#include "lexer.hh"
#include <gtest/gtest.h>
#include <iostream>

TEST(test_next_token, system) {
    std::string input =
        "let five = 5;"
        "let ten = 10;"
        "let add = fn(x, y) {"
        "x + y;"
        "};";

    std::vector<Token> tests = {   
        Token(LET, "let"),
        Token(IDENT, "five"),
        Token(ASSIGN, "="),
        Token(INT, "5"),
        Token(SEMICOLON, ";"),
        Token(LET, "let"),
        Token(IDENT, "ten"),
        Token(ASSIGN, "="),
        Token(INT, "10"),
        Token(SEMICOLON, ";"),
        Token(LET, "let"),
        Token(IDENT, "add"),
        Token(ASSIGN, "="),
        Token(FUNCTION, "fn"),
        Token(LPAREN, "("),
        Token(IDENT, "x"),
        Token(COMMA, ","),
        Token(IDENT, "y"),
        Token(RPAREN, ")"),
        Token(LBRACE, "{"),
        Token(IDENT, "x"),
        Token(PLUS, "+"),
        Token(IDENT, "y"),
        Token(SEMICOLON, ";"),
        Token(RBRACE, "}"),
        Token(SEMICOLON, ";"),
        Token("EOF", ""),
    };

    Lexer l = Lexer(input);
    for (int i = 0; i < tests.size(); i++) {
        Token tok = l.NextToken();
        EXPECT_EQ(tok.getType(), tests[i].getType());
        EXPECT_EQ(tok.getLiteral(), tests[i].getLiteral());
    }
}
