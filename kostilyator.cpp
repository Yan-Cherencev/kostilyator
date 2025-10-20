#include <iostream>
#include <fstream>
#include <string>

//#include "dvaque.h"
#include "lexan.h"
#include "hable.h"


int main(){
    //std::cout << "Hello World!\n";
    lexan analizator;
    hable<std::string,token, hash1> lexems;

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
                out << "Error on line " << line_num << ": Invalid token <<" << cur_token.value << ">>\n";
            }
            else {
                lexems[cur_token.value] = cur_token;
            }
        }
        ++line_num;
    }

    //hable<int, int> a;

    /*for (int i = 0; i < 1000; ++i) {
        a[i] = i;
        std::cout <<i<<' ' << a.index(i) << '\n';
    }*/

    dvaque<hable<std::string, token, hash1>::para> lexeme_list = lexems.get_all();

    for (size_t i = 0; i < lexeme_list.size(); ++i) {
        const token& t = lexeme_list[i].val;
        out << t.type << " | " << t.value << " | " << lexems.index(t.value) << "\n";
    }


    in.close();
    out.close();
    
    std::cout << "Success\n";
    return 0;
}

