#include "ast.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

TEST(ast, test_string) {
    std::vector<Statement*> statements = {
        new LetStatement(
            Token(token::LET, "let"),
            new Identifier(Token(token::IDENT, "myVar"), "myVar"),
            new Identifier(Token(token::IDENT, "anotherVar"), "anotherVar")
        )
    };
    std::string expected = "let myVar = anotherVar;";


    Program *program = new Program(&statements);
    EXPECT_EQ(program->string(), expected) << "program.String() wrong. got=" << program->string() << std::endl;
}