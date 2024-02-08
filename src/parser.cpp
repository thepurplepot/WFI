#include "parser.hh"

Parser::Parser(Lexer* l) {
    this->l = l;
    this->nextToken();
    this->nextToken();
}

Parser::~Parser() {
}

void Parser::nextToken() {
    this->curToken = this->peekToken;
    this->peekToken = this->l->nextToken();
}

Program* Parser::parseProgram() {
    Program* program = new Program();
    program->statements = new std::vector<Statement*>();
    while (!this->curTokenIs(token::EOF_)) {
        Statement* stmt = this->parseStatement();
        if (stmt != nullptr) {
            program->statements->push_back(stmt);
        }
        this->nextToken();
    }
    return program;
}

Statement* Parser::parseStatement() {
    if(this->curToken.getType() == token::LET) {
        return this->parseLetStatement();
    } else {
        return nullptr;
    
    }
}

LetStatement* Parser::parseLetStatement() {
    LetStatement* stmt = new LetStatement();
    stmt->token = this->curToken;
    if (!this->expectPeek(token::IDENT)) {
        return nullptr;
    }
    stmt->name = new Identifier(this->curToken, this->curToken.getLiteral());
    if (!this->expectPeek(token::ASSIGN)) {
        return nullptr;
    }
    while (!this->curTokenIs(token::SEMICOLON)) {
        this->nextToken();
    }
    return stmt;
}

bool Parser::curTokenIs(token_t t) {
    return this->curToken.getType() == t;
}

bool Parser::peekTokenIs(token_t t) {
    return this->peekToken.getType() == t;
}

bool Parser::expectPeek(token_t t) {
    if (this->peekTokenIs(t)) {
        this->nextToken();
        return true;
    } else {
        this->peekError(t);
        return false;
    }
}

void Parser::peekError(token_t t) {
    std::string msg = "expected next token to be " + t + ", got " + this->peekToken.getType() + " instead";
    this->errors.push_back(msg);
}

std::vector<std::string> Parser::getErrors() {
    return this->errors;
}