#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <stdexcept>

enum struct TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    LPAREN,
    RPAREN,
    SIMPLIFY
};

struct Token {
    TokenType type;
    std::string text;
    int value;

    Token(TokenType type, const std::string& text = "") : type(type), text(text), value(0) {}
    Token(TokenType type, int value = 0) : type(type), value(value) {}
};

struct Lexer {
    std::string input;
    size_t pos;

    Lexer(const std::string& input) : input(input), pos(0) {}

    int parseNumber() {
        size_t startPos = pos;

        while (pos < input.size() && (std::isdigit(input[pos]))) {
            pos++;
        }

        return std::stod(input.substr(startPos, pos - startPos));
    }

    std::string parseIdentifier() {
        size_t startPos = pos;

        while (pos < input.size() && std::isalpha(input[pos])) {
            pos++;
        }

        return input.substr(startPos, pos - startPos);
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < input.size()) {
            char currentChar = input[pos];

           if (std::isspace(currentChar)) {
                pos++;
                continue;
            }

            if (currentChar == '(') {
                tokens.push_back(Token(TokenType::LPAREN, "("));
                pos++;
                continue;
            }
            if (currentChar == ')') {
                tokens.push_back(Token(TokenType::RPAREN, ")"));
                pos++;
                continue;
            }
            if (std::isdigit(currentChar)) {
                tokens.push_back(Token(TokenType::NUMBER, parseNumber()));
                continue;
            }
            if (currentChar == '+') {
                tokens.push_back(Token(TokenType::PLUS, "+"));
                pos++;
                continue;
            }
            if (currentChar == '-') {
                tokens.push_back(Token(TokenType::MINUS, "-"));
                pos++;
                continue;
            }
            if (currentChar == '*') {
                tokens.push_back(Token(TokenType::MULTIPLY, "*"));
                pos++;
                continue;
            }
            if (std::isalpha(currentChar)) {
                tokens.push_back(Token(TokenType::SIMPLIFY, parseIdentifier()));
                continue;
            }

            throw std::runtime_error("Unknown character in input.");
        }
        return tokens;
    }
};

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct NumberNode : public ASTNode {
    int value;
    NumberNode(int value) : value(value) {}
};

struct UnaryOpNode : public ASTNode {
    std::unique_ptr<ASTNode> child;
    TokenType op;

    UnaryOpNode(std::unique_ptr<ASTNode> child, TokenType op)
        : child(std::move(child)), op(op) {}
};

struct BinaryOpNode : public ASTNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    TokenType op;

    BinaryOpNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, TokenType op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
};

struct Parser {
    std::vector<Token>& tokens;
    size_t currentTokenIndex;

    Parser(std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0){}

    std::unique_ptr<ASTNode> parse() {
        return parseStatement();
    }

    Token currentToken() const {
        return tokens[currentTokenIndex];
    }

    Token peek() const {
        return tokens[currentTokenIndex + 1];
    }

    void next() {
        if (currentTokenIndex < tokens.size()) {
            currentTokenIndex++;
        }
    }

    std::unique_ptr<ASTNode> parseExpression() {
        if (currentToken().type == TokenType::LPAREN) {
            next();
            std::unique_ptr<ASTNode> result;
            std::unique_ptr<ASTNode> left;
            std::unique_ptr<ASTNode> right;
            TokenType op = currentToken().type;
            switch (op) {
                case TokenType::PLUS:
                case TokenType::MULTIPLY:
                    next();
                    left = parseExpression();
                    right = parseExpression();

                    result = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
                    break;
                case TokenType::MINUS:
                    next();
                    left = parseExpression();
                    if (currentToken().type == TokenType::RPAREN) {
                        result = std::make_unique<UnaryOpNode>(std::move(left), op);
                    } else {
                        right = parseExpression();
                        result = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
                    }
                default:
                    break;
            }
            next();
            return result;
        } else if (currentToken().type == TokenType::NUMBER) {
            int value = currentToken().value;
            next();
            return std::make_unique<NumberNode>(value);
        } else {
            throw std::runtime_error("Invalid LISP expression, expected '(' or number");
        }
    }

