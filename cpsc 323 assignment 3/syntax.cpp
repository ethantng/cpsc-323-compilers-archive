#include "syntax.h"
#include <iomanip>
using namespace std;

vector<Token> tokens;
int currentTokenIndex = 0;
bool debug = true;
ofstream outputFile;

//==============================================================
unordered_map<string, int> symbolTable;
vector<string> instructions;
int instructionIndex = 1;
int memoryAddress = 10000;

void emit(const string& instr)
{
    instructions.push_back(to_string(instructionIndex++) + " " + instr);
}

void addToSymbolTable(const string& id) 
{
    if (symbolTable.find(id) == symbolTable.end()) 
    {
        symbolTable[id] = memoryAddress++;
    }
}

void printInstructions() {
    cout << "\nGenerated Assembly Code:\n";
    for (const auto& line : instructions)
        cout << line << endl;
}

void printSymbolTable() {
    cout << "\nSymbol Table\n";
    cout << left << setw(12) << "Identifier" << setw(18) << "MemoryLocation" << "Type" << endl;
    for (const auto& [id, addr] : symbolTable) {
        cout << left << setw(12) << id << setw(18) << addr << "integer" << endl;
    }
    cout << endl;
}
//==============================================================

Token peek() 
{
    if (currentTokenIndex < tokens.size()) 
    {
        return tokens[currentTokenIndex];
    }
    Token::line = -1;
    return 
    {"EOF", ""};
}

Token getNextToken() 
{
    if (currentTokenIndex < tokens.size()) 
    {
        return tokens[currentTokenIndex++];
    }
    Token::line = -1;
    return 
    {"EOF", ""};
}

void match(string expectedType) 
{
    Token token = peek();

    outputFile << "Token: " << token.type << "\t Lexeme: " << token.lexeme << endl;

    if (token.type == expectedType) 
    {
        getNextToken();
    } 
    else 
    {
        error("Expected " + expectedType + " but found " + token.lexeme);
    }
}

void error(string message) 
{
    Token token = peek();
    cerr << "Syntax Error: " << message << " at line " << Token::line << endl;
    outputFile << "Syntax Error: " << message << " at line " << Token::line << endl;
    exit(1);
}


void Rat25s()
{
    Token token = peek();
    if (debug) outputFile << "<Rat25S> -> $$ <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$\n";

    match("Separator");         // $$
    OptFunctionDefs();
    match("Separator");         // $$
    OptDeclarationList();
    match("Separator");         // $$
    StatementList();
    match("Separator");         // $$
}


void OptFunctionDefs() {
    Token token = peek();

    if (token.lexeme != "$$") {
        if (debug) outputFile << "<Opt Function Definitions> -> <Function Definitions>\n";
        FunctionDefs();
    }
    else {
        if (debug) outputFile << "<Opt Function Definitions> -> <Empty>\n";
        Empty();
    }
}


void FunctionDefs() {
    Token token = peek();
    if (debug) outputFile << "<Function Definitions> -> <Function> | <Function> <Function Definitions>\n";
    Function();
    FunctionDefsPrime();
}


void FunctionDefsPrime() {
    Token token = peek();
    if (token.lexeme != "$$") {
        if (debug) outputFile << "<Function Definitions'> -> <Function>\n";
        FunctionDefs();
    }
    else {
        if (debug) outputFile << "<Function Definitions'> -> <Empty>\n";
        Empty();
    }
}


void Function() {
    if (debug) outputFile << "<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n";

    match("Keyword");       // function
    match("Identifier");

    match("Separator");     // (
    OptParameterList();
    match("Separator");     // )

    OptDeclarationList();
    Body();
}   


void OptParameterList() {
    Token token = peek();
    if (token.lexeme != ")") {
        if (debug) outputFile << "<Opt Parameter List> -> <Parameter List>\n";
        ParameterList();
    }
    else {
        if (debug) outputFile << "<Opt Parameter List> -> <Empty>\n";
        Empty();
    }
}

