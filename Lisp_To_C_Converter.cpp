#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <stdexcept>

using namespace std;

// Token class to represent a token
class Token {
public:
    enum class Type { FUNCTION, NUMBER, OPERATOR, LPAREN, RPAREN, INVALID };

//Token type and its value
    Token(Type t, const string& v) : type(t), value(v) {}
    Type type;
    string value;
};

// Lexer function to tokenize the input
vector<Token> lexer(const string& input) {
    vector<Token> tokens;
    stringstream ss(input);
    char ch;

    cout << "\n--- Lexer Output ---\n";
    
    while (ss >> noskipws >> ch) {
        if (isspace(ch)) continue;
        // handle parenthesis
        if (ch == '(') {
            tokens.push_back(Token(Token::Type::LPAREN, "("));
            cout << "Token: LPAREN '(' \n";
        } else if (ch == ')') {
            tokens.push_back(Token(Token::Type::RPAREN, ")"));
            cout << "Token: RPAREN ')'\n";
        } else if (isdigit(ch) || ch == '-') { // Handles negative numbers
            string num(1, ch);
            while (ss >> noskipws >> ch && (isdigit(ch) || ch == '.')) {
                num.push_back(ch);
            }
            tokens.push_back(Token(Token::Type::NUMBER, num));
            cout << "Token: NUMBER '" << num << "'\n";
            ss.putback(ch);
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            tokens.push_back(Token(Token::Type::OPERATOR, string(1, ch)));
            cout << "Token: OPERATOR '" << ch << "'\n";
        } else if (isalpha(ch)) {// for function names
            string func(1, ch);
            while (ss >> noskipws >> ch && isalnum(ch)) {
                func.push_back(ch);
            }
            tokens.push_back(Token(Token::Type::FUNCTION, func));
            cout << "Token: FUNCTION '" << func << "'\n";
            ss.putback(ch);
        } else {
            tokens.push_back(Token(Token::Type::INVALID, string(1, ch)));// no match is found
            cout << "Token: INVALID '" << ch << "'\n";
        }
    }
    return tokens;
}

// Parser class to build an AST
class Parser {
public:
    Parser(const vector<Token>& tokens) : tokens(tokens), current(0) {}

    string parse() {
        cout << "\n--- Parser Output ---\n";
        if (tokens.empty()) return "";

        string result = parseExpression();
        if (current < tokens.size()) {//current token and the token size
            throw runtime_error("Error: Unexpected token at the end.");// if token is beyound limit
        }
        return result;
    }

private:
    vector<Token> tokens;
    size_t current;

    Token peek() const {
        if (current < tokens.size()) {
            return tokens[current];
        }
        return Token(Token::Type::INVALID, "");
    }

    Token advance() {
        if (current < tokens.size()) {
            return tokens[current++];
        }
        return Token(Token::Type::INVALID, "");
    }

    string parseExpression() {
        Token token = peek();

        if (token.type == Token::Type::LPAREN) {
            advance(); // Consume '('
            return parseFunctionCall();
        } else {
            // if it doesn't start with "("
            throw runtime_error("Error: Invalid expression start.");
        }
    }

    string parseFunctionCall() {
        Token functionToken = advance(); // Get the function name
        if (functionToken.type != Token::Type::FUNCTION) {
             //If it isn't a function token
            throw runtime_error("Error: Expected function name.");
        }

        string functionName = functionToken.value;
        string result = functionName + "(";
        cout << "Function Call: " << functionName << "(";

        // Parse arguments
        result += parseArguments();
        cout << "\n";
        return result;
    }

    string parseArguments() {
        string args = "";
        while (true) {
            Token token = peek();
            // parse till you find ")"
            if (token.type == Token::Type::RPAREN) {
                advance(); // Consume ')'
                break;
            }

            if (token.type == Token::Type::NUMBER) {
                Token numToken = advance();
                args += numToken.value;
                cout << "Argument: " << numToken.value << " ";
            } else if (token.type == Token::Type::OPERATOR) {
                Token opToken = advance();
                args += opToken.value;
                cout << "Operator: " << opToken.value << " ";
            } else if (token.type == Token::Type::LPAREN) {
                // Parse nested expressions
                args += "(" + parseExpression() + ")";
            } else {
                throw runtime_error("Error: Unexpected token inside arguments.");
            }

            // Add comma if there are more arguments
            token = peek();
            if (token.type == Token::Type::NUMBER || token.type == Token::Type::OPERATOR || token.type == Token::Type::LPAREN) {
                args += ", ";
            }
        }
        return args;
    }
};

// Function to convert LISP to C-like expression
string convertLispToC(const string& input) {
    vector<Token> tokens = lexer(input);
    Parser parser(tokens);
    return parser.parse();
}

int main() {
    string lispExpression;
    string cExpression;

    while (true) {
        try {
            cout << "\nEnter a LISP expression (or type 'exit' to stop): ";
            getline(cin, lispExpression);

            if (lispExpression == "exit") break;

            cout << "LISP Expression: " << lispExpression << endl;
            cExpression = convertLispToC(lispExpression);
            cout << "C Expression: " << cExpression << endl;

        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            // Prompt user to enter input again
        }
    }

    return 0;
}

