// ===================================================
// Attached: Assignment 1
// ===================================================
// Program: Compiler
// ===================================================
// Programmer: Ethan Nguyen (13801), Brian Zee (13814), Safe Gergis (13814)
// Class: CPSC 323 - Compilers and Program Languages
// ===================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "lexer.h"
#include "syntax.h"

using namespace std;

int Token::line;

int main() 
{
    vector<string> testArr = {
        "test1-simple.rat25s",
        "test2-simple.rat25s",
        "test3-simple.rat25s",
    };

    for (string oFile : testArr) {
        ifstream sourceFile(oFile);
        ofstream outputFileLexer(oFile + "-lexer.txt");

        Token::line = 1;

        if (!sourceFile) 
        {
            cerr << "Error: Cannot open input file." << endl;
            return 1;
        }

        outputFileLexer << "Token\tLexeme\n-------------------\n";

        vector<Token> tokens;
        Token token;

        while ((token = lexer(sourceFile, &token)).type != "EOF") 
        {
            tokens.push_back(token);
            cout << setw(15) << left << token.type << setw(15) << left << token.lexeme << "\t" << Token::line << endl;
            outputFileLexer << setw(15) << left << token.type << token.lexeme << endl;
        }
        tokens.push_back({"EOF", ""});

        sourceFile.close();
        outputFileLexer.close();

        // Run Syntax Analyzer
        parse(tokens, oFile);
        
        // Run Assembly Code Generation & Symbol Table
        printInstructions();
        printSymbolTable();
        instructions.clear();
        symbolTable.clear();
        instructionIndex = 1;
        memoryAddress = 10000;

    }
    
    return 0;
}