void ParameterList() {
    if (debug) outputFile << "<Parameter List> -> <Parameter> <Parameter List'>\n";
    Parameter();
    ParameterListPrime();
}

void ParameterListPrime() {
    Token token = peek();
    if (token.lexeme == ",") {
        if (debug) outputFile << "<Parameter List'> -> , <Parameter List>\n";

        match("Separator");
        ParameterList();
    }
    else {
        if (debug) outputFile << "<Parameter List'> -> <Empty>\n";
        Empty();
    }
}


void Parameter() {
    if (debug) outputFile << "<Parameter> -> <IDs> <Qualifier>\n";
    IDS();
    Qualifier();
}


void Qualifier() {
    Token token = peek();
    if (token.lexeme == "integer") {
        if (debug) outputFile << "<Qualifier> -> integer\n";
        match("Keyword");
    }
    else if (token.lexeme == "boolean") {
        if (debug) outputFile << "<Qualifier> -> boolean\n";
        match("Keyword");
    }
    else if (token.lexeme == "real") {
        if (debug) outputFile << "<Qualifier> -> real\n";
        match("Keyword");
    }
    else if (token.lexeme == "array") {
        if (debug) outputFile << "<Qualifier> -> array\n";
        match("Keyword");
    }
    else {
        error("Expected integer, boolean, real, or array, but found " + token.lexeme + "\n");
    }
}


void Body() {
    Token token = peek();
    if (token.lexeme == "{") {
        match("Separator");
        StatementList();
        match("Separator");
    }
    else {
        error("Expected '{' but found " + token.lexeme + "\n");
    }
}


void OptDeclarationList() {
    Token token = peek();
    if (token.lexeme != "$$" && token.lexeme != "{") {
        if (debug) outputFile << "<Opt Declaration List> -> <Declaration List>\n";
        DeclarationList();
    }
    else {
        if (debug) outputFile << "<Opt Declaration List> -> <Empty>\n";
        Empty();
    }
}


void DeclarationList() {
    if (debug) outputFile << "<Declaration List> -> <Declaration> ; <Declaration List'>\n";
    Declaration();
    match("Separator");
    DeclarationListPrime();
}

void DeclarationListPrime() {
    Token token = peek();
    if (token.lexeme != "$$") {
        if (debug) outputFile << "<Declaration List'> -> <Declaration List>\n";
        DeclarationList();
    }
    else {
        if (debug) outputFile << "<Declaration List'> -> <Empty>\n";
        Empty();
    }
}


void Declaration() {
    if (debug) outputFile << "<Declaration> -> <Qualifier> <IDs>\n";
    Qualifier();
    IDS();
}


void IDS() {
    Token token = peek();
    if (token.type == "Identifier") {
        if (debug) outputFile << "<IDS> -> <Identifier> <IDS'>\n";

        addToSymbolTable(token.lexeme);

        match("Identifier");
        IDSPrime();
    }
    else {
        error("Expected Identifier, but found " + token.lexeme + "\n");
    }
}


void IDSPrime() {
    Token token = peek();
    if (token.lexeme == ",") {
        if (debug) outputFile << "<IDS'> -> , <IDS>\n";
        match("Separator");
        IDS();
    }
    else {
        if (debug) outputFile << "<IDS'> -> <Empty>\n";
        Empty();
    }
}


void StatementList() {
    if (debug) outputFile << "<Statement List> -> <Statement> <Statement List'>\n";
    Statement();
    StatementListPrime();
}

void StatementListPrime() {
    string lexeme = peek().lexeme;
    if (peek().type == "Identifier" || lexeme == "{" || lexeme == "if" || lexeme == "return" || lexeme == "print" || lexeme == "scan" || lexeme == "while") {
        if (debug) outputFile << "<Statement List'> -> <Statement List>\n";
        StatementList();
    }
    else {
        if (debug) outputFile << "<Statement List'> -> <Empty>\n";
        Empty();
    }
}

