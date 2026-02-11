// ===================================================
// Attached: Assignment 2
// ===================================================
// Program: Syntax Analyzer
// ===================================================
// Programmer: Ethan Nguyen (13801), Brian Zee (13814), Safe Gergis (13814)
// Class: CPSC 323 - Compilers and Program Languages
// ===================================================

#ifndef SYNTAX_H
#define SYNTAX_H

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>
#include "lexer.h"

extern std::vector<Token> tokens;
extern int currentTokenIndex;
extern bool debug;
extern std::ofstream outputFile;

// assembly code gen and symbol table
extern std::unordered_map<std::string, int> symbolTable;
extern std::vector<std::string> instructions;
extern int instructionIndex;
extern int memoryAddress;

void emit(const std::string& instr);
void addToSymbolTable(const std::string& id);
void printInstructions();
void printSymbolTable();

void parse(std::vector<Token> tokensList, const std::string& fileName);
void match(std::string expectedType);
void error(std::string message);

void Rat25s();
void OptFunctionDefs();
void FunctionDefs();
void FunctionDefsPrime();
void Function();
void OptParameterList();
void ParameterList();
void ParameterListPrime();
void Parameter();
void Qualifier();
void Body();
void OptDeclarationList();
void DeclarationList();
void DeclarationListPrime();
void Declaration();
void IDS();
void IDSPrime();
void StatementList();
void StatementListPrime();
void Statement();
void Compound();
void Assign();
void If();
void IfPrime(int jmpElseIndex);
void Return();
void ReturnPrime();
void Print();
void Scan();
void While();
void Condition();
std::string Relop(); //change to string
void Expression();
void ExpressionPrime();
void Term();
void TermPrime();
void Primary();
void PrimaryPrime();
void Factor();
void Empty();

#endif


/* 
======= Production Rules ==========

R1. <Rat25S> ::= $$ <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$
R2. <Opt Function Definitions> ::= <Function Definitions> | <Empty>

R3. <Function Definitions> ::= <Function> | <Function> <Function Definitions>
    -- Left factorization for R3 --
    FD -> F | F FD

    FD -> F FD'
    FD' -> FD | epsilon

R4. <Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
R5. <Opt Parameter List> ::= <Parameter List> | <Empty>

R6. <Parameter List> ::= <Parameter> | <Parameter> , <Parameter List>
    -- Left factorization for R6 --
    PL -> P | P, PL

    PL -> P PL'
    PL' -> , PL | epsilon

R7. <Parameter> ::= <IDs> <Qualifier>
R8. <Qualifier> ::= integer | boolean | real
R9. <Body> ::= { < Statement List> }
R10. <Opt Declaration List> ::= <Declaration List> | <Empty>

R11. <Declaration List> := <Declaration> ; | <Declaration> ; <Declaration List>
    -- Left factorizaion for R11 --
    DL -> D ; | D ; DL

    DL -> D ; DL'
    DL' -> DL | epsilon

R12. <Declaration> ::= <Qualifier > <IDs>

R13. <IDs> ::= <Identifier> | <Identifier>, <IDs>
    -- Left factorization for R13 --
    ID -> I | I, ID

    ID -> I ID'
    ID' -> , ID | epsilon

R14. <Statement List> ::= <Statement> | <Statement> <Statement List>
    -- Left factorization for R14 --
    SL -> S | S SL
    
    SL -> S SL'
    SL' -> SL | epsilon

R15. <Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
R16. <Compound> ::= { <Statement List> }
R17. <Assign> ::= <Identifier> = <Expression> ;

R18. <If> ::= if ( <Condition> ) <Statement> endif | if ( <Condition> ) <Statement> else <Statement> endif
    -- Left factorization for R18 --
    IF -> if ( C ) S endif | if ( C ) S else S endif

    IF -> if ( C ) S IF' endif
    IF' -> else S | epsilon

R19. <Return> ::= return ; | return <Expression> ;
    -- Left factorization for R19 --
    R -> r ; | r E ;

    R -> r R'
    R' -> E ; | ;

R21. <Print> ::= print ( <Expression>);
R21. <Scan> ::= scan ( <IDs> );
R22. <While> ::= while ( <Condition> ) <Statement> endwhile
R23. <Condition> ::= <Expression> <Relop> <Expression>
R24. <Relop> ::= == | != | > | < | <= | =>

R25. <Expression> ::= <Expression> + <Term> | <Expression> - <Term> | <Term>
    -- R25 has LEFT RECURSION --
    E -> E + T | E - T | T

    E -> E' | T
    E' -> +TE' | -TE' | epsilon

R26. <Term> ::= <Term> * <Factor> | <Term> / <Factor> | <Factor>

R27. <Factor> ::= - <Primary> | <Primary>
    -- R26 - 27 has LEFT RECURSION --
    T -> T * F | T / F | F
    F -> -P | P

    T -> FT'
    T' -> *FT' | /FT' | epsilon
    F -> -P | P

R28. <Primary> ::= <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false
    -- Left factorization for R28 --
    P -> I | IN | I ( ID ) | ( E ) | R | t | f

    P -> I P' | IN | ( E ) | R | t | f
    P' -> ( ID ) | epsilon

R29. <Empty> ::= epsilon
*/