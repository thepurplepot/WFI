#include "evaluator.hh"

object::Object* evaluator::eval(Node *node, object::Environment *env) {
    if(node->type() == "Program") {
        return evalProgram(dynamic_cast<Program*>(node), env);
    } else if (node->type() == "ExpressionStatement") {
        return eval(dynamic_cast<ExpressionStatement*>(node)->expression, env);
    } else if (node->type() == "BlockStatement") {
        return evalBlockStatement(dynamic_cast<BlockStatement*>(node), env);
    } else if (node->type() == "ReturnStatement") {
        object::Object *val = eval(dynamic_cast<ReturnStatement*>(node)->returnValue, env);
        if (evaluator::isError(val)) {
            return val;
        }
        return new object::ReturnValue(val);
    } else if (node->type() == "LetStatement") {
        object::Object *val = eval(dynamic_cast<LetStatement*>(node)->value, env);
        if (evaluator::isError(val)) {
            return val;
        }
        return env->set(dynamic_cast<LetStatement*>(node)->name->value, val);
    } else if (node->type() == "IfExpression") {
        return evalIfExpression(dynamic_cast<IfExpression*>(node), env);
    } else if (node->type() == "Identifier") {
        return evalIdentifier(dynamic_cast<Identifier*>(node), env);
    } else if (node->type() == "IntegerLiteral") {
        return new object::Integer(dynamic_cast<IntegerLiteral*>(node)->value);
    } else if (node->type() == "Boolean") {
        return evaluator::nativeBoolToBooleanObject(dynamic_cast<Boolean*>(node)->value);
    } else if (node->type() == "StringLiteral") {
        return new object::String(dynamic_cast<StringLiteral*>(node)->value);
    } else if (node->type() == "FunctionLiteral") {
        return new object::Function(&dynamic_cast<FunctionLiteral*>(node)->parameters, dynamic_cast<FunctionLiteral*>(node)->body, env);
    } else if (node->type() == "ArrayLiteral") {
        std::vector<object::Object*> elements = evalExpressions(dynamic_cast<ArrayLiteral*>(node)->elements, env);
        if (elements.size() == 1 && evaluator::isError(elements[0])) {
            return elements[0];
        }
        return new object::Array(elements);
    } else if (node->type() == "HashLiteral") {
        return evalHashLiteral(dynamic_cast<HashLiteral*>(node), env);
    } else if (node->type() == "IndexExpression") {
        object::Object *left = eval(dynamic_cast<IndexExpression*>(node)->left, env);
        if (evaluator::isError(left)) {
            return left;
        }
        object::Object *index = eval(dynamic_cast<IndexExpression*>(node)->index, env);
        if (evaluator::isError(index)) {
            return index;
        }
        return evalIndexExpression(left, index);
    } else if (node->type() == "PrefixExpression") {
        object::Object *right = eval(dynamic_cast<PrefixExpression*>(node)->right, env);
        if (evaluator::isError(right)) {
            return right;
        }
        return evalPrefixExpression(dynamic_cast<PrefixExpression*>(node)->op, right);
    } else if (node->type() == "InfixExpression") {
        object::Object *left = eval(dynamic_cast<InfixExpression*>(node)->left, env);
        if (evaluator::isError(left)) {
            return left;
        }
        object::Object *right = eval(dynamic_cast<InfixExpression*>(node)->right, env);
        if (evaluator::isError(right)) {
            return right;
        }
        return evalInfixExpression(dynamic_cast<InfixExpression*>(node)->op, left, right);
    } else if (node->type() == "CallExpression") {
        auto function = eval(dynamic_cast<CallExpression*>(node)->function, env);
        if(evaluator::isError(function)) {
            return function;
        }
        std::vector<object::Object*> args = evalExpressions(dynamic_cast<CallExpression*>(node)->arguments, env);
        if (args.size() == 1 && evaluator::isError(args[0])) {
            return args[0];
        }
        return applyFunction(function, args);
    } else {
        return new object::Error("unknown node type: " + node->type());
    }
}

