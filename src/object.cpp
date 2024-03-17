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

ObjectType object::String::type() {
    return object::STRING_OBJ;
}

std::string object::String::inspect() {
    return this->value;
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

ObjectType object::Builtin::type() {
    return object::BUILTIN_OBJ;
}

std::string object::Builtin::inspect() {
    return "builtin function";
}

ObjectType object::Array::type() {
    return object::ARRAY_OBJ;
}

std::string object::Array::inspect() {
    std::string out = "[";
    for (auto elem : this->elements) {
        out += elem->inspect() + ", ";
    }
    out += "]";
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


uint64_t object::Object::hash_key() {
    std::hash<std::string> hash_fn;
    if(this->type() == object::STRING_OBJ) {
        auto str = dynamic_cast<object::String*>(this);
        return hash_fn(str->value);
    } else if (this->type() == object::INTEGER_OBJ) {
        auto i = dynamic_cast<object::Integer*>(this);
        return i->value;
    } else if (this->type() == object::BOOLEAN_OBJ) {
        auto b = dynamic_cast<object::Boolean*>(this);
        return b->value ? 1 : 0;
    }
    return 0;
}

bool object::Object::hashable() {
    return this->type() == object::STRING_OBJ || this->type() == object::INTEGER_OBJ || this->type() == object::BOOLEAN_OBJ;
}

object::Hash::Hash(std::map<object::Object*, object::Object*> pairs) {
    for(auto pair : pairs) {
        this->pairs[pair.first->hash_key()] = new HashPair(pair.first, pair.second);
    }
}

ObjectType object::Hash::type() {
    return object::HASH_OBJ;
}

std::string object::Hash::inspect() {
    std::string out = "{";
    int i = 0;
    for (auto pair : this->pairs) {
        object::HashPair hashPair = *pair.second;
        out += hashPair.key->inspect() + ": " + hashPair.value->inspect();
        if (i++ != this->pairs.size() - 1) {
            out += ", ";
        }
    }
    out += "}";
    return out;
}
