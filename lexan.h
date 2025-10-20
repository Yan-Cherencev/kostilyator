#pragma once
#include <vector>

#include "token.h"
//#include "dvaque.h"
#include "hable.h"




class lexan{
    //поля
    std::string buffer;
    size_t position;

    hable<std::string, token_type, hash1> keywords;


    //это что за покемон?
    bool is_whitespace(const char& lit);
    bool is_letter(const char& lit);
    bool is_digit(const char& lit);
    bool is_illegal(const char& lit);


    void skip_whitespace();


    //получение следующих слов
    token get_num();
    token identification();
    

public:
    lexan();

    token get_next_token();

    void load(const std::string& input);

    //std::vector<token> tokenisation(const std::string &input);
};

