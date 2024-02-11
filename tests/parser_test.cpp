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

void testIntegerLiteral(Expression* il, int value) {
    IntegerLiteral* integ = dynamic_cast<IntegerLiteral*>(il);
    if(integ == nullptr) {
        FAIL() << "il not *ast.IntegerLiteral. got=" << il << std::endl;
    }
    if(integ->value != value) {
        FAIL() << "integ.Value not " << value << ". got=" << integ->value << std::endl;
    }
    if(integ->token_literal() != std::to_string(value)) {
        FAIL() << "integ.TokenLiteral not " << value << ". got=" << integ->token_literal() << std::endl;
    }
}


TEST(parser, test_let_statements) {
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

TEST(parser, test_return_statements) {
    std::string input =
        "return 5;"
        "return 10;"
        "return 993322;";

    Lexer l = Lexer(input);
    Parser p = Parser(&l);

    Program* program = p.parseProgram();
    checkParserErrors(&p);

    if(program == nullptr) 
        FAIL() << "ParseProgram() returned nullptr" << std::endl;
    if(program->statements->size() != 3) 
        FAIL() << "program.Statements does not contain 3 statements. got=" << program->statements->size() << std::endl;

    for(int i = 0; i < program->statements->size(); i++) {
        Statement* stmt = program->statements->at(i);

        EXPECT_EQ(stmt->token_literal(), "return") << "stmt.TokenLiteral not 'return'. got=" << stmt->token_literal() << std::endl;

        ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(stmt);
        EXPECT_FALSE(returnStmt == nullptr) << "stmt not *ast.ReturnStatement. got=" << stmt << std::endl;
    }
}

TEST(parser, test_identifier_expression) {
    std::string input = "foobar;";

    Lexer l = Lexer(input);
    Parser p = Parser(&l);

    Program* program = p.parseProgram();
    checkParserErrors(&p);

    if(program == nullptr) 
        FAIL() << "ParseProgram() returned nullptr" << std::endl;
    if(program->statements->size() != 1)
        FAIL() << "program.Statements does not contain 1 statements. got=" << program->statements->size() << std::endl;

    Statement* stmt = program->statements->at(0); 
    EXPECT_EQ(stmt->token_literal(), "foobar") << "stmt.TokenLiteral not 'foobar'. got=" << stmt->token_literal() << std::endl;

    ExpressionStatement* expStmt = dynamic_cast<ExpressionStatement*>(stmt);
    EXPECT_FALSE(expStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << stmt << std::endl;

    Identifier* ident = dynamic_cast<Identifier*>(expStmt->expression);
    EXPECT_FALSE(ident == nullptr) << "expStmt.Expression not *ast.Identifier. got=" << expStmt->expression << std::endl;
    EXPECT_EQ(ident->value, "foobar") << "ident.Value not 'foobar'. got=" << ident->value << std::endl;
    EXPECT_EQ(ident->token_literal(), "foobar") << "ident.TokenLiteral not 'foobar'. got=" << ident->token_literal() << std::endl;
}

TEST(parser, test_integer_literal_expression) {
    std::string input = "5;";

    Lexer l = Lexer(input);
    Parser p = Parser(&l);

    Program* program = p.parseProgram();
    checkParserErrors(&p);

    if(program == nullptr) 
        FAIL() << "ParseProgram() returned nullptr" << std::endl;
    if(program->statements->size() != 1)
        FAIL() << "program.Statements does not contain 1 statements. got=" << program->statements->size() << std::endl;

    Statement* stmt = program->statements->at(0); 
    EXPECT_EQ(stmt->token_literal(), "5") << "stmt.TokenLiteral not '5'. got=" << stmt->token_literal() << std::endl;

    ExpressionStatement* expStmt = dynamic_cast<ExpressionStatement*>(stmt);
    EXPECT_FALSE(expStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << stmt << std::endl;

    IntegerLiteral* ident = dynamic_cast<IntegerLiteral*>(expStmt->expression);
    EXPECT_FALSE(ident == nullptr) << "expStmt.Expression not *ast.IntegerLiteral. got=" << expStmt->expression << std::endl;
    EXPECT_EQ(ident->value, 5) << "ident.Value not 5. got=" << ident->value << std::endl;
    EXPECT_EQ(ident->token_literal(), "5") << "ident.TokenLiteral not '5'. got=" << ident->token_literal() << std::endl;
}

TEST(parser, test_prefix_expressions) {
    struct PrefixTest {
        std::string input;
        std::string op;
        int value;
    };

    std::vector<PrefixTest> tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (int i = 0; i < tests.size(); i++) {
        PrefixTest tt = tests[i];
        Lexer l = Lexer(tt.input);
        Parser p = Parser(&l);
        Program* program = p.parseProgram();
        checkParserErrors(&p);

        if(program == nullptr) 
            FAIL() << "ParseProgram() returned nullptr" << std::endl;
        if(program->statements->size() != 1)
            FAIL() << "program.Statements does not contain 1 statements. got=" << program->statements->size() << std::endl;

        Statement* stmt = program->statements->at(0); 
        EXPECT_EQ(stmt->token_literal(), tt.op) << "stmt.TokenLiteral not '" << tt.op << "'. got=" << stmt->token_literal() << std::endl;

        ExpressionStatement* expStmt = dynamic_cast<ExpressionStatement*>(stmt);
        EXPECT_FALSE(expStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << stmt << std::endl;

        PrefixExpression* prefixExp = dynamic_cast<PrefixExpression*>(expStmt->expression);
        EXPECT_FALSE(prefixExp == nullptr) << "expStmt.Expression not *ast.PrefixExpression. got=" << expStmt->expression << std::endl;
        EXPECT_EQ(prefixExp->op, tt.op) << "prefixExp.Operator is not '" << tt.op << "'. got=" << prefixExp->op << std::endl;
        testIntegerLiteral(prefixExp->right, tt.value);
    }
}

TEST(parser, test_infix_expressions) {
    struct InfixTest {
        std::string input;
        int leftValue;
        std::string op;
        int rightValue;
    }; 

    std::vector<InfixTest> tests = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
    };

    for (int i = 0; i < tests.size(); i++) {
        InfixTest tt = tests[i];
        Lexer l = Lexer(tt.input);
        Parser p = Parser(&l);
        Program* program = p.parseProgram();
        checkParserErrors(&p);

        if(program == nullptr) 
            FAIL() << "ParseProgram() returned nullptr" << std::endl;
        if(program->statements->size() != 1)
            FAIL() << "program.Statements does not contain 1 statements. got=" << program->statements->size() << std::endl;

        Statement* stmt = program->statements->at(0); 
        ExpressionStatement* expStmt = dynamic_cast<ExpressionStatement*>(stmt);
        EXPECT_FALSE(expStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << stmt << std::endl;

        InfixExpression* infixExp = dynamic_cast<InfixExpression*>(expStmt->expression);
        EXPECT_FALSE(infixExp == nullptr) << "expStmt.Expression not *ast.InfixExpression. got=" << expStmt->expression << std::endl;
        testIntegerLiteral(infixExp->left, tt.leftValue);
        EXPECT_EQ(infixExp->op, tt.op) << "infixExp.Operator is not '" << tt.op << "'. got=" << infixExp->op << std::endl;
        testIntegerLiteral(infixExp->right, tt.rightValue);
    }
}

TEST(parser, test_operator_precedence) {
    struct OperatorTest {
        std::string input;
        std::string expected;
    };

    std::vector<OperatorTest> tests = {
        {"-a * b;", "((-a) * b)"},
        {"!-a;", "(!(-a))"},
        {"a + b + c;", "((a + b) + c)"},
        {"a + b - c;", "((a + b) - c)"},
        {"a * b * c;", "((a * b) * c)"},
        {"a * b / c;", "((a * b) / c)"},
        {"a + b / c;", "(a + (b / c))"},
        {"a + b * c + d / e - f;", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5;", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4;", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4;", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5;", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
    };

    for (int i = 0; i < tests.size(); i++) {
        OperatorTest tt = tests[i];
        Lexer l = Lexer(tt.input);
        Parser p = Parser(&l);
        Program* program = p.parseProgram();
        checkParserErrors(&p);

        if(program == nullptr) 
            FAIL() << "ParseProgram() returned nullptr" << std::endl;
        if(program->string() != tt.expected)
            FAIL() << "expected=" << tt.expected << ", got=" << program->string() << std::endl;
    }
}