object::Object* evaluator::evalProgram(Program *program, object::Environment *env) {
    object::Object *result;
    for (auto &statement : *program->statements) {
        result = evaluator::eval(statement, env);
        if (result->type() == object::RETURN_VALUE_OBJ) {
            object::Object *value = dynamic_cast<object::ReturnValue*>(result)->value;
            return value;
        } else if (result->type() == object::ERROR_OBJ) {
            return result;
        }
    }
    return result;  
}

std::vector<object::Object*> evaluator::evalExpressions(std::vector<Expression*> expressions, object::Environment *env) {
    std::vector<object::Object*> result;
    for (auto &expression : expressions) {
        object::Object *evaluated = evaluator::eval(expression, env);
        if (evaluator::isError(evaluated)) {
            return {evaluated};
        }
        result.push_back(evaluated);
    }
    return result;
}

object::Object* evaluator::evalBlockStatement(BlockStatement *block, object::Environment *env) {
    object::Object *result;
    for (auto &statement : *block->statements) {
        result = evaluator::eval(statement, env);
        if (result != nullptr && (result->type() == object::RETURN_VALUE_OBJ || result->type() == object::ERROR_OBJ)) {
            return result;
        }
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
        return new object::Error("unknown operator: " + op + right->type());
    }
}

object::Object* evaluator::evalBangOperatorExpression(object::Object *right) {
    if (right->type() == object::BOOLEAN_OBJ) {
        return dynamic_cast<object::Boolean*>(right)->value ? evaluator::FALSE : evaluator::TRUE;
    } else if (right->type() == object::NULL_OBJ) {
        return evaluator::TRUE;
    } else {
        return evaluator::FALSE;
    }
}

object::Object* evaluator::evalMinusPrefixOperatorExpression(object::Object *right) {
    if (right->type() != object::INTEGER_OBJ) {
        return new object::Error("unknown operator: -" + right->type());
    }
    return new object::Integer(-dynamic_cast<object::Integer*>(right)->value);
}

object::Object* evaluator::evalInfixExpression(std::string op, object::Object *left, object::Object *right) {
    if (left->type() == object::INTEGER_OBJ && right->type() == object::INTEGER_OBJ) {
        return evalIntegerInfixExpression(op, dynamic_cast<object::Integer*>(left), dynamic_cast<object::Integer*>(right));
    } else if (left->type() == object::BOOLEAN_OBJ && right->type() == object::BOOLEAN_OBJ) {
        return evalBooleanInfixExpression(op, dynamic_cast<object::Boolean*>(left), dynamic_cast<object::Boolean*>(right));
    } else if (left->type() == object::STRING_OBJ && right->type() == object::STRING_OBJ) {
        return evalStringInfixExpression(op, dynamic_cast<object::String*>(left), dynamic_cast<object::String*>(right));
    } else if (left->type() != right->type()) {
        return new object::Error("type mismatch: " + left->type() + " " + op + " " + right->type());
    } else {
        return new object::Error("unknown operator: " + left->type() + " " + op + " " + right->type());
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
        return new object::Error("unknown operator: " + left->type() + " " + op + " " + right->type());
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
        return new object::Error("unknown operator: " + left->type() + " " + op + " " + right->type());
    }
}

object::Object* evaluator::evalStringInfixExpression(std::string op, object::String *left, object::String *right) {
    if (op != "+") {
        return new object::Error("unknown operator: " + left->type() + " " + op + " " + right->type());
    }
    return new object::String(left->value + right->value);
}

object::Object* evaluator::evalIfExpression(IfExpression *ie, object::Environment *env) {
    object::Object *condition = evaluator::eval(ie->condition, env);
    if (condition->type() == object::ERROR_OBJ) {
        return condition;
    }
    if (evaluator::isTruthy(condition)) {
        return evaluator::eval(ie->consequence, env);
    } else if (ie->alternative != nullptr) {
        return evaluator::eval(ie->alternative, env);
    } else {
        return evaluator::NULLobj;
    }
}

