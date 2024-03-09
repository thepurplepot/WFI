#include "object.hh"

ObjectType object::Integer::type() {
    return object::INTEGER_OBJ;
}

std::string object::Integer::inspect() {
    return std::to_string(this->value);
}

ObjectType object::Boolean::type() {
    return object::BOOLEAN_OBJ;
}

std::string object::Boolean::inspect() {
    return this->value ? "true" : "false";
}

ObjectType object::Null::type() {
    return object::NULL_OBJ;
}

std::string object::Null::inspect() {
    return "null";
}

ObjectType object::ReturnValue::type() {
    return object::RETURN_VALUE_OBJ;
}

std::string object::ReturnValue::inspect() {
    return this->value->inspect();
}

ObjectType object::Function::type() {
    return object::FUNCTION_OBJ;
}

std::string object::Function::inspect() {
    std::string out = "fn(";
    for (auto param : *this->parameters) {
        out += param->value + ", ";
    }
    out += ") {\n";
    out += this->body->string();
    out += "\n}";
    return out;
}

ObjectType object::Error::type() {
    return object::ERROR_OBJ;
}

std::string object::Error::inspect() {
    return "ERROR: " + this->message;
}

object::Environment::Environment() {
    store = new std::map<std::string, object::Object*>();
    outer = nullptr;
}

object::Object* object::Environment::get(std::string name) {
    auto found = store->find(name);
    if (found == store->end()) {
        if (outer == nullptr) {
            return nullptr;
        }
        return outer->get(name);
    }
    return found->second;
}

object::Object* object::Environment::set(std::string name, object::Object *value) {
    store->insert({name, value});
    return value;
}

object::Environment* object::Environment::newEnclosedEnvironment() {
    object::Environment *env = new object::Environment();
    env->outer = this;
    return env;
}