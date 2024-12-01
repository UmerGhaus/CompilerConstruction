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
    int line;       
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

class SymbolTable {
public:

    void declareVariable(const string &name, const string &type) {
        if (symbolTable.find(name) != symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is already declared.");
        }
        symbolTable[name] = type;
    }

    string getVariableType(const string &name) {
        if (symbolTable.find(name) == symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
        }
        return symbolTable[name];
    }

    bool isDeclared(const string &name) const {
        return symbolTable.find(name) != symbolTable.end();
    }

private:
    map<string, string> symbolTable;
};

class IntermediateCodeGnerator {
public:
    vector<string> instructions;
    int tempCount = 0;
    int lblCount = 1;

    string newTemp() {
        return "t" + to_string(tempCount++);
    }
    string newLabel(){
        return "L" + to_string(lblCount++);
    }

    void addInstruction(const string &instr) {
        instructions.push_back(instr);
    }

    void printInstructions() {
        for (const auto &instr : instructions) {
            cout << instr << endl;
        }
    }

};

class Parser {
public:
    Parser(const vector<Token> &tokens, SymbolTable &symTable, IntermediateCodeGnerator &icg)
        : tokens(tokens), pos(0), symTable(symTable), icg(icg) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable &symTable;
    IntermediateCodeGnerator &icg;

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
            cout << "Syntax error: unexpected token " << tokens[pos].value 
                << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    
    void parseDeclaration() {
        TokenType varType = tokens[pos].type; 
        expect(varType);
        string varName = expectAndReturnValue(T_ID);
        string typeString;
        switch (varType) {
            case T_INT:
                typeString = "int";
                break;
            case T_FLOAT:
                typeString = "float";
                break;
            case T_DOUBLE:
                typeString = "double";
                break;
            case T_BOOL:
                typeString = "bool";
                break;
            case T_CHAR:
                typeString = "char";
                break;
            case T_STRING:
                typeString = "string";
                break;
            default:
                throw std::runtime_error("Unsupported type in declaration");
        }
        symTable.declareVariable(varName, typeString);
        expect(T_SEMICOLON);
    }


    void parseAssignment() {
        string varName = expectAndReturnValue(T_ID);
        symTable.getVariableType(varName);  
        expect(T_ASSIGN);
        string expr = parseExpression();
        icg.addInstruction(varName + " = " + expr); 
        expect(T_SEMICOLON);
    }
    void parseForAssignment() {
        string varName = expectAndReturnValue(T_ID);
        symTable.getVariableType(varName);  
        expect(T_ASSIGN);
        string expr = parseExpression();
        icg.addInstruction(varName + " = " + expr); 
    }
    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseIfStatement() {
        expect(T_AGAR);    
        expect(T_LPAREN); 
        string cond = parseExpression();
        expect(T_RPAREN);

        string trueLabel = icg.newLabel();  // Generate new label for the true block
        string falseLabel = icg.newLabel(); // Generate new label for the false block
        string endLabel = icg.newLabel();   // Generate label for the end of the if-else

        icg.addInstruction("agar " + cond + " goto " + trueLabel); 
        icg.addInstruction("goto " + falseLabel); 

        icg.addInstruction(trueLabel + ":");   // True block
        if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            parseStatement();
        }

