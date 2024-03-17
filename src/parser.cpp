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
    {token::LPAREN, CALL},
    {token::LBRACKET, INDEX}
};


Parser::Parser(Lexer* l) {
    this->l = l;
    this->nextToken();
    this->nextToken();

    this->registerPrefix(token::IDENT, Parser::parseIdentifier);
    this->registerPrefix(token::INT, Parser::parseIntegerLiteral);
    this->registerPrefix(token::BANG, Parser::parsePrefixExpression);
    this->registerPrefix(token::MINUS, Parser::parsePrefixExpression);
    this->registerPrefix(token::TRUE, Parser::parseBoolean);
    this->registerPrefix(token::FALSE, Parser::parseBoolean); 
    this->registerPrefix(token::LPAREN, Parser::parseGroupedExpression);
    this->registerPrefix(token::IF, Parser::parseIfExpression);
    this->registerPrefix(token::FUNCTION, Parser::parseFunctionLiteral);
    this->registerPrefix(token::STRING, Parser::parseStringLiteral);
    this->registerPrefix(token::LBRACKET, Parser::parseArrayLiteral);
    this->registerPrefix(token::LBRACE, Parser::parseHashLiteral);

    this->registerInfix(token::PLUS, Parser::parseInfixExpression);
    this->registerInfix(token::MINUS, Parser::parseInfixExpression);
    this->registerInfix(token::SLASH, Parser::parseInfixExpression);
    this->registerInfix(token::ASTERISK, Parser::parseInfixExpression);
    this->registerInfix(token::EQ, Parser::parseInfixExpression);
    this->registerInfix(token::NOT_EQ, Parser::parseInfixExpression);
    this->registerInfix(token::LT, Parser::parseInfixExpression);
    this->registerInfix(token::GT, Parser::parseInfixExpression);
    this->registerInfix(token::LPAREN, Parser::parseCallExpression);
    this->registerInfix(token::LBRACKET, Parser::parseIndexExpression);
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
    nextToken();
    stmt->value = parseExpression(LOWEST);
    while (!curTokenIs(token::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

ReturnStatement* Parser::parseReturnStatement() {
    ReturnStatement* stmt = new ReturnStatement();
    stmt->token = curToken;
    nextToken();
    stmt->returnValue = parseExpression(LOWEST);
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

Expression* Parser::parseStringLiteral() {
    return new StringLiteral(curToken, curToken.getLiteral());
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

Expression* Parser::parseGroupedExpression() {
    nextToken();
    Expression* exp = parseExpression(LOWEST);
    if (!expectPeek(token::RPAREN)) {
        return nullptr;
    }
    return exp;
}

Expression* Parser::parseBoolean() {
    return new Boolean(curToken, curTokenIs(token::TRUE));
}

Expression* Parser::parseIfExpression() {
    IfExpression* exp = new IfExpression(curToken);
    if (!expectPeek(token::LPAREN)) {
        return nullptr;
    }
    nextToken();
    exp->condition = parseExpression(LOWEST);
    if (!expectPeek(token::RPAREN)) {
        return nullptr;
    }
    if (!expectPeek(token::LBRACE)) {
        return nullptr;
    }
    exp->consequence = parseBlockStatement();

    if (peekTokenIs(token::ELSE)) {
        nextToken();
        if (!expectPeek(token::LBRACE)) {
            return nullptr;
        }
        exp->alternative = parseBlockStatement();
    }

    return exp;
}

BlockStatement* Parser::parseBlockStatement() {
    BlockStatement* block = new BlockStatement(curToken);
    nextToken();
    while (!curTokenIs(token::RBRACE) && !curTokenIs(token::EOF_)) {
        Statement* stmt = parseStatement();
        if (stmt != nullptr) {
            block->statements->push_back(stmt);
        }
        nextToken();
    }
    return block;
}

Expression* Parser::parseFunctionLiteral() {
    FunctionLiteral* lit = new FunctionLiteral(curToken);
    if (!expectPeek(token::LPAREN)) {
        return nullptr;
    }
    lit->parameters = parseFunctionParameters();
    if (!expectPeek(token::LBRACE)) {
        return nullptr;
    }
    lit->body = parseBlockStatement();
    return lit;
}

std::vector<Identifier*> Parser::parseFunctionParameters() {
    std::vector<Identifier*> params;
    if (peekTokenIs(token::RPAREN)) {
        nextToken();
        return params;
    }
    nextToken();
    Identifier* ident = new Identifier(curToken, curToken.getLiteral());
    params.push_back(ident);
    while (peekTokenIs(token::COMMA)) {
        nextToken();
        nextToken();
        Identifier* ident = new Identifier(curToken, curToken.getLiteral());
        params.push_back(ident);
    }
    if (!expectPeek(token::RPAREN)) {
        //NULL
        return std::vector<Identifier*>();
    }
    return params;
}

Expression* Parser::parseCallExpression(Expression* function) {
    CallExpression* exp = new CallExpression(curToken, function);
    exp->arguments = parseExpressionList(token::RPAREN);
    return exp;
}

std::vector<Expression*> Parser::parseCallArguments() {
    std::vector<Expression*> args;
    if (peekTokenIs(token::RPAREN)) {
        nextToken();
        return args;
    }
    nextToken();
    args.push_back(parseExpression(LOWEST));
    while (peekTokenIs(token::COMMA)) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(LOWEST));
    }
    if (!expectPeek(token::RPAREN)) {
        return std::vector<Expression*>();
    }
    return args;
}

std::vector<Expression*> Parser::parseExpressionList(token_t end) {
    std::vector<Expression*> list;
    if (peekTokenIs(end)) {
        nextToken();
        return list;
    }
    nextToken();
    list.push_back(parseExpression(LOWEST));
    while (peekTokenIs(token::COMMA)) {
        nextToken();
        nextToken();
        list.push_back(parseExpression(LOWEST));
    }
    if (!expectPeek(end)) {
        return std::vector<Expression*>();
    }
    return list;
}

Expression* Parser::parseArrayLiteral() {
    ArrayLiteral* array = new ArrayLiteral(curToken);
    array->elements = parseExpressionList(token::RBRACKET);
    return array;
}

Expression* Parser::parseIndexExpression(Expression* left) {
    IndexExpression* exp = new IndexExpression(curToken, left);
    nextToken();
    exp->index = parseExpression(LOWEST);
    if (!expectPeek(token::RBRACKET)) {
        return nullptr;
    }
    return exp;
}

Expression* Parser::parseHashLiteral() {
    HashLiteral* hash = new HashLiteral(curToken);
    hash->pairs = std::map<Expression*, Expression*>();
    while (!peekTokenIs(token::RBRACE)) {
        nextToken();
        Expression* key = parseExpression(LOWEST);
        if (!expectPeek(token::COLON)) {
            return nullptr;
        }
        nextToken();
        Expression* value = parseExpression(LOWEST);
        hash->pairs[key] = value;
        if (!peekTokenIs(token::RBRACE) && !expectPeek(token::COMMA)) {
            return nullptr;
        }
    }
    if (!expectPeek(token::RBRACE)) {
        return nullptr;
    }
    return hash;
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