#include "lexer.hh"
#include "object.hh"
#include "parser.hh"
#include "evaluator.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

void testIntegerObject(object::Object *obj, int expected) {
    object::Integer *result = dynamic_cast<object::Integer*>(obj);
    ASSERT_TRUE(result != nullptr) << "object is not Integer. got=" << obj->type() << std::endl;
    ASSERT_EQ(result->value, expected) << "object has wrong value. got=" << result->value << ", want=" << expected << std::endl;
}

void testBooleanObject(object::Object *obj, bool expected) {
    object::Boolean *result = dynamic_cast<object::Boolean*>(obj);
    ASSERT_TRUE(result != nullptr) << "object is not Boolean. got=" << obj->type() << std::endl;
    ASSERT_EQ(result->value, expected) << "object has wrong value. got=" << result->value << ", want=" << expected << std::endl;
}

TEST(evaluator, test_eval_integer_expression) {
    struct EvalIntegerTest {
        std::string input;
        int expected;
    };

    std::vector<EvalIntegerTest> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for(auto test : tests) {
        Lexer l = Lexer(test.input);
        Parser p = Parser(&l);
        Program *program = p.parseProgram();

        object::Object *evaluated = evaluator::eval(program);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_eval_boolean_expression) {
    struct EvalBooleanTest {
        std::string input;
        bool expected;
    };

    std::vector<EvalBooleanTest> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };

    for(auto test : tests) {
        Lexer l = Lexer(test.input);
        Parser p = Parser(&l);
        Program *program = p.parseProgram();

        object::Object *evaluated = evaluator::eval(program);
        testBooleanObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_bang_operator) {
    struct BangOperatorTest {
        std::string input;
        bool expected;
    };

    std::vector<BangOperatorTest> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    for(auto test : tests) {
        Lexer l = Lexer(test.input);
        Parser p = Parser(&l);
        Program *program = p.parseProgram();

        object::Object *evaluated = evaluator::eval(program);
        testBooleanObject(evaluated, test.expected);
    }
}