    std::unique_ptr<ASTNode> parseStatement() {
        if (currentToken().type == TokenType::LPAREN) {
            next();
            if (currentToken().type == TokenType::SIMPLIFY) {
                next();
                TokenType op = TokenType::SIMPLIFY;
                std::unique_ptr<ASTNode> child = parseExpression();
                return std::make_unique<UnaryOpNode>(std::move(child), op);
            } else {
                throw std::runtime_error("Invalid LISP expression, expected 'simplify'");
            }
        } else {
            throw std::runtime_error("Invalid LISP expression, expected '('");
        }
    }
};

struct Evaluator {
    int evaluate(const ASTNode* node) {
        if (const auto* numNode = dynamic_cast<const NumberNode*>(node)) {
            return numNode->value;
        }
        if (const auto* unOpNode = dynamic_cast<const UnaryOpNode*>(node)) {
            if (unOpNode->op == TokenType::MINUS) {
                return -1 * evaluate(unOpNode->child.get());;
            } else {
                return evaluate(unOpNode->child.get());
            }
        }
        if (const auto* binOpNode = dynamic_cast<const BinaryOpNode*>(node)) {
            int leftVal = evaluate(binOpNode->left.get());
            int rightVal = evaluate(binOpNode->right.get());
            switch (binOpNode->op) {
                case TokenType::PLUS:
                    return leftVal + rightVal;
                case TokenType::MINUS:
                    return leftVal - rightVal;
                case TokenType::MULTIPLY:
                    return leftVal * rightVal;
                default:
                    throw std::runtime_error("Unknown operator.");
            }
        }
        throw std::runtime_error("Invalid AST node.");
    }
};

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "Tokens: [";
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::NUMBER:
                std::cout << "NUMBER(" << token.value << "), ";
                break;
            case TokenType::PLUS:
                std::cout << "PLUS(+), ";
                break;
            case TokenType::MINUS:
                std::cout << "MINUS(-), ";
                break;
            case TokenType::MULTIPLY:
                std::cout << "MULTIPLY(*), ";
                break;
            case TokenType::LPAREN:
                std::cout << "LPAREN(()), ";
                break;
            case TokenType::RPAREN:
                std::cout << "RPAREN()), ";
                break;
            case TokenType::SIMPLIFY:
                std::cout << "SIMPLIFY, ";
                break;
        }
    }
    std::cout << "]" << std::endl;
}

void printIndentation(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
}

void printAST(const ASTNode* node, int level = 0) {
    printIndentation(level);

    if (const auto* numNode = dynamic_cast<const NumberNode*>(node)) {
        std::cout << "Number(" << numNode->value << ")" << std::endl;
    } else if (const auto* unOpNode = dynamic_cast<const UnaryOpNode*>(node)) {
        std::string opStr;
        switch (unOpNode->op) {
            case TokenType::SIMPLIFY:
                opStr = "SIMPLIFY";
                break;
            case TokenType::MINUS:
                opStr = "MINUS(-)";
                break;
            default:
                opStr = "UNKNOWN_OPERATOR";
                break;
        }

        std::cout << "UnaryOp(" << opStr << ")" << std::endl;
        printIndentation(level);
        std::cout << "Child:" << std::endl;
        printAST(unOpNode->child.get(), level + 1);
    } else if (const auto* binOpNode = dynamic_cast<const BinaryOpNode*>(node)) {
        std::string opStr;
        switch (binOpNode->op) {
            case TokenType::PLUS:
                opStr = "PLUS(+)";
                break;
            case TokenType::MINUS:
                opStr = "MINUS(-)";
                break;
            case TokenType::MULTIPLY:
                opStr = "MULTIPLY(*)";
                break;
            default:
                opStr = "UNKNOWN_OPERATOR";
                break;
        }

        std::cout << "BinaryOp(" << opStr << ")" << std::endl;
        printIndentation(level);
        std::cout << "Left:" << std::endl;
        printAST(binOpNode->left.get(), level + 1);

        printIndentation(level);
        std::cout << "Right:" << std::endl;
        printAST(binOpNode->right.get(), level + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file-path>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            Lexer lexer(line);
            std::vector<Token> tokens = lexer.tokenize();
            // printTokens(tokens);
            Parser parser(tokens);
            std::unique_ptr<ASTNode> ast = parser.parse();
            // printAST(ast.get());

            Evaluator evaluator;
            int result = evaluator.evaluate(ast.get());
            if (result < 0) {
                std::cout << "(- " << result * -1 << ")" << std::endl;
            } else {
                std::cout << result << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing expression: " << line << " - " << e.what() << std::endl;
        }
    }

    file.close();
    return 0;
}
