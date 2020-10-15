#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdlib.h>
std::string input = R"(AAAAAAAAAAAAAAAAAAAA
BBBBBBBBBBBBBBBBBBBB
CCCCCCCCCCCCCCCCCCCC
DDDDDDDDDDDDDDDDDDDD
EEEEEEEEEEEEEEEEEEEE
FFFFFFFFFFFFFFFFFFFF
GGGGGGGGGGGGGGGGGGGG
HHHHHHHHHHHHHHHHHHHH
IIIIIIIIIIIIIIIIIIII
JJJJJJJJJJJJJJJJJJJJ
KKKKKKKKKKKKKKKKKKKK
LLLLLLLLLLLLLLLLLLLL
MMMMMMMMMMMMMMMMMMMM
NNNNNNNNNNNNNNNNNNNN
OOOOOOOOOOOOOOOOOOOO
PPPPPPPPPPPPPPPPPPPP
QQQQQQQQQQQQQQQQQQQQ
RRRRRRRRRRRRRRRRRRRR
SSSSSSSSSSSSSSSSSSSS
TTTTTTTTTTTTTTTTTTTT
UUUUUUUUUUUUUUUUUUUU
VVVVVVVVVVVVVVVVVVVV
WWWWWWWWWWWWWWWWWWWW
XXXXXXXXXXXXXXXXXXXX
YYYYYYYYYYYYYYYYYYYY
ZZZZZZZZZZZZZZZZZZZZ
AAAAAAAAAAAAAAAAAAAA
BBBBBBBBBBBBBBBBBBBB
CCCCCCCCCCCCCCCCCCCC
DDDDDDDDDDDDDDDDDDDD
EEEEEEEEEEEEEEEEEEEE
FFFFFFFFFFFFFFFFFFFF
GGGGGGGGGGGGGGGGGGGG
HHHHHHHHHHHHHHHHHHHH
IIIIIIIIIIIIIIIIIIII
JJJJJJJJJJJJJJJJJJJJ
KKKKKKKKKKKKKKKKKKKK
LLLLLLLLLLLLLLLLLLLL
MMMMMMMMMMMMMMMMMMMM
NNNNNNNNNNNNNNNNNNNN
OOOOOOOOOOOOOOOOOOOO
PPPPPPPPPPPPPPPPPPPP
QQQQQQQQQQQQQQQQQQQQ
RRRRRRRRRRRRRRRRRRRR
SSSSSSSSSSSSSSSSSSSS
TTTTTTTTTTTTTTTTTTTT
UUUUUUUUUUUUUUUUUUUU
VVVVVVVVVVVVVVVVVVVV
WWWWWWWWWWWWWWWWWWWW
XXXXXXXXXXXXXXXXXXXX
YYYYYYYYYYYYYYYYYYYY
ZZZZZZZZZZZZZZZZZZZZ
AAAAAAAAAAAAAAAAAAAA
BBBBBBBBBBBBBBBBBBBB
CCCCCCCCCCCCCCCCCCCC
DDDDDDDDDDDDDDDDDDDD
EEEEEEEEEEEEEEEEEEEE
FFFFFFFFFFFFFFFFFFFF
GGGGGGGGGGGGGGGGGGGG
HHHHHHHHHHHHHHHHHHHH
IIIIIIIIIIIIIIIIIIII
JJJJJJJJJJJJJJJJJJJJ
KKKKKKKKKKKKKKKKKKKK
LLLLLLLLLLLLLLLLLLLL
MMMMMMMMMMMMMMMMMMMM
NNNNNNNNNNNNNNNNNNNN
OOOOOOOOOOOOOOOOOOOO
PPPPPPPPPPPPPPPPPPPP
QQQQQQQQQQQQQQQQQQQQ
RRRRRRRRRRRRRRRRRRRR
SSSSSSSSSSSSSSSSSSSS
TTTTTTTTTTTTTTTTTTTT
UUUUUUUUUUUUUUUUUUUU
VVVVVVVVVVVVVVVVVVVV
WWWWWWWWWWWWWWWWWWWW
XXXXXXXXXXXXXXXXXXXX
YYYYYYYYYYYYYYYYYYYY
ZZZZZZZZZZZZZZZZZZZZ
AAAAAAAAAAAAAAAAAAAA
BBBBBBBBBBBBBBBBBBBB
CCCCCCCCCCCCCCCCCCCC
DDDDDDDDDDDDDDDDDDDD
EEEEEEEEEEEEEEEEEEEE
FFFFFFFFFFFFFFFFFFFF
GGGGGGGGGGGGGGGGGGGG
HHHHHHHHHHHHHHHHHHHH
IIIIIIIIIIIIIIIIIIII
JJJJJJJJJJJJJJJJJJJJ
KKKKKKKKKKKKKKKKKKKK
LLLLLLLLLLLLLLLLLLLL
MMMMMMMMMMMMMMMMMMMM
NNNNNNNNNNNNNNNNNNNN
OOOOOOOOOOOOOOOOOOOO
PPPPPPPPPPPPPPPPPPPP
QQQQQQQQQQQQQQQQQQQQ
RRRRRRRRRRRRRRRRRRRR
SSSSSSSSSSSSSSSSSSSS
TTTTTTTTTTTTTTTTTTTT
UUUUUUUUUUUUUUUUUUUU
VVVVVVVVVVVVVVVVVVVV
WWWWWWWWWWWWWWWWWWWW
XXXXXXXXXXXXXXXXXXXX
YYYYYYYYYYYYYYYYYYYY
ZZZZZZZZZZZZZZZZZZZZ
EOF)";
int main(){
    if (!std::filesystem::exists("input.txt")){
        std::ofstream input_file("input.txt");
        input_file << input;
    }
    std::string mode;
    std::cout << "Please input mode (GBN, SR, TCP):";
    std::cin >> mode;
    if(mode != "GBN" && mode != "SR" && mode != "TCP"){
        std::cout << "error!" << std::endl;
        return 0;
    }
    bool success = true;
    for (int i = 0; i < 10; ++i) {
#ifdef WIN32
        system((".\\rdt_lab.exe input.txt output.txt " + mode).c_str());
#else
        system(("./rdt_lab input.txt output.txt " + mode).c_str());
#endif

        std::ifstream output_stream("output.txt");
        std::istringstream input_stream(input);
        std::string iline, oline;
        int line_no = 0;
        bool single_run_success = true;
        do {
            ++line_no;
            std::getline(output_stream, oline);
            std::getline(input_stream, iline);
            if(oline != iline){

                int w = std::cout.width();
                if(single_run_success){
                    std::cout << "error occurred in runtime no." << i << std::endl;
                    std::cout <<  "line  " << std::setw(22) << "input" << "output" << std::endl;
                    single_run_success = false;
                }

                std::cout << std::setw(4) << i << std::setw(22) << iline << oline << std::setw(w) << std::endl;
            }

        }while (input_stream);
        if(!single_run_success) {
            success = false;
            std::cout << "Error occurred in open file no." << i + 1 << std::endl;
        }
    }

    if(success)
        std::cout << "finished successfully" << std::endl;
    std::cout << "last output log from program:" << std::endl;
    std::ifstream rout("out.txt");
    std::string line ;
    while(std::getline(rout,line))
        std::cout << line << std::endl;

}