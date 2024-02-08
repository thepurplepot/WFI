#include "parser.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

void checkParserErrors(Parser *p) {
    std::vector<std::string> errors = p->getErrors();
    int len = errors.size();
    if (len == 0) {
        return;
    }
    for (int i = 0; i < len; i++) {
        std::cout << "parser error: " << errors[i] << std::endl;
    } 
    FAIL() << "parser has " << len << " errors" << std::endl;
}


TEST(test_parser, system) {
    std::string input =
        "let x = 5;"
        "let y = 10;"
        "let foobar = 838383;";

    std::vector<std::string> expectedIdentifiers = {"x", "y", "foobar"};

    Lexer l = Lexer(input);
    Parser p = Parser(&l);

    Program* program = p.parseProgram();
    checkParserErrors(&p);

    if(program == nullptr) 
        FAIL() << "ParseProgram() returned nullptr" << std::endl;
    if(program->statements->size() != 3) 
        FAIL() << "program.Statements does not contain 3 statements. got=" << program->statements->size() << std::endl;

    for(int i = 0; i < expectedIdentifiers.size(); i++) {
        Statement* stmt = program->statements->at(i);

        EXPECT_EQ(stmt->token_literal(), "let") << "stmt.TokenLiteral not 'let'. got=" << stmt->token_literal() << std::endl;

        LetStatement* letStmt = dynamic_cast<LetStatement*>(stmt);
        EXPECT_FALSE(letStmt == nullptr) << "stmt not *ast.LetStatement. got=" << stmt << std::endl;

        EXPECT_EQ(letStmt->name->value, expectedIdentifiers[i]) << "letStmt.Name.Value not '" << expectedIdentifiers[i] << "'. got=" << letStmt->name->value << std::endl;
        EXPECT_EQ(letStmt->name->token_literal(), expectedIdentifiers[i]) << "letStmt.Name.TokenLiteral not '" << expectedIdentifiers[i] << "'. got=" << letStmt->name->token_literal() << std::endl;
    }
}