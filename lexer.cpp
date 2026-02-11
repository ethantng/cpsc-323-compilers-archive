// ===================================================
// Attached: Assignment 1
// ===================================================
// Program: Lexical Analyzer
// ===================================================
// Programmer: Ethan Nguyen (13801), Brian Zee (13814), Safe Gergis (13814)
// Class: CPSC 323 - Compilers and Program Languages
// ===================================================

#include <fstream>
#include <unordered_set>
#include "lexer.h"

using namespace std;

unordered_set<string> keywords = {
                                "while", "endwhile",
                                "scan", "print",
                                "integer", "boolean", "real", "array", "function",
                                "if", "endif", "else", 
                                "true", "false",
                                "return"
                                };
unordered_set<string> operators = {"<", ">", "<=", "==", "=>", "!=", "=", "+", "++", "-", "--", "*", "/", "%"};
unordered_set<string> separators = {",", "(", ")", ";", "[", "]", "{", "}", "$$"};

bool fsmIdentifier(const string &str)
{
    vector<vector<int>> table = {
    //  {l, d, _}
        {1, 5, 5},      // state 0 start
        {2, 3, 4},      // state 1 accept
        {2, 3, 4},      // state 2 accept
        {2, 3, 4},      // state 3 accept
        {2, 3, 4},      // state 4 accept
        {5, 5, 5}       // state 5
    };

    unordered_set<int> F = {1, 2, 3, 4};

    // FSM for Rat25s identifier: starts with a letter, followed by letters or digits
    int state = 0;
    int col = 0;
    for (char ch : str)
    {
        if (isalpha(ch)) { col = 0; }
        else if (isdigit(ch)) { col = 1; }
        else if (ch == '_') { col = 2; }

        state = table[state][col];
    }
    return F.count(state) > 0; // Valid if we end in accept state
}

bool fsmInteger(const string &str)
{
    vector<vector<int>> table = {   // state table
        {0, 1},  // state 1 start
        {1, 1}   // state 2 accept
    };

    unordered_set<int> F = {1};     // accepting state(s)

    // FSM for Rat25s integer: one or more digits
    int state = 0;
    int col = 0;

    for (char ch : str)
    {
        col = isdigit(ch) ? 1 : 0;    // if digit, column to search is 1, else is 0
        state = table[state][col];
    }
    return F.count(state) > 0; // Valid if we end in accept state
}

bool fsmReal(const string &str)
{
    vector<vector<int>> table = {
    //  {d, .}
        {1, 4},     // state 0 start
        {1, 2},     // state 1
        {3, 4},     // state 2
        {3, 4},     // state 3 accept
        {4, 4}      // state 4
    };

    unordered_set<int> F = {3};

    // FSM for Rat25s real number: digits followed by a decimal point and more digits
    int state = 0;
    int col = 0;
    for (char ch : str)
    {
        if (isdigit(ch)) { col = 0; }
        else if (ch == '.') { col = 1; }

        state = table[state][col];
    }
    return F.count(state) > 0; // Valid if we end in accept state (digits after decimal point)
}

Token lexer(ifstream &file, Token* token)
{
    char ch;       // Current character being processed
    string lexeme; // Current lexeme being built
    // Token token;   // Token to be returned
    bool commenting = false; // Flag to track if we're inside a comment block
    // int lineNumber = 1;

    while (file.get(ch)) // Read characters until end of file
    {
        if (ch == '\n') Token::line++;

        // Check for start of comment block [*
        if (ch == '[' && file.peek() == '*')
        {
            commenting = true;
            file.get(ch); // Consume the '*' character
        }
        // Check for end of comment block *]
        else if (ch == '*' && file.peek() == ']')
        {
            file.get(ch); // Consume the ']' character
            commenting = false;
            continue; // Skip to next iteration
        }

        // Skip whitespace and commented sections
        if (isspace(ch) || commenting)
            continue;

        lexeme = ch; // Start building new lexeme with current character

        // Check if character is a separator (e.g., '(', ')', ';', etc.)
        if (separators.count(lexeme) || ch == '$')
        {
            if (ch == '$' && file.peek() == '$')          // See if separator is '$$'
            {
                file.get(ch);
                lexeme += ch;
            } 
            return {"Separator", lexeme};
        }

        // Check for operators (e.g., '+', '-', '==', '<=', etc.)
        if (operators.count(lexeme) || (ch == '<' || ch == '>' || ch == '=' || ch == '!'))
        {
            file.get(ch); // Look ahead for possible double operators
            string doubleOp = lexeme + ch;

            if (operators.count(doubleOp))
            {
                return {"Operator", doubleOp}; // Return double operator (e.g., '<=')
            }
            else
            {
                file.unget(); // Put back character if not part of double operator
            }
            return {"Operator", lexeme}; // Return single operator
        }

        // Process identifiers and keywords (starting with a letter)
        if (isalpha(ch))
        {
            // Continue reading alphanumeric characters
            while (file.get(ch) && (isalnum(ch) || ch == '_'))
            {
                lexeme += ch;
            }
            file.unget(); // Put back the last character that's not part of the lexeme

            // Check if lexeme is a keyword or identifier
            if (keywords.count(lexeme))
                return {"Keyword", lexeme};
            else if (fsmIdentifier(lexeme))
                return {"Identifier", lexeme};
        }
        // Process numbers (integers and reals)
        else if (isdigit(ch))
        {
            // Continue reading digits and possibly a decimal point
            while (file.get(ch) && (isdigit(ch) || ch == '.'))
            {
                lexeme += ch;
            }
            file.unget(); // Put back the last character that's not part of the number

            // Check if lexeme is a real number or integer
            if (fsmReal(lexeme))
                return {"Real", lexeme};
            if (fsmInteger(lexeme))
                return {"Integer", lexeme};
        }

        // Process unknown strings
        else {
            while (file.get(ch)) {
                if (ch == ' ') { break; }

                lexeme += ch;
                
                // If char is separator or operator, don't add to lexeme
                if (ch == ';' || operators.count(lexeme) || (ch == '<' || ch == '>' || ch == '=' || ch == '!')) {
                    file.unget();
                    lexeme = lexeme.substr(0, lexeme.size()-1);
                    break;
                }
            }
            return {"Unknown", lexeme};
        }
    }
    // Return EOF token when file is completely read'
    return {"EOF", ""};
}