void Statement() 
{
    Token token = peek();

    if (token.type == "Identifier") 
    {
        if (debug) outputFile << "<Statement> -> <Assign>\n";
        Assign();
    }
    else if (token.lexeme == "{") {
        if (debug) outputFile << "<Statement> -> <Compound>\n";
        Compound();
    }
    else if (token.lexeme == "if") {
        if (debug) outputFile << "<Statement> -> <If>\n";
        If();
    }
    else if (token.lexeme == "return") {
        if (debug) outputFile << "<Statement> -> <Return>\n";
        Return();
    }
    else if (token.lexeme == "print") {
        if (debug) outputFile << "<Statement> -> <Print>\n";
        Print();
    }
    else if (token.lexeme == "scan") {
        if (debug) outputFile << "<Statement> -> <Scan>\n";
        emit("SIN");
        Scan();
    }
    else if (token.lexeme == "while") 
    {
        if (debug) outputFile << "<Statement> -> <While>\n";
        While();
    }
    else 
    {
        error("Unexpected token" + token.lexeme + "\n");
    }
}


void Compound() {
    if (debug) outputFile << "<Compound> -> { <Statement List> }\n"; 
    match("Separator");     // {
    StatementList();
    match("Separator");     // }
}

//==================================================================================================
void Assign() 
{
    if (debug) outputFile << "<Assign> -> <Identifier> = <Expression> ;\n";

    string id = peek().lexeme;
    addToSymbolTable(id);             // Reserve a memory slot if needed
    match("Identifier");

    match("Operator");                // '='
    Expression();                     // Generates code for RHS

    emit("POPM " + to_string(symbolTable[id]));  // Store result into memory
    match("Separator");              // ';'
}
//==================================================================================================


void If() {
    if (debug) outputFile << "<If> -> if ( <Condition> ) <Statement> <If'> endif\n";
    
    match("Keyword");       // if
    match("Separator");     // (
    Condition();
    match("Separator");     // )

    int jmpElseIndex = instructionIndex;
    emit("JMP0 XX");

    Statement();

    IfPrime(jmpElseIndex);

    match("Keyword");       // endif
}

void IfPrime(int jmpElseIndex) {
    Token token = peek();

    if (token.lexeme == "else") {
        if (debug) outputFile << "<If'> -> else <Statement>\n";

        int jmpEndIfIndex = instructionIndex;
        emit("JMP XX");
        
        instructions[jmpElseIndex-1] = to_string(jmpElseIndex) + " JMP0 " + to_string(instructionIndex);

        match("Keyword");       // else
        Statement();

        instructions[jmpEndIfIndex-1] = to_string(jmpEndIfIndex) + " JMP " + to_string(instructionIndex);
    }
    else {
        if (debug) outputFile << "<If'> -> <Empty>\n";

        instructions[jmpElseIndex-1] = to_string(jmpElseIndex) + " JMP0 " + to_string(instructionIndex);

        Empty();
    }
}


void Return() {
    Token token = peek();
    if (token.lexeme == "return") {
        if (debug) outputFile << "<Return> -> return <Return'>\n";
        match("Keyword");
        ReturnPrime();
    }
}

void ReturnPrime() {
    Token token = peek();
    if (token.lexeme != ";") {
        if (debug) outputFile << "<Return'> -> <Expression> ;\n";
        Expression();
        match("Separator");
    }
    else {
        if (debug) outputFile << "<Return'> -> ;\n";
        match("Separator");
    }
}


void Print() {
    if (debug) outputFile << "<Print> -> print ( <Expression> );\n";
    match("Keyword");       // print
    match("Separator");     // (
    Expression();
    match("Separator");     // )
    match("Separator");     // ;

    emit("SOUT");
}


