#include<iostream>
#include<vector>
#include<string>
#include<cctype>
#include<map>
#include<fstream>

using namespace std;

enum TokenType {
    T_INT, T_ID, T_NUM, T_AGAR, T_WARNA, T_RETURN, // Converting if to AGAR and Else to WARNA
    T_ASSIGN, T_MUL, T_MINUS, T_DIV, T_PLUS,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
    T_SEMICOLON, T_GT, T_EOF, T_FLOAT, T_CHAR, T_DOUBLE,
    T_FOR, T_WHILE, T_DO, T_STRING, T_BOOL, T_LT, T_NEQ, T_OR, T_AND, T_EQ

};

struct Token {
    TokenType type;
    string value;
    int line; // To store line number
};

class Symbol {
    public:
        string name; // Stores the name of the symbol
        TokenType type; // Stores the type of the symbol
        string scope; // Stores the scope, Global, function etc
        int line; // Stores at which line number symbol is declared

        Symbol(string name, TokenType type, string scope, int line)
            : name(name), type(type), scope(scope), line(line) {}

        // Default constructor
        Symbol() : name(""), type(T_INT), scope(""), line(0) {}
};

struct TAC {
    string op; // operations 
    string arg1; // first operand
    string arg2; // second operand
    string result; // result variable or label

};

class SymbolTable {
    private:
        vector<map<string, Symbol>> scopes;

    public:
        SymbolTable(){
            scopes.push_back(map<string, Symbol>()); // It is global scope and it is empty initially
        }
        //Adding new symbol to current scope
        void addSymbol(const string& name, TokenType type, const string& scope, int line){
            Symbol symbol(name, type, scope,line);
            scopes.back()[name] = symbol; //this is insert the name into current scope
        }
        //it will lookup symbols from current scopes and outer scopes
        Symbol* lookup(const string& name) {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it){
                auto found = it->find(name);
                if(found != it->end()) {
                    return &found->second; // it will return the symbol from the first scope found
                }
            }

            return nullptr; // if not found then return null pointer
        }
        //enter new scope
        void enterScope(){
            scopes.push_back(map<string, Symbol>());
        }
        //exit current scope
        void exitScope(){
            if(scopes.size()>1){
                scopes.pop_back(); // remove the current scope
                

            }
            else {
                cout <<"Error : Cannot exit global scope!" << endl;
                exit(1);
            }
        }

        // Print the contents of the symbol table
        // Checking if the symbol table is implemented perfectly or not
        void printTable() {
            cout << "Symbol Table:\n";
            for (size_t i = 0; i < scopes.size(); ++i) {
                cout << "Scope " << i << ":\n";
                for (const auto& entry : scopes[i]) {
                    cout << "  " << entry.first << " (Type: " << entry.second.type 
                        << ", Scope: " << entry.second.scope 
                        << ", Line: " << entry.second.line << ")\n";
                }
            }
        }
        
};

class Lexer{

    private:
    string src;
    size_t pos;
    int line; // To track line number

    public:
    Lexer(const string &src){
        this->src = src;
        this->pos = 0;
        this->line = 1;
    }

