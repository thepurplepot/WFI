#include "parser.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

template <typename T> std::string type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

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
    EXPECT_FALSE(integ == nullptr) << "Expression not IntegerLiteral. got=" << il << std::endl;
    EXPECT_EQ(integ->value, value) << "integ.Value not " << value << ". got=" << integ->value << std::endl;
    EXPECT_EQ(integ->token_literal(), std::to_string(value)) << "integ.TokenLiteral not " << value << ". got=" << integ->token_literal() << std::endl;
}

void testBooleanLiteral(Expression* exp, bool value) {
    Boolean* bo = dynamic_cast<Boolean*>(exp);
    EXPECT_FALSE(bo == nullptr) << "Expression not Boolean. got=" << exp << std::endl;
    EXPECT_EQ(bo->value, value) << "bo.Value not " << value << ". got=" << bo->value << std::endl;
    EXPECT_EQ(bo->token_literal(), value ? "true" : "false") << "bo.TokenLiteral not " << value << ". got=" << bo->token_literal() << std::endl;
}

void testIdentifier(Expression* exp, std::string value) {
    Identifier* ident = dynamic_cast<Identifier*>(exp); 
    EXPECT_FALSE(ident == nullptr) << "Expression not Identifier. got=" << exp << std::endl;
    EXPECT_EQ(ident->value, value) << "ident.Value not " << value << ". got=" << ident->value << std::endl;
    EXPECT_EQ(ident->token_literal(), value) << "ident.TokenLiteral not" << value << ". got=" << ident->token_literal() << std::endl; 
}

template<typename T>
void testLiteralExpression(Expression* exp, T expected) {
    FAIL() << type_name<decltype(expected)>() << " type of exp not handled. got =" << exp;
}

template<>
void testLiteralExpression<int>(Expression* exp, int expected) {
    testIntegerLiteral(exp, expected);
}

template<>
void testLiteralExpression<long>(Expression* exp, long expected) {
    testIntegerLiteral(exp, expected);
}

template<>
void testLiteralExpression<std::string>(Expression* exp, std::string expected) {
    testIdentifier(exp, expected);
}

template<>
void testLiteralExpression<bool>(Expression* exp, bool expected) {
    testBooleanLiteral(exp, expected);
}

template<>
void testLiteralExpression<char*>(Expression* exp, char* expected) {
    testIdentifier(exp, expected);
}

template<typename R>
void testPrefixExpression(Expression* exp, std::string op, R right) {
    PrefixExpression* opExp = dynamic_cast<PrefixExpression*>(exp);
    EXPECT_FALSE(opExp == nullptr) << "Expression not PrefixExpression. got=" << exp << std::endl; 
    EXPECT_EQ(opExp->op, op) << "Operator is not " << op << ". got=" << opExp->op << std::endl; 
    testLiteralExpression<R>(opExp->right, right);
}

template<typename L, typename R>
void testInfixExpression(Expression* exp, L left, std::string op, R right) {
    InfixExpression* opExp = dynamic_cast<InfixExpression*>(exp);
    EXPECT_FALSE(opExp == nullptr) << "Expression not InfixExpression. got=" << exp << std::endl; 
    testLiteralExpression<L>(opExp->left, left);
    EXPECT_EQ(opExp->op, op) << "Operator is not " << op << ". got=" << opExp->op << std::endl; 
    testLiteralExpression<R>(opExp->right, right);
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

    testIdentifier(expStmt->expression, "foobar");
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

    testIntegerLiteral(expStmt->expression, 5);
}

TEST(parser, test_prefix_expressions) {
    struct PrefixTest {
        std::string input;
        std::string op;
        //TODO add bool testing
        int value;
    };

    std::vector<PrefixTest> tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        //{"!true", "!", true},
        //{"!false", "!", false}
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

        testPrefixExpression<int>(expStmt->expression, tt.op, tt.value);
    }
}