void Scan() {
    if (debug) outputFile << "<Scan> -> ( <IDs> );\n";
    match("Keyword");       // scan
    match("Separator");     // (

    Token token = peek();
    emit("POPM " + to_string(symbolTable[token.lexeme]));
    IDS();

    match("Separator");     // )
    match("Separator");     // ;
}

//==================================================================================================
void While() 
{
    if (debug) outputFile << "<While> -> while ( <Condition> ) <Statement> endwhile\n";

    int loopStart = instructionIndex;  // Mark beginning
    emit("LABEL");

    match("Keyword");       // while
    match("Separator");     // (
    Condition();            // leaves 0 or 1 on stack
    match("Separator");     // )

    int jmpFalseIndex = instructionIndex;
    emit("JMP0 XX");        // Patch later

    Statement();            // body of while

    emit("JMP " + to_string(loopStart)); // Go back to start

    int loopEnd = instructionIndex;
    instructions[jmpFalseIndex - 1] = to_string(jmpFalseIndex) + " JMP0 " + to_string(loopEnd);

    match("Keyword");       // endwhile
}
//==================================================================================================

//==================================================================================================
void Condition() {
    if (debug) outputFile << "<Condition> -> <Expression> <Relop> <Expression>\n";

    Expression();           // Push left side of condition
    std::string relop = Relop();
    Expression();           // Push right side of condition

    emit(relop);            // Emit the VM comparison instruction (e.g., LEQ, GRT)
}
//==================================================================================================

//==================================================================================================
std::string Relop() {
    Token token = peek();
    std::string op;

    if (token.lexeme == "==") {
        if (debug) outputFile << "<Relop> -> ==\n";
        op = "EQU";
    }
    else if (token.lexeme == "!=") {
        if (debug) outputFile << "<Relop> -> !=\n";
        op = "NEQ";
    }
    else if (token.lexeme == ">") {
        if (debug) outputFile << "<Relop> -> >\n";
        op = "GRT";
    }
    else if (token.lexeme == "<") {
        if (debug) outputFile << "<Relop> -> <\n";
        op = "LES";
    }
    else if (token.lexeme == "<=") {
        if (debug) outputFile << "<Relop> -> <=\n";
        op = "LEQ";
    }
    else if (token.lexeme == "=>") {
        if (debug) outputFile << "<Relop> -> =>\n";
        op = "GEQ";
    }
    else {
        error("Expected relational operator but found " + token.lexeme);
    }

    match("Operator");
    return op;
}
//==================================================================================================


void Expression() 
{
    if (debug) outputFile << "<Expression> -> <Term> <Expression Prime>\n";
    Term();
    ExpressionPrime();
}

//==================================================================================================
void ExpressionPrime() 
{
    if (peek().lexeme == "+") {
        match("Operator");
        if (debug) outputFile << "<Expression Prime> -> + <Term> <Expression Prime>\n";
        Term();
        emit("A");  // Add top two stack values
        ExpressionPrime();
    } 
    else if (peek().lexeme == "-") {
        match("Operator");
        if (debug) outputFile << "<Expression Prime> -> - <Term> <Expression Prime>\n";
        Term();
        emit("S");  // Subtract top two stack values
        ExpressionPrime();
    }
    else {
        if (debug) outputFile << "<Expression Prime> -> <Empty>\n";
        Empty();
    }
}

//==================================================================================================

void Term() 
{
    if (debug) outputFile << "<Term> -> <Factor> <Term Prime>\n";
    Factor();
    TermPrime();
}

//==================================================================================================
void TermPrime() 
{
    if (peek().lexeme == "*") {
        match("Operator");
        if (debug) outputFile << "<TermPrime> -> * <Factor> <TermPrime>\n";
        Factor();
        emit("M");
        TermPrime();
    }
    else if (peek().lexeme == "/") {
        match("Operator");
        if (debug) outputFile << "<TermPrime> -> / <Factor> <TermPrime>\n";
        Factor();
        emit("D");
        TermPrime();
    }
    else {
        if (debug) outputFile << "<TermPrime> -> <Empty>\n";
        Empty();
    }
}
//==================================================================================================

