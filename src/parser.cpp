#include "parser.hh"

Lexer* Parser::l = NULL;
Token Parser::curToken = Token();
Token Parser::peekToken = Token();
std::vector<std::string> Parser::errors = std::vector<std::string>();
std::map<token_t, prefixParseFn_t*> Parser::prefixParseFns = std::map<token_t, prefixParseFn_t*>();
std::map<token_t, infixParseFn_t*> Parser::infixParseFns = std::map<token_t, infixParseFn_t*>();

const std::map<token_t, precedence_t> Parser::precedences = {
    {token::EQ, EQUALS},
    {token::NOT_EQ, EQUALS},
    {token::LT, LESSGREATER},
    {token::GT, LESSGREATER},
    {token::PLUS, SUM},
    {token::MINUS, SUM},
    {token::SLASH, PRODUCT},
    {token::ASTERISK, PRODUCT},
};


Parser::Parser(Lexer* l) {
    this->l = l;
    this->nextToken();
    this->nextToken();

    this->registerPrefix(token::IDENT, Parser::parseIdentifier);
    this->registerPrefix(token::INT, Parser::parseIntegerLiteral);
    this->registerPrefix(token::BANG, Parser::parsePrefixExpression);
    this->registerPrefix(token::MINUS, Parser::parsePrefixExpression);

    this->registerInfix(token::PLUS, Parser::parseInfixExpression);
    this->registerInfix(token::MINUS, Parser::parseInfixExpression);
    this->registerInfix(token::SLASH, Parser::parseInfixExpression);
    this->registerInfix(token::ASTERISK, Parser::parseInfixExpression);
    this->registerInfix(token::EQ, Parser::parseInfixExpression);
    this->registerInfix(token::NOT_EQ, Parser::parseInfixExpression);
    this->registerInfix(token::LT, Parser::parseInfixExpression);
    this->registerInfix(token::GT, Parser::parseInfixExpression);
}

Parser::~Parser() {
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->nextToken();
}

Program* Parser::parseProgram() {
    Program* program = new Program();
    program->statements = new std::vector<Statement*>();
    while (!curTokenIs(token::EOF_)) {
        Statement* stmt = parseStatement();
        if (stmt != nullptr) {
            program->statements->push_back(stmt);
        }
        nextToken();
    }
    return program;
}

Statement* Parser::parseStatement() {
    if(curToken.getType() == token::LET) {
        return parseLetStatement();
    } else if (curToken.getType() == token::RETURN) {
        return parseReturnStatement();
    } else {
        return parseExpressionStatement();
    }
}

LetStatement* Parser::parseLetStatement() {
    LetStatement* stmt = new LetStatement();
    stmt->token = curToken;
    if (!expectPeek(token::IDENT)) {
        return nullptr;
    }
    stmt->name = new Identifier(curToken, curToken.getLiteral());
    if (!expectPeek(token::ASSIGN)) {
        return nullptr;
    }
    while (!curTokenIs(token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

ReturnStatement* Parser::parseReturnStatement() {
    ReturnStatement* stmt = new ReturnStatement();
    stmt->token = curToken;
    nextToken();
    while (!curTokenIs(token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

ExpressionStatement* Parser::parseExpressionStatement() {
    ExpressionStatement* stmt = new ExpressionStatement(curToken, parseExpression(LOWEST));
    if (peekTokenIs(token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

Expression* Parser::parseExpression(precedence_t precedence) {
    prefixParseFn_t* prefix = prefixParseFns[curToken.getType()];
    if (prefix == nullptr) {
        noPrefixParseFnError(curToken.getType());
        return nullptr;
    }

    Expression* leftExp = prefix();

    while (!peekTokenIs(token::SEMICOLON) && precedence < peekPrecedence()) {
        infixParseFn_t* infix = infixParseFns[peekToken.getType()];
        if (infix == nullptr) {
            return leftExp;
        }
        nextToken();
        leftExp = infix(leftExp);
    }

    return leftExp;
}

Expression* Parser::parseIdentifier() {
    return new Identifier(curToken, curToken.getLiteral());
}

Expression* Parser::parseIntegerLiteral() {
    try {
        return new IntegerLiteral(curToken, std::stoi(curToken.getLiteral()));
    } catch (std::invalid_argument e) {
        errors.push_back("could not parse " + curToken.getLiteral() + " as integer");
        return nullptr;
    } catch (std::out_of_range e) {
        errors.push_back("could not parse " + curToken.getLiteral() + " as integer (out of range)");
        return nullptr;
    }
}

Expression* Parser::parsePrefixExpression() {
    PrefixExpression* exp = new PrefixExpression(curToken, curToken.getLiteral());
    nextToken();
    exp->right = parseExpression(PREFIX);
    return exp;
}

Expression* Parser::parseInfixExpression(Expression* left) {
    InfixExpression* exp = new InfixExpression(curToken, curToken.getLiteral(), left);
    precedence_t precedence = curPrecedence();
    nextToken();
    exp->right = parseExpression(precedence);
    return exp;
}

bool Parser::curTokenIs(token_t t) {
    return curToken.getType() == t;
}

bool Parser::peekTokenIs(token_t t) {
    return peekToken.getType() == t;
}

bool Parser::expectPeek(token_t t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

void Parser::peekError(token_t t) {
    std::string msg = "expected next token to be " + t + ", got " + peekToken.getType() + " instead";
    errors.push_back(msg);
}

precedence_t Parser::peekPrecedence() {
    if (precedences.find(peekToken.getType()) != precedences.end()) {
        return precedences.at(peekToken.getType());
    }
    return LOWEST;
}

precedence_t Parser::curPrecedence() {
    if (precedences.find(curToken.getType()) != precedences.end()) {
        return precedences.at(curToken.getType());
    }
    return LOWEST;
}

std::vector<std::string> Parser::getErrors() {
    return errors;
}

void Parser::registerPrefix(token_t t, prefixParseFn_t* fn) {
    prefixParseFns[t] = fn;
}

void Parser::registerInfix(token_t t, infixParseFn_t* fn) {
    infixParseFns[t] = fn;
}

void Parser::noPrefixParseFnError(token_t t) {
    std::string msg = "no prefix parse function for " + t + " found";
    errors.push_back(msg);
}