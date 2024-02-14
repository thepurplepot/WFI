#include "ast.hh"

std::string Program::token_literal() {
    if (this->statements->size() > 0) {
        return this->statements->at(0)->token_literal();
    } else {
        return "";
    }
}

std::string Program::string() {
    std::string out;
    for (auto stmt : *this->statements) {
        out += stmt->string();  
    }
    return out;
}

std::string LetStatement::token_literal() {
    return this->token.getLiteral();
}

std::string LetStatement::statement_node() {
    return "LetStatement";
}

std::string LetStatement::string() {
    std::string out;
    out += this->token.getLiteral() + " ";
    out += this->name->string();
    out += " = ";
    if (this->value != nullptr) {
        out += this->value->string();
    }
    out += ";";
    return out;
}

std::string Identifier::token_literal() {
    return this->token.getLiteral();
}

std::string Identifier::expression_node() {
    return "Identifier";
}

std::string Identifier::string() {
    return this->value;
}

std::string ReturnStatement::token_literal() {
    return this->token.getLiteral();
}

std::string ReturnStatement::statement_node() {
    return "ReturnStatement";
}

std::string ReturnStatement::string() {
    std::string out;
    out += this->token.getLiteral() + " ";
    if (this->returnValue != nullptr) {
        out += this->returnValue->string();
    }
    out += ";";
    return out;
}

std::string ExpressionStatement::token_literal() {
    return this->token.getLiteral();
}

std::string ExpressionStatement::statement_node() {
    return "ExpressionStatement";
}

std::string ExpressionStatement::string() {
    if (this->expression != nullptr) {
        return this->expression->string();
    }
    return "";
}

std::string IntegerLiteral::token_literal() {
    return this->token.getLiteral();
}

std::string IntegerLiteral::expression_node() {
    return "IntegerLiteral";
}

std::string IntegerLiteral::string() {
    return this->token.getLiteral();
}

std::string PrefixExpression::token_literal() {
    return this->token.getLiteral();
}

std::string PrefixExpression::expression_node() {
    return "PrefixExpression";
}

std::string PrefixExpression::string() {
    std::string out;
    out += "(";
    out += this->op;
    out += this->right->string();
    out += ")";
    return out;
}

std::string InfixExpression::token_literal() {
    return this->token.getLiteral();
}

std::string InfixExpression::expression_node() {
    return "InfixExpression";
}

std::string InfixExpression::string() {
    std::string out;
    out += "(";
    out += this->left->string();
    out += " " + this->op + " ";
    out += this->right->string();
    out += ")";
    return out;
}

std::string Boolean::token_literal() {
    return this->token.getLiteral();
}

std::string Boolean::expression_node() {
    return "Boolean";
}

std::string Boolean::string() {
    return this->token.getLiteral();
}

std::string IfExpression::token_literal() {
    return this->token.getLiteral();
}

std::string IfExpression::expression_node() {
    return "IfExpression";
}

std::string IfExpression::string() {
    std::string out;
    out += "if";
    out += this->condition->string();
    out += " ";
    out += this->consequence->string();
    if (this->alternative != nullptr) {
        out += "else ";
        out += this->alternative->string();
    }
    return out;
}

std::string BlockStatement::token_literal() {
    return this->token.getLiteral();
}

std::string BlockStatement::statement_node() {
    return "BlockStatement";
}

std::string BlockStatement::string() {
    std::string out;
    for (auto stmt : *this->statements) {
        out += stmt->string();
    }
    return out;
}

std::string FunctionLiteral::token_literal() {
    return this->token.getLiteral();
}

std::string FunctionLiteral::expression_node() {
    return "FunctionLiteral";
}

std::string FunctionLiteral::string() {
    std::string out;
    out += this->token.getLiteral();
    out += "(";
    for (int i = 0; i < this->parameters.size(); i++) {
        out += this->parameters.at(i)->string();
        if (i != this->parameters.size() - 1) {
            out += ", ";
        }
    }
    out += ")";
    out += this->body->string();
    return out;
}

std::string CallExpression::token_literal() {
    return this->token.getLiteral();
}

std::string CallExpression::expression_node() {
    return "CallExpression";
}

std::string CallExpression::string() {
    std::string out;
    out += this->function->string();
    out += "(";
    for(int i = 0; i < this->arguments.size(); i++) {
        out += this->arguments.at(i)->string();
        if (i != this->arguments.size() - 1) {
            out += ", ";
        }
    }
    out += ")";
    return out;
}