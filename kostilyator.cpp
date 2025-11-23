#include <iostream>
#include <fstream>
#include <string>

//#include "dvaque.h"
#include "lexan.h"
#include "hable.h"
#include "syntex.h"
//#include "hable.h"




int main(){
    //std::cout << "Hello World!\n";
    lexan analizator;
    //hable<std::string,token, hash1> lexems;

    std::vector<token> tokens;
    std::vector<std::string> synt_errors;

    std::ifstream in("in.txt");

    /*if (!in.is_open()) {
        std::cout << "File is not found\n";
        return 0;
    }*/

    std::ofstream out("out.txt");

    /*if (!out.is_open()) {
        std::cout << "File is not found\n";
        return 0;
    }*/

    std::string temp;
    size_t line_num = 1;
    while (std::getline(in, temp)) {
        analizator.load(temp);

        token cur_token;

        while ((cur_token = analizator.get_next_token()).type != END_OF_FILE) {
            

            if (cur_token.type == ERROR) {
                out << "Lex error on line " << line_num << ": Invalid token <<" << cur_token.value << ">>\n";
            }
            else {
                cur_token.line = line_num;
                tokens.push_back(cur_token);
            }
        }
        ++line_num;
    }

    tokens.push_back(token(END_OF_FILE, "", line_num));
    in.close();

    syntex synt_analiz(tokens, synt_errors);

    node* root = synt_analiz.start();

    for (const auto& it : synt_errors) {
        out << it << '\n';
    }

    if(synt_errors.empty())    {
        root->print(out);
    }

    

    //dvaque<hable<std::string, token, hash1>::para> lexeme_list = lexems.get_all();

    /*for (size_t i = 0; i < lexeme_list.size(); ++i) {
        const token& t = lexeme_list[i].val;
        out << t.type << " | " << t.value << " | " << lexems.index(t.value) << "\n";
    }*/


    
    out.close();
    
    std::cout << "Success\n";
    return 0;
}

