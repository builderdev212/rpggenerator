#include <iostream>
#include "story_funcs.h"

int numInput(int min, int max, std::string prompt) {
    bool isFirst = true;
    int userInput;

    do {
        if (!isFirst) std::cout << "Error: Please input a number between " << min << " and " << max << ".\n";

        isFirst = false;

        std::cout << prompt;
        std::cin >> userInput;
    } while ((userInput < min) || (userInput > max));

    return userInput;
};
