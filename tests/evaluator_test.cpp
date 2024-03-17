#include "lexer.hh"
#include "object.hh"
#include "parser.hh"
#include "evaluator.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

object::Object *testEval(const std::string &input) {
    Lexer l = Lexer(input);
    Parser p = Parser(&l);
    Program *program = p.parseProgram();
    object::Environment *env = new object::Environment();

    return evaluator::eval(program, env);
}

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

void testNullObject(object::Object *obj) {
    object::Null *result = dynamic_cast<object::Null*>(obj);
    ASSERT_TRUE(result != nullptr) << "object is not NULL. got=" << obj->type() << std::endl;
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
        object::Object *evaluated = testEval(test.input);
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
        object::Object *evaluated = testEval(test.input);
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
        object::Object *evaluated = testEval(test.input);
        testBooleanObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_if_else_expressions) {
    struct returnTypes {
        object::Integer *integer = nullptr;
        object::Null *null = nullptr;
    };
    struct IfElseTest {
        std::string input;
        returnTypes expected;
    };

    std::vector<IfElseTest> tests = {
        {"if (true) { 10 }", {.integer = new object::Integer(10)}},
         {"if (false) { 10 }", {.null = new object::Null()}},
         {"if (1) { 10 }", {.integer = new object::Integer(10)}},
         {"if (1 < 2) { 10 }", {.integer = new object::Integer(10)}},
         {"if (1 > 2) { 10 }", {.null = new object::Null()}},
         {"if (1 > 2) { 10 } else { 20 }", {.integer = new object::Integer(20)}},
         {"if (1 < 2) { 10 } else { 20 }", {.integer = new object::Integer(10)}},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        if (test.expected.integer != nullptr) {
            testIntegerObject(evaluated, test.expected.integer->value);
        } else {
            testNullObject(evaluated);
        }
    }
}

