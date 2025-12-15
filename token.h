#pragma once

#include <fstream>
#include <string>

enum token_type {
    KEYWORD,

    PROGRAM, END, CALL,
    CONST, INTEGER_LIT, FLOAT_LIT,

    ID,

    OPERATOR,
    PUNCTUATOR,

    END_OF_FILE,
    

    UNKNOWN, UNKNOW
};



struct token {
    token_type type;
    std::string value;
    size_t line;
    //и что-то ещё

    token() :type(UNKNOWN), value(""), line(0) {};
    token(token_type typ /*= token_type::UNKNOWN*/, const std::string& val, size_t line = 0) :type(typ), value(val), line(line) {};
};

inline std::ostream& operator<<(std::ostream& os, token_type type) {
    switch (type) {
    case KEYWORD:       return os << "KEYWORD";
        case PROGRAM:       return os << "PROGRAM";
        case END:           return os << "END";
        case CALL:          return os << "CALL";
        case CONST:         return os << "CONST";
        case INTEGER_LIT:   return os << "INTEGER_LIT";
        case FLOAT_LIT:     return os << "FLOAT_LIT";
        case ID:            return os << "ID";
        case OPERATOR:      return os << "OPERATOR";
        case PUNCTUATOR:    return os << "PUNCTUATOR";
        case END_OF_FILE:   return os << "EOF";
        case UNKNOWN:       return os << "UNKNOWN";
        case UNKNOW:         return os << "ERROR";
    default:            return os << "UNKNOWN";
    }
}