    vector<Token> tokenize(){
        vector<Token> tokens;
        while(pos < src.size()){
            char current = src[pos];

            if(isspace(current)) {
                if(current == '\n'){
                    line++; // Incrementing Line Number On New Line
                } 
                pos++;
                continue;
            }
            //single line comments
            if (current == '/' && src[pos + 1] == '/') {
                while (pos < src.size() && src[pos] != '\n'){
                    pos++;
                } 
                continue;
            }

            //multi line comments
            if (current == '/' && src[pos + 1] == '*') {
                pos += 2;
                while (pos < src.size() && !(src[pos] == '*' && src[pos + 1] == '/')) {
                    if (src[pos] == '\n') line++; 
                    pos++;
                }
                pos += 2; 
                continue;
            }
            if(isdigit(current)){
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;

            }
            if(isalpha(current)){
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word, line});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line}); //DataType Added
                else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});    //DataType Added
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});  //DataType Added
                else if (word == "string") tokens.push_back(Token{T_STRING, word, line});  //DataType Added
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});   //DataType Added
                else if(word == "if") tokens.push_back(Token{T_AGAR, word, line});
                else if(word == "else") tokens.push_back(Token{T_WARNA, word, line});
                else if(word == "return") tokens.push_back(Token{T_RETURN, word, line});
                else if (word == "for") tokens.push_back(Token{T_FOR, word, line});     
                else if (word == "while") tokens.push_back(Token{T_WHILE, word, line}); 
                else if (word == "do") tokens.push_back(Token{T_DO, word, line});       
                else tokens.push_back(Token{T_ID, word, line});
                continue;
            }

            switch (current)
            {
                case '=':
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_EQ, "==", line});
                        pos++;
                    }else {
                        tokens.push_back(Token{T_ASSIGN, "=", line});
                    }
                    break;
                case '&':
                    if (src[pos + 1] == '&'){
                        tokens.push_back(Token{T_AND, "&&", line});
                        pos++;
                    }
                    break;
                case '|':
                    if(src[pos + 1 ] == '|'){
                        tokens.push_back(Token{T_OR, "||", line});
                        pos++;
                    }
                    break;
                case '!':
                    if(src[pos + 1] == '='){
                        tokens.push_back(Token{T_NEQ, "!=", line});
                        pos++;
                    }
                    break;
            
            case '+' : tokens.push_back(Token{T_PLUS, "+", line});
                break;
            case '-' : tokens.push_back(Token{T_MINUS, "-", line});
                break;
            case '*' : tokens.push_back(Token{T_MUL, "*", line});
                break;
            case '/' : tokens.push_back(Token{T_DIV, "/", line});
                break;
            case '(' : tokens.push_back(Token{T_LPAREN, "(", line});
                break;
            case ')' : tokens.push_back(Token{T_RPAREN, ")", line});
                break;
            case '{' : tokens.push_back(Token{T_LBRACE, "{", line});
                break;
            case '}' : tokens.push_back(Token{T_RBRACE, "}", line});
                break;
            case ';' : tokens.push_back(Token{T_SEMICOLON, ";", line});
                break;
            case '>' : tokens.push_back(Token{T_GT, ">", line});
                break;
            case '<' : tokens.push_back(Token{T_LT, "<", line});
                break;
            default: cout <<"Unexpected Character: " << current << "at line :"<< line << endl; exit(1);
                break;
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    } 


    string consumeNumber(){
        size_t start = pos;
        bool isPointValue;
        int dotCount = 0;

        while(pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')){
            if(src[pos]=='.') {
                dotCount++;
                if(dotCount > 1){
                    cout<<"Syntax Error : Invalid Floating Point Number at Line" << line << endl;
                    exit(1);
                }
                isPointValue = true;
            }
            pos++;
        } 
        string number =  src.substr(start, pos - start);
        if(isPointValue){
            return number; // Return as float datatype if dot is found
        }
        else {
            return number;// return as Integer datatype if dot is not found
        }
    }

    string consumeWord(){
        size_t start = pos;
        while(pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

};

class Parser{

    public:
    Parser(const vector<Token> &tokens) : tokens(tokens), pos(0) , tempVarCount(0), labelCount(0){
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram(){
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No syntax error." << endl;
        printTAC();
    }

    
    private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable symbolTable;
    int tempVarCount;
    int labelCount;
    vector<TAC> tacInstructions;

    string generateTempVar(){
        return "t" + to_string(tempVarCount++);
    }

    string generateLabel(){
        return "L" + to_string(labelCount++);
    }

    void parseStatement(){
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_CHAR || tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING || 
        tokens[pos].type == T_BOOL) {
            parseDeclaration();
        }
        else if(tokens[pos].type == T_ID) {
            parseAssignment();
        }
        else if(tokens[pos].type == T_AGAR){
            parseIfStatment();
        }
        else if (tokens[pos].type == T_RETURN){
            parseReturnStatement();
        }
        else if(tokens[pos].type == T_FOR){
            parseForLoop();
        }
        else if(tokens[pos].type == T_WHILE){
            parseWhileLoop();
        }
        else if(tokens[pos].type == T_DO){
            parseDoWhileLoop();
        }
        else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        }
        else {
            cout << "Syntax error : unexpected token" << tokens[pos].value<< "at line : "<< tokens[pos].line<<endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type){
            pos++;
        }
        else {
            cout << "Syntax error: expected " << type << " but found " << tokens[pos].value << " at line " << tokens[pos].line << endl;
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

    void parseDeclaration(){
        TokenType type = tokens[pos].type;
        expect(type);
        expect(T_ID);
        string varName = tokens[pos-1].value;
        int line  = tokens[pos-1].line;

        //adding the symbol in the symbol table for the current scope
        symbolTable.addSymbol(varName, type, "global", line);
        expect(T_SEMICOLON);
    }

    void parseAssignment(){
        string varName = tokens[pos].value;
        int line = tokens[pos].line;
        expect(T_ID);
        expect(T_ASSIGN);
        
        //lookup the symbol from the symbol table
        Symbol* symbol = symbolTable.lookup(varName);
        if(!symbol) {
            cout<<"Error : Undefined variable " << varName << " at line " << line << endl;
            exit(1);
        }
        string exprResult = parseExpression();
        expect(T_SEMICOLON);

        tacInstructions.push_back({"=", exprResult, "", varName});
    }

    void parseIfStatment(){
        expect(T_AGAR); 
        expect(T_LPAREN);
        string condition = parseExpression();
        expect(T_RPAREN);

        string labelTrue = generateLabel();
        string labelEnd = generateLabel();

        tacInstructions.push_back({"AGAR", condition, "", labelTrue});
        tacInstructions.push_back({"goto", "", "", labelEnd});

        tacInstructions.push_back({"label", "", "", labelTrue});
        parseStatement();
        tacInstructions.push_back({"label", "", "", labelEnd});

        if(tokens[pos].type == T_WARNA){
            expect(T_WARNA);
            parseStatement();
        }
    }

    void parseReturnStatement(){
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

   string parseExpression() {
        string temp1 = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            string op = tokens[pos].value;
            pos++;
            string temp2 = parseTerm();
            string result = generateTempVar();
            tacInstructions.push_back({op, temp1, temp2, result});
            temp1 = result;
        }
        
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT || tokens[pos].type == T_AND || tokens[pos].type == T_OR ||
               tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ) {  
            pos++;
            parseExpression();
        }
        return temp1;
    }

    string parseTerm(){
        string temp1 = parseFactor();
        while(tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            string op = tokens[pos].value;
            pos++;
            string temp2 = parseFactor();
            string result = generateTempVar();
            tacInstructions.push_back({op, temp1, temp2, result});
            temp1 = result;
        }
        return temp1;
    }

    string parseFactor(){
        if(tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            string value = tokens[pos].value;
            pos++;
            return value;
        }
        else if (tokens[pos].type == T_LPAREN) {
            expect (T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else {
            cout << "Syntax Error : unexpected token" << tokens[pos].value << "at line : "<< tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseForLoop(){
        expect(T_FOR);
        expect(T_LPAREN);
        parseAssignment();
       // expect(T_SEMICOLON);
        parseExpression();
        expect(T_SEMICOLON);
        parseAssignment();
        expect(T_RPAREN);
        if  (tokens[pos].type == T_LBRACE) {
            parseBlock();
        }
        else {
            parseStatement();
        }
    }

    void parseWhileLoop(){
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        if(tokens[pos].type == T_LBRACE){
            parseBlock();
        }else {
            parseStatement();
        }
        //expect(T_RBRACE);
    
    }

    void parseDoWhileLoop(){
        expect(T_DO);
        parseStatement();
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
    }

    void printTAC(){
        cout <<" \nThree Address Code (TAC): \n";
        for(const auto& tac : tacInstructions) {
            if (tac.op == "label") {
                cout << tac.result << ":\n";
            }
            else if (tac.op == "if"){
                
                cout << "if" << tac.arg1 <<"goto" << tac.result << "\n"; 
            }
            else if(tac.op == "goto"){
                cout << "goto" << tac.result << "\n";
            }
            else {
                cout << tac.result << " = " << tac.arg1;
                if (!tac.arg2.empty()) cout << " " << tac.op << " " <<tac.arg2;
                cout<< "\n";
            }
        }
    }
};



int main (int argc, char *argv[]){
    
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