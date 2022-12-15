#include <iostream>
#include <string>

int numInput(int, int, std::string);
void s();

void e1();
void e2();
void e3();
void e4();

void t1();
void t3();
void t4();
void t6();
void t7();
void t8();


int main() {
    std::cout << "Intro goes here" << std::endl;

    s();
}

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
}

void s() {
    int options = 4;
    int userInput = numInput(1, options, "Starting thing");

    switch (userInput) {
        case 1:
            e1();
            break;

        case 2:
            e2();
            break;

        case 3:
            e3();
            break;

        case 4:
            e4();
            break;

        default:
            break;
    }
}

void e1() {
    int options = 2;
    int userInput = numInput(1, options, "Starting thing");

    switch (userInput) {
        case 1:
            t8();
            break;
        case 2:
            t7();
            break;
        default:
            break;
    }
}
void e2() {
    int options = 2;
    int userInput = numInput(1, options, "Starting thing");

    switch (userInput) {
        case 1:
            t6();
            break;
        case 2:
            s();
            break;
        default:
            break;
    }
}
void e3() {
    int options = 2;
    int userInput = numInput(1, options, "Starting thing");

    switch (userInput) {
        case 1:
            t4();
            break;
        case 2:
            t3();
            break;
        default:
            break;
    }
}
void e4() {
    int options = 2;
    int userInput = numInput(1, options, "Starting thing");

    switch (userInput) {
        case 1:
            e3();
            break;
        case 2:
            t1();
        default:
            break;
    }
}

void t1() {
    std::cout << "end 1" << std::endl;
}
void t3() {
    std::cout << "end 3" << std::endl;
}
void t4() {
    std::cout << "end 4" << std::endl;
}
void t6() {
    std::cout << "end 6" << std::endl;
}
void t7() {
    std::cout << "end 7" << std::endl;
}
void t8() {
    std::cout << "end 8" << std::endl;
}