        if (tokens[pos].type == T_WARNA) { // Else block
            icg.addInstruction("goto " + endLabel);
            icg.addInstruction(falseLabel + ":");
            expect(T_WARNA);
            if (tokens[pos].type == T_LBRACE) {
                parseBlock();
            } else {
                parseStatement();
            }
            icg.addInstruction(endLabel + ":");
        } else {
            icg.addInstruction(falseLabel + ":");
        }
    }


    void parseWhileStatement() {
        expect(T_JABTAK);
        expect(T_LPAREN);
        string cond = parseExpression();
        expect(T_RPAREN);

        string startLabel = icg.newLabel();   // Label for the start of the loop
        string trueLabel = icg.newLabel();    // Label for the true block
        string endLabel = icg.newLabel();     // Label for exiting the loop

        icg.addInstruction(startLabel + ":");
        icg.addInstruction("agar " + cond + " goto " + trueLabel); 
        icg.addInstruction("goto " + endLabel);

        icg.addInstruction(trueLabel + ":");
        if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            parseStatement();
        }
        icg.addInstruction("goto " + startLabel);
        icg.addInstruction(endLabel + ":");
    }


    void parseForStatement() {
        expect(T_FOR);               
        expect(T_LPAREN);            

        // Initialization
        parseAssignment();           // Handles the initialization (e.g., `i = 0;`)

        
        string conditionLabel = icg.newLabel(); // Label for condition check
        string bodyLabel = icg.newLabel();      // Label for the loop body
        string incrementLabel = icg.newLabel(); // Label for increment step
        string endLabel = icg.newLabel();       // Label for loop exit

        // Generate the condition check label
        icg.addInstruction(conditionLabel + ":");
        
        // Parse and evaluate the condition
        string cond = parseExpression();        // Handles condition (e.g., `i < 10`)
        expect(T_SEMICOLON);
        icg.addInstruction("agar " + cond + " goto " + bodyLabel);
        icg.addInstruction("goto " + endLabel); // Exit if condition is false

        
        // Increment/Update step
        icg.addInstruction(incrementLabel + ":");
        parseForAssignment();                       // Handles increment (e.g., `i = i + 1`)
        expect(T_RPAREN);
        icg.addInstruction("goto " + conditionLabel);  // Jump back to the condition check

        // Loop body
        icg.addInstruction(bodyLabel + ":");
        if (tokens[pos].type == T_LBRACE) {
            parseBlock();                       // Parse block if `{}` is used
        } else {
            parseStatement();                   // Parse single statement
        }
        icg.addInstruction("goto " + incrementLabel); // Jump back to the incremental label

        // End of the loop
        icg.addInstruction(endLabel + ":");
    }


    void parseReturnStatement() {
        expect(T_WAPSI);
        string expr = parseExpression();
        icg.addInstruction("wapsi " + expr); 
        expect(T_SEMICOLON);
    }
    string parseExpression() {
        string term = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            TokenType op = tokens[pos++].type;
            string nextTerm = parseTerm(); 
            string temp = icg.newTemp();  
            icg.addInstruction(temp + " = " + term + (op == T_PLUS ? " + " : " - ") + nextTerm);
            term = temp;
        }
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT || tokens[pos].type == T_AND || tokens[pos].type == T_OR ||
             tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ) {
            TokenType operatorType = tokens[pos].type;
            pos++; 
            string nextExpr = parseExpression();
            string operatorString;
            switch (operatorType) {
                case T_GT:
                    operatorString = ">";
                    break;
                case T_LT:
                    operatorString = "<";
                    break;
                case T_AND:
                    operatorString = "&&";
                    break;
                case T_OR:
                    operatorString = "||";
                    break;
                case T_EQ:
                    operatorString = "==";
                    break;
                case T_NEQ:
                    operatorString = "!=";
                    break;
                default:
                    throw std::runtime_error("Unsupported operator in expression");
            }

            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term + " " + operatorString + " " + nextExpr);
            term = temp;
        }

        return term;
    }

    string parseTerm() {
        string factor = parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            TokenType op = tokens[pos++].type;
            string nextFactor = parseFactor();
            string temp = icg.newTemp(); 
            icg.addInstruction(temp + " = " + factor + (op == T_MUL ? " * " : " / ") + nextFactor); 
            factor = temp;
        }
        return factor;
    }

    string parseFactor() {
        if (tokens[pos].type == T_NUM) {
            return tokens[pos++].value;
        } else if (tokens[pos].type == T_ID) {
            return tokens[pos++].value;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            string expr = parseExpression();
            expect(T_RPAREN);
            return expr;
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].line << endl;
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
    string expectAndReturnValue(TokenType type) {
        string value = tokens[pos].value;
        expect(type);
        return value;
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

    SymbolTable symTable;
    IntermediateCodeGnerator icg;
    Parser parser(tokens, symTable, icg);
    
    parser.parseProgram();
    icg.printInstructions();

    return 0;
}
