#include "evaluator.hh"

object::Object* evaluator::eval(Node *node) {
    if(node->type() == "Program") {
        return evalStatements(dynamic_cast<Program*>(node)->statements);
    } else if (node->type() == "ExpressionStatement") {
        return eval(dynamic_cast<ExpressionStatement*>(node)->expression);
    } else if (node->type() == "IntegerLiteral") {
        return new object::Integer(dynamic_cast<IntegerLiteral*>(node)->value);
    } else if (node->type() == "Boolean") {
        return evaluator::nativeBoolToBooleanObject(dynamic_cast<Boolean*>(node)->value);
    } else if (node->type() == "PrefixExpression") {
        object::Object *right = eval(dynamic_cast<PrefixExpression*>(node)->right);
        if (right->type() == "ERROR") {
            return right;
        }
        return evalPrefixExpression(dynamic_cast<PrefixExpression*>(node)->op, right);
    } else if (node->type() == "InfixExpression") {
        object::Object *left = eval(dynamic_cast<InfixExpression*>(node)->left);
        if (left->type() == "ERROR") {
            return left;
        }
        object::Object *right = eval(dynamic_cast<InfixExpression*>(node)->right);
        if (right->type() == "ERROR") {
            return right;
        }
        return evalInfixExpression(dynamic_cast<InfixExpression*>(node)->op, left, right);
    } else {
        return nullptr;
    }
}

object::Object* evaluator::evalStatements(std::vector<Statement*> *statements) {
    object::Object *result;
    for (auto &statement : *statements) {
        result = evaluator::eval(statement);
    }
    return result;
}

object::Object* evaluator::nativeBoolToBooleanObject(bool input) {
    return input ? evaluator::TRUE : evaluator::FALSE;
}

object::Object* evaluator::evalPrefixExpression(std::string op, object::Object *right) {
    if (op == "!") {
        return evalBangOperatorExpression(right);
    } else if (op == "-") {
        return evalMinusPrefixOperatorExpression(right);
    } else {
        return evaluator::NULLobj;
    }
}

object::Object* evaluator::evalBangOperatorExpression(object::Object *right) {
    if (right->type() == "BOOLEAN") {
        return dynamic_cast<object::Boolean*>(right)->value ? evaluator::FALSE : evaluator::TRUE;
    } else if (right->type() == "NULL") {
        return evaluator::TRUE;
    } else {
        return evaluator::FALSE;
    }
}

object::Object* evaluator::evalMinusPrefixOperatorExpression(object::Object *right) {
    if (right->type() != "INTEGER") {
        return new object::Error("unknown operator: -" + right->type());
    }
    return new object::Integer(-dynamic_cast<object::Integer*>(right)->value);
}

object::Object* evaluator::evalInfixExpression(std::string op, object::Object *left, object::Object *right) {
    if (left->type() == "INTEGER" && right->type() == "INTEGER") {
        return evalIntegerInfixExpression(op, dynamic_cast<object::Integer*>(left), dynamic_cast<object::Integer*>(right));
    } else if (left->type() == "BOOLEAN" && right->type() == "BOOLEAN") {
        return evalBooleanInfixExpression(op, dynamic_cast<object::Boolean*>(left), dynamic_cast<object::Boolean*>(right));
    } else if (left->type() != right->type()) {
        return new object::Error("type mismatch: " + left->type() + " " + op + " " + right->type());
    } else {
        return evaluator::NULLobj;
    }
}

object::Object* evaluator::evalIntegerInfixExpression(std::string op, object::Integer *left, object::Integer *right) {
    int leftVal = left->value;
    int rightVal = right->value;
    if (op == "+") {
        return new object::Integer(leftVal + rightVal);
    } else if (op == "-") {
        return new object::Integer(leftVal - rightVal);
    } else if (op == "*") {
        return new object::Integer(leftVal * rightVal);
    } else if (op == "/") {
        return new object::Integer(leftVal / rightVal);
    } else if (op == "<") {
        return evaluator::nativeBoolToBooleanObject(leftVal < rightVal);
    } else if (op == ">") {
        return evaluator::nativeBoolToBooleanObject(leftVal > rightVal);
    } else if (op == "==") {
        return evaluator::nativeBoolToBooleanObject(leftVal == rightVal);
    } else if (op == "!=") {
        return evaluator::nativeBoolToBooleanObject(leftVal != rightVal);
    } else {
        return evaluator::NULLobj;
    }
}

object::Object* evaluator::evalBooleanInfixExpression(std::string op, object::Boolean *left, object::Boolean *right) {
    bool leftVal = left->value;
    bool rightVal = right->value;
    if (op == "==") {
        return evaluator::nativeBoolToBooleanObject(leftVal == rightVal);
    } else if (op == "!=") {
        return evaluator::nativeBoolToBooleanObject(leftVal != rightVal);
    } else {
        return evaluator::NULLobj;
    }
}