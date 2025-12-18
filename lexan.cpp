#include "lexan.h"




lexan::lexan():position(0) {

    keywords["PROGRAM"] = PROGRAM;
    keywords["END"] = END;
    keywords["CALL"] = CALL;
    keywords["INT"] = KEYWORD;
    keywords["REAL"] = KEYWORD;
    keywords["CONST"] = CONST;


    /*keywords["program"] = token(token_type::PROGRAM, "program");

    keywords["end"] = token(token_type::END, "end");
    keywords["call"] = token(token_type::CALL, "call");

    keywords["int"] = token(token_type::KEYWORD, "int");
    keywords["float"] = token(token_type::KEYWORD, "float");
    keywords["const"] = token(token_type::KEYWORD, "const");
    
    keywords[","] = token(token_type::PUNCTUATOR, ",");*/

    //keywords["+"] = token(token_type::OPERATOR, "+");
    //keywords["-"] = token(token_type::OPERATOR, "-");
    //keywords["%"] = token(token_type::OPERATOR, "%");
    //keywords["/"] = token(token_type::OPERATOR, "/");
    //keywords["*"] = token(token_type::OPERATOR, "*");
}



void lexan::load(const std::string& input){
    buffer = input;
    position = 0;
}


bool lexan::is_whitespace(const char& lit){
    return lit ==' '||lit=='\n'||lit =='\t';
}
bool lexan::is_letter(const char& lit){
    return (lit >= 'a' && lit <= 'z') || (lit >= 'A' && lit <= 'Z') || (lit=='_');
}
bool lexan::is_digit(const char& lit){
    return (lit >= '0' && lit <= '9');
}
bool lexan::is_illegal(const char& lit){
    return !(is_digit(lit)|| is_whitespace(lit)||is_letter(lit));
}


token lexan::identification() {
    size_t start = position;

    while (position < buffer.size() && is_letter(buffer[position] )) { //не забыть вырезать цифры
        ++position;
    }

    std::string word = buffer.substr(start, position - start);

    const token_type* type = keywords.find(word);
    if (type != nullptr) {
        return token(*type, word);
    }

    return token(ID, word);
}

void lexan::skip_whitespace(){
    while (position < buffer.size() && is_whitespace(buffer[position])) {
        ++position;
    }
}

token lexan::get_next_token(){
    skip_whitespace();

    if (position >= buffer.size()) {
        return token(END_OF_FILE, "");
    }
    
    char current = buffer[position];

    if (is_letter(current)) {
        return identification();
    }
    else if (is_digit(current)) {
        return get_num();
    }

    ++position;

    switch (current){
        case ',': return token(PUNCTUATOR, ",");
        case '(': return token(PUNCTUATOR, "(");
        case ')': return token(PUNCTUATOR, ")");
        case ';': return token(PUNCTUATOR, ";");

        case '+': return token(OPERATOR, "+");
        case '-': return token(OPERATOR, "-");
        case '*': return token(OPERATOR, "*");
        case '/': return token(OPERATOR, "/");
        case '=': return token(OPERATOR, "=");
    default:
        return token(UNKNOW, std::string(1, current));
    }
}

token lexan::get_num(){

    size_t start = position;
    bool is_float = false;

    while (position < buffer.size() && is_digit(buffer[position]) ) {
        ++position;
    }

    if (position < buffer.length() && buffer[position] == '.') {
        is_float = true;
        ++position;

        if (position >= buffer.length() || !is_digit(buffer[position])) {
            return token(UNKNOW, buffer.substr(start, position - start));
        }

        while (start < buffer.size() && is_digit(buffer[position])) {
            ++position;
        }
    }
    std::string num = buffer.substr(start, position - start);

    /*const token_type* type = keywords.find(word);
    if (type != nullptr) {
        return token(*type, word);
    }*/

    if (buffer[start] == '0'&& buffer[start+1]!= '.' && num.size()>1) {
        return token(UNKNOW, num);
    }

    if (is_float) {
        return token(FLOAT_LIT, num);
    }
    return token(INTEGER_LIT, num);
}


//std::vector<token> lexan::tokenisation(const std::string &input){
//
//    std::vector<token> tokens;
//    
//    size_t pos=0;
//    
//    while(pos<input.size()){
//        
//        char temp=input[pos];
//        
//
//        if (is_whitespace(temp)) {
//            ++pos;
//            continue;
//        }
//        else if (is_letter(temp)) {
//
//            std::string word="";
//
//            while (pos< input.size() && is_letter(input[pos])) {
//
//                word += input[pos++];
//            }
//            //std::cout << word << '\n';
//            if (keywords.find(word)!=nullptr) {
//                tokens.push_back(keywords[word]);
//            }
//            else {
//                tokens.push_back(token(token_type::ID, word));
//            }
//            std::cout << word << '\n';
//            continue;
//        }
//
//        else if (is_digit(temp)) {
//
//            bool real =  false;
//            std::string num = "";
//            //num += temp;
//            
//
//            while (pos < input.size() && is_digit(input[pos])) {
//                num += input[pos++];
//            }
//
//            if (input[pos] == '.'&& pos < input.size()) {
//                real = true;
//
//                num += input[pos++];
//
//
//                while (pos<input.size() && is_digit(input[pos])) {
//                    num += input[pos++];
//                }
//
//                /*if (!is_digit(input[pos])) {
//                    tokens.push_back(token(token_type::ERROR, num));
//                    ++pos;
//                    continue;
//                }*/
//
//                
//
//            }
//
//            if (real) {
//                tokens.push_back(token(token_type::FLOAT_LIT, num));
//            }
//            else {
//                tokens.push_back(token(token_type::INTEGER_LIT, num));
//            }
//            continue;
//        }
//
//
//        switch (temp){
//
//        case ',':
//            tokens.push_back(token(token_type::PUNCTUATOR, ","));
//            break;
//
//        case '(':
//            tokens.push_back(token(token_type::PUNCTUATOR, "("));
//            break;
//            
//        case ')':
//            tokens.push_back(token(token_type::PUNCTUATOR, ")"));
//            break;
//
//        case '+':
//            tokens.push_back(token(token_type::OPERATOR, "+"));;
//            break;
//
//        case '-':
//            tokens.push_back(token(token_type::OPERATOR, "-"));
//            break;
//
//        case '*':
//            tokens.push_back(token(token_type::OPERATOR, "*"));
//            break;
//
//        case '/':
//            tokens.push_back(token(token_type::OPERATOR, "/"));
//            break;
//
//        case '=':
//            tokens.push_back(token(token_type::OPERATOR, "="));
//            break;
//        case ';':
//            tokens.push_back(token(token_type::PUNCTUATOR, ";"));
//            break;
//
//
//        default:
//            tokens.push_back(token(token_type::ERROR, std::string(1, temp)));
//            break;
//        }
//
//        
//
//        ++pos;
//    }
//
//
//
//
//    return tokens;
//}


