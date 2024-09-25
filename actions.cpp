#include "actions.h"
#include <iostream>

void open_new_file(std::string &curr_file) {
    std::cout << "Open new file" << std::endl;
    curr_file.assign("new file");
}
void save_file() { std::cout << "Save file" << std::endl; }
void compile_file() { std::cout << "Compile" << std::endl; }
void check_file() { std::cout << "Check" << std::endl; }
void format_file() { std::cout << "Format" << std::endl; }