TEST(evaluator, test_return_statements) {
    struct ReturnTest {
        std::string input;
        int expected;
    };

    std::vector<ReturnTest> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {
            "if (10 > 1) {"
            "if (10 > 1) {"
            "return 10;"
            "}"
            "return 1;"
            "}",
            10
        },
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_error_handeling) {
    struct ErrorTest {
        std::string input;
        std::string expected;
    };

    std::vector<ErrorTest> tests = {
        {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
        {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
        {"-true", "unknown operator: -BOOLEAN"},
        {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
        {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
        {
            "if (10 > 1) {"
            "if (10 > 1) {"
            "return true + false;"
            "}"
            "return 1;"
            "}",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {"foobar", "identifier not found: foobar"},
        {"\"Hello\" - \"World\"", "unknown operator: STRING - STRING"},
        {"len(1)", "argument to `len` not supported, got INTEGER"},
        {"len(\"one\", \"two\")", "wrong number of arguments. got=2, want=1"},
        {"{\"name\": \"Monkey\"}[fn(x) { x }];", "unusable as hash key: FUNCTION"},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);

        object::Error *err = dynamic_cast<object::Error*>(evaluated);
        ASSERT_TRUE(err != nullptr) << "no error object returned. got=" << evaluated->type() << std::endl;
        ASSERT_EQ(err->message, test.expected) << "wrong error message. got=" << err->message << ", want=" << test.expected << std::endl;
    }
}

TEST(evaluator, test_let_statements) {
    struct LetTest {
        std::string input;
        int expected;
    };

    std::vector<LetTest> tests = {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_function_object) {
    std::string input = "fn(x) { x + 2; };";

    object::Object *evaluated = testEval(input);
    object::Function *fn = dynamic_cast<object::Function*>(evaluated);
    ASSERT_TRUE(fn != nullptr) << "object is not Function. got=" << evaluated->type() << std::endl;

    ASSERT_EQ(fn->parameters->size(), 1) << "function has wrong parameters. got=" << fn->parameters->size() << std::endl;
    ASSERT_EQ(fn->parameters->at(0)->value, "x") << "parameter is not 'x'. got=" << fn->parameters->at(0)->value << std::endl;
    ASSERT_EQ(fn->body->string(), "(x + 2)") << "body is not 'x + 2'. got=" << fn->body->string() << std::endl;
}

TEST(evaluator, test_function_application) {
    struct FunctionApplicationTest {
        std::string input;
        int expected;
    };

    std::vector<FunctionApplicationTest> tests = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_closures) {
    std::string input = R"(
        let newAdder = fn(x) {
            fn(y) { x + y };
        };
        let addTwo = newAdder(2);
        addTwo(2);
    )";

    object::Object *evaluated = testEval(input);
    testIntegerObject(evaluated, 4);
}

TEST(evaluator, test_string_literal) {
    std::string input = R"("Hello World!")";
    object::Object *evaluated = testEval(input);
    object::String *str = dynamic_cast<object::String*>(evaluated);
    ASSERT_TRUE(str != nullptr) << "object is not String. got=" << evaluated->type() << std::endl;
    ASSERT_EQ(str->value, "Hello World!") << "String has wrong value. got=" << str->value << std::endl;
}

TEST(evaluator, test_string_concatination) {
    std::string input = R"("Hello" + " " + "World!")";
    object::Object *evaluated = testEval(input);
    object::String *str = dynamic_cast<object::String*>(evaluated);
    ASSERT_TRUE(str != nullptr) << "object is not String. got=" << evaluated->type() << std::endl;
    ASSERT_EQ(str->value, "Hello World!") << "String has wrong value. got=" << str->value << std::endl;
}

TEST(evaluator, test_builtin_functions) {
    struct builtinReturnTypes {
        int integer;
        std::string str;
    };

    struct BuiltinTest {
        std::string input = "";
        builtinReturnTypes expected;
    };
    
    std::vector<BuiltinTest> tests = {
        {"len(\"\")", {.integer = 0}},
        {"len(\"four\")", {.integer = 4}},
        {"len(\"hello world\")", {.integer = 11}},
        {"len(1)", {.str = "argument to `len` not supported, got INTEGER"}},
        {"len(\"one\", \"two\")", {.str = "wrong number of arguments. got=2, want=1"}},
        {"first([1, 2, 3])", {.integer = 1}},
    };
    
    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        if (test.expected.str != "") {
            object::Error *err = dynamic_cast<object::Error*>(evaluated);
            ASSERT_TRUE(err != nullptr) << "no error object returned. got=" << evaluated->type() << std::endl;
            ASSERT_EQ(err->message, test.expected.str) << "wrong error message. got=" << err->message << ", want=" << test.expected.str << std::endl;
        } else {
            testIntegerObject(evaluated, test.expected.integer);
        }
    }
}

TEST(evaluator, test_array_literals) {
    std::string input = "[1, 2 * 2, 3 + 3]";
    object::Object *evaluated = testEval(input);
    object::Array *result = dynamic_cast<object::Array*>(evaluated);
    ASSERT_TRUE(result != nullptr) << "object is not Array. got=" << evaluated->type() << std::endl;
    ASSERT_EQ(result->elements.size(), 3) << "array has wrong number of elements. got=" << result->elements.size() << std::endl;
    testIntegerObject(result->elements[0], 1);
    testIntegerObject(result->elements[1], 4);
    testIntegerObject(result->elements[2], 6);
}

TEST(evaluator, test_array_index_expressions) {
    struct ArrayIndexTest {
        std::string input;
        int expected;
    };

    std::vector<ArrayIndexTest> tests = {
        {"[1, 2, 3][0]", 1},
        {"[1, 2, 3][1]", 2},
        {"[1, 2, 3][2]", 3},
        {"let i = 0; [1][i];", 1},
        {"[1, 2, 3][1 + 1];", 3},
        {"let myArray = [1, 2, 3]; myArray[2];", 3},
        {"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6},
        {"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", 2},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(evaluator, test_hash_literals) {
    std::string input = R"({"one": 10, "two": 10 * 2, "three": 10 + 3})";
    object::Object *evaluated = testEval(input);
    object::Hash *result = dynamic_cast<object::Hash*>(evaluated);
    ASSERT_TRUE(result != nullptr) << "object is not Hash. got=" << evaluated->type() << std::endl;

    std::map<std::string, int> expected = {
        {"one", 10},
        {"two", 20},
        {"three", 13},
    };

    ASSERT_EQ(result->pairs.size(), expected.size()) << "hash has wrong number of pairs. got=" << result->pairs.size() << std::endl;

    for (auto expectedPair : expected) {
        auto expectedKey = new object::String(expectedPair.first);
        auto pair = result->pairs[expectedKey->hash_key()];
        
        ASSERT_TRUE(pair != nullptr) << "no pair for given key in pairs" << std::endl;
        ASSERT_TRUE(pair->key->inspect() == expectedPair.first) << "pair has no key" << std::endl;

        testIntegerObject(pair->value, expectedPair.second);
    }
}

TEST(evaluator, test_hash_index_expressions) {
    struct HashIndexTest {
        std::string input;
        int expected;
    };

    std::vector<HashIndexTest> tests = {
        {R"({"one": 1, "two": 2, "three": 3}["one"])", 1},
        {R"({"one": 1, "two": 2, "three": 3}["two"])", 2},
        {R"({"one": 1, "two": 2, "three": 3}["three"])", 3},
        //{R"({"one": 1, "two": 2, "three": 3}[\"four\"])", 0},
        {R"({1: 1, 2: 2, 3: 3}[1])", 1},
        {R"({1: 1, 2: 2, 3: 3}[2])", 2},
        {R"({1: 1, 2: 2, 3: 3}[3])", 3},
        //{R"({1: 1, 2: 2, 3: 3}[4])", 0},
    };

    for(auto test : tests) {
        object::Object *evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}