TEST(parser, test_infix_expressions) {
    
    struct InfixTest {
        std::string input;
        //TODO add bool testing
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
        //{"true == true", true, "==", true},
        //{"true != false", true, "!=", false},
        //{"false == false", false, "==", false}
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

        testInfixExpression(expStmt->expression, tt.leftValue, tt.op, tt.rightValue);
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
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
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
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"},
        {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
        {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"}
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

TEST(parser, test_if_expression) {
    std::string input = "if (x < y) { x }";

    Lexer l = Lexer(input);
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

    IfExpression* ifExp = dynamic_cast<IfExpression*>(expStmt->expression);
    EXPECT_FALSE(ifExp == nullptr) << "exp not *ast.IfExpression. got=" << expStmt->expression << std::endl;

    testInfixExpression(ifExp->condition, std::string("x"), "<", std::string("y"));
    if(ifExp->consequence->statements->size() != 1)
        FAIL() << "consequence is not 1 statements. got=" << ifExp->consequence->statements->size() << std::endl;
    
    Statement* consequence = ifExp->consequence->statements->at(0);
    ExpressionStatement* consExpStmt = dynamic_cast<ExpressionStatement*>(consequence);
    EXPECT_FALSE(consExpStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << consequence << std::endl;
    testIdentifier(consExpStmt->expression, "x");
    EXPECT_TRUE(ifExp->alternative == nullptr) << "ifExp.Alternative is not null. got=" << ifExp->alternative << std::endl;
}

TEST(parser, test_if_else_expression) {
    std::string input = "if (x < y) { x } else { y }";

    Lexer l = Lexer(input);
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

    IfExpression* ifExp = dynamic_cast<IfExpression*>(expStmt->expression);
    EXPECT_FALSE(ifExp == nullptr) << "exp not *ast.IfExpression. got=" << expStmt->expression << std::endl;

    testInfixExpression(ifExp->condition, std::string("x"), "<", std::string("y"));
    if(ifExp->consequence->statements->size() != 1)
        FAIL() << "consequence is not 1 statements. got=" << ifExp->consequence->statements->size() << std::endl;
    
    Statement* consequence = ifExp->consequence->statements->at(0);
    ExpressionStatement* consExpStmt = dynamic_cast<ExpressionStatement*>(consequence);
    EXPECT_FALSE(consExpStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << consequence << std::endl;
    testIdentifier(consExpStmt->expression, "x");

    if(ifExp->alternative->statements->size() != 1)
        FAIL() << "alternative is not 1 statements. got=" << ifExp->alternative->statements->size() << std::endl;
    
    Statement* alternative = ifExp->alternative->statements->at(0);
    ExpressionStatement* altExpStmt = dynamic_cast<ExpressionStatement*>(alternative);
    EXPECT_FALSE(altExpStmt == nullptr) << "stmt not *ast.ExpressionStatement. got=" << alternative << std::endl;
    testIdentifier(altExpStmt->expression, "y");
}

TEST(parser, test_function_literal) {
    std::string input = "fn(x, y) { x + y; }";

    Lexer l = Lexer(input);
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

    FunctionLiteral* func = dynamic_cast<FunctionLiteral*>(expStmt->expression);
    EXPECT_FALSE(func == nullptr) << "exp not *ast.FunctionLiteral. got=" << expStmt->expression << std::endl;

    if(func->parameters.size() != 2)
        FAIL() << "function literal parameters wrong. want 2, got=" << func->parameters.size() << std::endl;
    
    testLiteralExpression<std::string>(func->parameters.at(0), "x");
    testLiteralExpression<std::string>(func->parameters.at(1), "y");

    if(func->body->statements->size() != 1)
        FAIL() << "function.Body.Statements has not 1 statements. got=" << func->body->statements->size() << std::endl;
    
    Statement* bodyStmt = func->body->statements->at(0);
    ExpressionStatement* bodyExpStmt = dynamic_cast<ExpressionStatement*>(bodyStmt);
    EXPECT_FALSE(bodyExpStmt == nullptr) << "function body stmt not *ast.ExpressionStatement. got=" << bodyStmt << std::endl;
    testInfixExpression(bodyExpStmt->expression, std::string("x"), "+", std::string("y"));
}

TEST(parser, function_parameter_parsing) {
    struct FunctionParameterTest {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<FunctionParameterTest> tests = {
        {"fn() {};", {}},
        {"fn(x) {};", {"x"}},
        {"fn(x, y, z) {};", {"x", "y", "z"}}
    };

    for(int i = 0; i < tests.size(); i++) {
        FunctionParameterTest tt = tests[i];
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

        FunctionLiteral* func = dynamic_cast<FunctionLiteral*>(expStmt->expression);
        EXPECT_FALSE(func == nullptr) << "exp not *ast.FunctionLiteral. got=" << expStmt->expression << std::endl;

        if(func->parameters.size() != tt.expectedParams.size())
            FAIL() << "length parameters wrong. want " << tt.expectedParams.size() << ", got=" << func->parameters.size() << std::endl;

        for(int j = 0; j < tt.expectedParams.size(); j++) {
            testLiteralExpression<std::string>(func->parameters.at(j), tt.expectedParams.at(j));
        }
    }
}

TEST(parser, call_expression) {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    Lexer l = Lexer(input);
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

    CallExpression* exp = dynamic_cast<CallExpression*>(expStmt->expression);
    EXPECT_FALSE(exp == nullptr) << "exp not *ast.CallExpression. got=" << expStmt->expression << std::endl;

    testIdentifier(exp->function, "add");
    if(exp->arguments.size() != 3)
        FAIL() << "wrong length of arguments. got=" << exp->arguments.size() << std::endl;
    
    testLiteralExpression<int>(exp->arguments.at(0), 1);
    testInfixExpression(exp->arguments.at(1), 2, "*", 3);
    testInfixExpression(exp->arguments.at(2), 4, "+", 5);
}