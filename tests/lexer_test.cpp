#include "lexer.hh"
#include <gtest/gtest.h>
#include <iostream>

TEST(lexer, test_next_token) {
    std::string input =
        "let five = 5;"
        "let ten = 10;"
        "let add = fn(x, y) {"
        "x + y;"
        "};"
        "\n"
        "!-/*5;"
        "5 < 10 > 5;"
        "if (5 < 10) {"
        "return true;"
        "} else {"
        "return false;"
        "}"
        "\n"
        "10 == 10;"
        "10 != 9;";

    std::vector<Token> tests = {   
        Token(token::LET, "let"),
        Token(token::IDENT, "five"),
        Token(token::ASSIGN, "="),
        Token(token::INT, "5"),
        Token(token::SEMICOLON, ";"),
        Token(token::LET, "let"),
        Token(token::IDENT, "ten"),
        Token(token::ASSIGN, "="),
        Token(token::INT, "10"),
        Token(token::SEMICOLON, ";"),
        Token(token::LET, "let"),
        Token(token::IDENT, "add"),
        Token(token::ASSIGN, "="),
        Token(token::FUNCTION, "fn"),
        Token(token::LPAREN, "("),
        Token(token::IDENT, "x"),
        Token(token::COMMA, ","),
        Token(token::IDENT, "y"),
        Token(token::RPAREN, ")"),
        Token(token::LBRACE, "{"),
        Token(token::IDENT, "x"),
        Token(token::PLUS, "+"),
        Token(token::IDENT, "y"),
        Token(token::SEMICOLON, ";"),
        Token(token::RBRACE, "}"),
        Token(token::SEMICOLON, ";"),
        Token(token::BANG, "!"),
        Token(token::MINUS, "-"),
        Token(token::SLASH, "/"),
        Token(token::ASTERISK, "*"),
        Token(token::INT, "5"),
        Token(token::SEMICOLON, ";"),
        Token(token::INT, "5"),
        Token(token::LT, "<"),
        Token(token::INT, "10"),
        Token(token::GT, ">"),
        Token(token::INT, "5"),
        Token(token::SEMICOLON, ";"),
        Token(token::IF, "if"),
        Token(token::LPAREN, "("),
        Token(token::INT, "5"),
        Token(token::LT, "<"),
        Token(token::INT, "10"),
        Token(token::RPAREN, ")"),
        Token(token::LBRACE, "{"),
        Token(token::RETURN, "return"),
        Token(token::TRUE, "true"),
        Token(token::SEMICOLON, ";"),
        Token(token::RBRACE, "}"),
        Token(token::ELSE, "else"),
        Token(token::LBRACE, "{"),
        Token(token::RETURN, "return"),
        Token(token::FALSE, "false"),
        Token(token::SEMICOLON, ";"),
        Token(token::RBRACE, "}"),
        Token(token::INT, "10"),
        Token(token::EQ, "=="),
        Token(token::INT, "10"),
        Token(token::SEMICOLON, ";"),
        Token(token::INT, "10"),
        Token(token::NOT_EQ, "!="),
        Token(token::INT, "9"),
        Token(token::SEMICOLON, ";"),
        Token(token::EOF_, ""),
    };

    Lexer l = Lexer(input);
    for (int i = 0; i < tests.size(); i++) {
        Token tok = l.nextToken();
        EXPECT_EQ(tok.getType(), tests[i].getType());
        EXPECT_EQ(tok.getLiteral(), tests[i].getLiteral());
    }
}
