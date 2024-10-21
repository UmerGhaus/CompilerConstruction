#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

enum TokenType {
    T_FLOAT, T_DOUBLE, T_BOOL, T_CHAR, T_STRING, T_JABTAK, T_FOR,
    T_INT, T_ID, T_NUM, T_AGAR, T_WARNA, T_WAPSI,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON,  T_LT, T_LE , T_GT , T_GE , T_EQ, T_NEQ, T_AND, T_OR,
    T_EOF,
};

struct Token {
    TokenType type;
    string value;
    int line;       //Adding line number in Token
};

class Lexer {
private:
    string src;
    size_t pos;
    int line;

public:
    Lexer(const string &src) {
        this->src = src;
        this->pos = 0;
        this->line = 1;     //Initializing Line Number with 1
    }

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];

            if (isspace(current)) {
                if (current == '\n') line++;
                pos++;
                continue;
            }
            // Skip single-line comments
            if (current == '/' && src[pos + 1] == '/') {
                while (pos < src.size() && src[pos] != '\n') pos++;
                continue;
            }

            // Skip multi-line comments
            if (current == '/' && src[pos + 1] == '*') {
                pos += 2;
                while (pos < src.size() && !(src[pos] == '*' && src[pos + 1] == '/')) {
                    if (src[pos] == '\n') line++; 
                    pos++;
                }
                pos += 2; 
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }
            if (isalpha(current)) {
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word, line});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                else if (word == "agar") tokens.push_back(Token{T_AGAR, word, line});
                else if (word == "warna") tokens.push_back(Token{T_WARNA, word, line});
                else if (word == "wapsi") tokens.push_back(Token{T_WAPSI, word, line});
                else if (word == "jabtak") tokens.push_back(Token{T_JABTAK, word, line});
                else if (word == "for") tokens.push_back(Token{T_FOR, word, line});
                else tokens.push_back(Token{T_ID, word, line});
                continue;
            }

            switch (current) {
                case '=':
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_EQ, "==", line});
                        pos++;
                    } else {
                        tokens.push_back(Token{T_ASSIGN, "=", line});
                    }
                    break;
                case '&':
                    if (src[pos + 1] == '&') {
                        tokens.push_back(Token{T_AND, "&&", line});
                        pos++;
                    }
                    break;
                case '|':
                    if (src[pos + 1] == '|') {
                        tokens.push_back(Token{T_OR, "||", line});
                        pos++;
                    }
                    break;
                case '!':
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_NEQ, "!=", line});
                        pos++;
                    }
                    break;
                case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;
                case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                case '>':
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_GE, ">=", line});
                        pos++; // Increment position to skip the '='
                    } else {
                        tokens.push_back(Token{T_GT, ">", line});
                    }
                    break;
                case '<':
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_LE, "<=", line});
                        pos++; // Increment position to skip the '='
                    } else {
                        tokens.push_back(Token{T_LT, "<", line});
                    }
                    break;
                default: 
                    cout << "Unexpected character: " << current << " at line " << line << endl; 
                    exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    }

    string consumeNumber() {
        size_t start = pos;
        bool isPointValue = false;
        int dotCount = 0;

        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) {
            if (src[pos] == '.') {
                dotCount++;
                if (dotCount > 1) {
                    cout << "Syntax error: Invalid number value at line " << line << endl;
                    exit(1);
                }
                isPointValue = true; 
            }
            pos++;
        }

        string number = src.substr(start, pos - start);
        if (isPointValue) {
            return number;  
        } else {
            return number; 
        }
    }


    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }
};

class Parser {
public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || 
            tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR || tokens[pos].type == T_STRING) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_AGAR) {
            parseIfStatement();
        } else if (tokens[pos].type == T_JABTAK) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_FOR) {
            parseForStatement();
        } else if (tokens[pos].type == T_WAPSI) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            // Provide a detailed error message showing unexpected token and line
            cout << "Syntax error: unexpected token " << tokens[pos].value 
                << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        expect(tokens[pos].type); 
        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment() {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement() {
        expect(T_AGAR);    
        expect(T_LPAREN); 
        parseExpression();  
        expect(T_RPAREN);

        if (tokens[pos].type == T_LBRACE) {
            parseBlock();           // If "{" is found, parse the block
        } else {
            parseStatement();       // Otherwise, parse a single statement
        }

        if (tokens[pos].type == T_WARNA) {
            expect(T_WARNA);
            if (tokens[pos].type == T_LBRACE) {
                parseBlock();       // Parse block if "{" is found
            } else {
                parseStatement();   // Otherwise, parse a single statement
            }
        }
    }

    void parseWhileStatement() {
        expect(T_JABTAK);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            parseStatement();
        }
    }

    void parseForStatement() {
        expect(T_FOR);               // Expect the 'for' keyword
        expect(T_LPAREN);           // Expect the opening parenthesis
        
        parseAssignment();           // Parse initialization like `i = 0`
        expect(T_SEMICOLON);        // Expect semicolon after initialization
        
        parseExpression();           // Parse condition like `i < 10`
        expect(T_SEMICOLON);        // Expect semicolon after condition
        
        
        parseAssignment();           // Parse increment like `i = i + 1`
        expect(T_RPAREN);           // Expect closing parenthesis

        if (tokens[pos].type == T_LBRACE) {
            parseBlock();            // If there is a block
        } else {
            parseStatement();        // If it's a single statement
        }
    }

    void parseReturnStatement() {
        expect(T_WAPSI);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT || tokens[pos].type == T_AND || tokens[pos].type == T_OR ||
               tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ) {  
            pos++;
            parseExpression();
        }
    }

    void parseTerm() {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();
        }
    }

    void parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: expected " << type << " but found " << tokens[pos].value << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Please provide a source file." << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "Error opening file." << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