object::Object* evaluator::evalIndexExpression(object::Object *left, object::Object *index) {
    if (left->type() == object::ARRAY_OBJ && index->type() == object::INTEGER_OBJ) {
        return evalArrayIndexExpression(dynamic_cast<object::Array*>(left), dynamic_cast<object::Integer*>(index));
    } else if (left->type() == object::HASH_OBJ) {
        return evalHashIndexExpression(dynamic_cast<object::Hash*>(left), index);
    } else {
        return new object::Error("index operator not supported: " + left->type());
    }
}

object::Object* evaluator::evalArrayIndexExpression(object::Array *array, object::Integer *index) {
    int idx = index->value;
    int max = array->elements.size() - 1;
    if (idx < 0 || idx > max) {
        return evaluator::NULLobj;
    }
    return array->elements[idx];
}

object::Object* evaluator::evalHashIndexExpression(object::Hash *hash, object::Object *index) {
    if (!index->hashable()) {
        return new object::Error("unusable as hash key: " + index->type());
    }
    auto pair = hash->pairs.find(index->hash_key());
    if (pair == hash->pairs.end()) {
        return evaluator::NULLobj;
    }
    return pair->second->value;
}

object::Object* evaluator::evalIdentifier(Identifier *node, object::Environment *env) {
    object::Object *val = env->get(node->value);
    if (val != nullptr) {
        return val;
    }
    if (evaluator::builtins.find(node->value) != evaluator::builtins.end()) {
        return evaluator::builtins[node->value];
    }
    return new object::Error("identifier not found: " + node->value);
}

object::Object* evaluator::applyFunction(object::Object *fn, std::vector<object::Object*> args) {
    if (fn->type() == object::FUNCTION_OBJ) {
        object::Function *function = dynamic_cast<object::Function*>(fn);
        object::Environment *extendedEnv = extendFunctionEnv(function, args);
        object::Object *evaluated = evaluator::eval(function->body, extendedEnv);
        return unwrapReturnValue(evaluated);
    } else if (fn->type() == object::BUILTIN_OBJ) {
        return dynamic_cast<object::Builtin*>(fn)->fn(args);
    } else {
        return new object::Error("not a function: " + fn->type());
    }
}

object::Environment* evaluator::extendFunctionEnv(object::Function *fn, std::vector<object::Object*> args) {
    object::Environment *env = fn->env->newEnclosedEnvironment();
    for (int i = 0; i < fn->parameters->size(); i++) {
        env->set(fn->parameters->at(i)->value, args[i]);
    }
    return env;
}

object::Object* evaluator::unwrapReturnValue(object::Object *obj) {
    if (obj->type() == object::RETURN_VALUE_OBJ) {
        return dynamic_cast<object::ReturnValue*>(obj)->value;
    }
    return obj;
}

object::Object* evaluator::evalHashLiteral(HashLiteral *node, object::Environment *env) {
    std::map<object::Object*, object::Object*> pairs;
    for (auto pair : node->pairs) {
        object::Object *key = evaluator::eval(pair.first, env);
        if (evaluator::isError(key)) {
            return key;
        }
        if (!key->hashable()) {
            return new object::Error("unusable as hash key: " + key->type());
        }
        object::Object *value = evaluator::eval(pair.second, env);
        if (evaluator::isError(value)) {
            return value;
        }
        pairs[key] = value;
    }
    return new object::Hash(pairs);
}

bool evaluator::isTruthy(object::Object *obj) {
    if (obj == evaluator::NULLobj) {
        return false;
    } else if (obj == evaluator::TRUE) {
        return true;
    } else if (obj == evaluator::FALSE) {
        return false;
    } else {
        return true;
    }
}

bool evaluator::isError(object::Object *obj) {
    if (obj != nullptr) {
        return obj->type() == object::ERROR_OBJ;
    }
    return false;
}