//==================================================================================================
void Primary() {
    Token token = peek();

    if (token.type == "Identifier") {
        if (debug) outputFile << "<Primary> -> <Identifier> <Primary'>\n";

        addToSymbolTable(token.lexeme);  // Reserve memory if needed
        emit("PUSHM " + to_string(symbolTable[token.lexeme]));  // Push value from memory

        match("Identifier");
        PrimaryPrime();
    }
    else if (token.type == "Integer") {
        if (debug) outputFile << "<Primary> -> <Integer>\n";

        emit("PUSHI " + token.lexeme);  // Push integer literal
        match("Integer");
    }
    else if (token.type == "Real") {
        if (debug) outputFile << "<Primary> -> <Real>\n";

        emit("PUSHI " + token.lexeme);  // Push real literal
        match("Real");
    }
    else if (token.lexeme == "(") {
        if (debug) outputFile << "<Primary> -> ( <Expression> )\n";

        match("Separator");  // (
        Expression();        // Handles codegen internally
        match("Separator");  // )
    }
    else if (token.lexeme == "true") {
        if (debug) outputFile << "<Primary> -> true\n";

        emit("PUSHI 1");  // Push 1 for true
        match("Keyword");
    }
    else if (token.lexeme == "false") {
        if (debug) outputFile << "<Primary> -> false\n";

        emit("PUSHI 0");  // Push 0 for false
        match("Keyword");
    }
    else {
        error("Unexpected token in <Primary>: " + token.lexeme);
    }
}
//==================================================================================================

void PrimaryPrime() {
    Token token = peek();
    if (token.lexeme == "(") {
        if (debug) outputFile << "<Primary'> -> ( IDS )\n";
        match("Separator");
        IDS();
        match("Separator");
    }
    else {
        if (debug) outputFile << "<Primary'> -> <Empty>\n";
        Empty();
    }
}


void Factor() 
{
    Token token = peek();
    if (token.lexeme == "-") {
        if (debug) outputFile << "<Factor> -> - <Primary>\n";
        match("Operator");
        Primary();
    }
    else {
        if (debug) outputFile << "<Factor> -> <Primary>\n";
        Primary();
    }
    
}


void Empty() {
    if (debug) outputFile << "<Empty> -> ε\n";
}


void parse(vector<Token> tokensList, const string& fileName) 
{
    tokens = tokensList;
    currentTokenIndex = 0;
    outputFile.open(fileName + "-syntax.txt");

    Rat25s();

    if (peek().type != "EOF") 
    {
        error("Unexpected tokens at the end of input.\n");
    }

    outputFile.close();
}

//=======================================================================
/*

void Parser::Assign() 
{
    string id = currentToken.lexeme;
    match(TokenType::Identifier);
    match(TokenType::Operator, "=");

    Expression();

    addToSymbolTable(id);
    emit("POPM " + to_string(symbolTable[id]));

    match(TokenType::Separator, ";");
}



void Parser::Expression() 
{
    if (currentToken.type == TokenType::Integer) 
    {
        emit("PUSHI " + currentToken.lexeme);
        match(TokenType::Integer);
    } 
    else if (currentToken.type == TokenType::Identifier) 
    {
        addToSymbolTable(currentToken.lexeme);
        emit("PUSHM " + to_string(symbolTable[currentToken.lexeme]));
        match(TokenType::Identifier);
    }

    // Add logic for '+', '-', etc. here if needed
}


void Parser::While() 
{
    int loopStart = instructionIndex;
    emit("LABEL");

    Condition();

    int jmpFalseIndex = instructionIndex;
    emit("JMP0 XX");

    Statement();

    emit("JMP " + to_string(loopStart));

    int loopEnd = instructionIndex;
    instructions[jmpFalseIndex - 1] = to_string(jmpFalseIndex) + " JMP0 " + to_string(loopEnd);
}

*/

