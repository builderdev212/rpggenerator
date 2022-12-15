#include <iostream>
#include "story_funcs.h"

int numInput(int min, int max, std::string prompt) {
    bool isFirst = true;
    int userInput;

    std::cout << prompt << std::endl;

    do {
        if (!isFirst) std::cout << "Error: Please input a number between " << min << " and " << max << ".\n";

        isFirst = false;

        std::cout << "Please enter your choice("+std::to_string(min)+"-"+std::to_string(max)+"): ";
        std::cin >> userInput;
    } while ((userInput < min) || (userInput > max));

    std::cout << std::endl;
    return userInput;
};
