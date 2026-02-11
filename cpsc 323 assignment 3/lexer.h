#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>

struct Token {
    std::string type;
    std::string lexeme;
    static int line;
};


Token lexer(std::ifstream &file, Token* token);

#endif
