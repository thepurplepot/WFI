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

ObjectType object::Error::type() {
    return object::ERROR;
}

std::string object::Error::inspect() {
    return this->message;
}
