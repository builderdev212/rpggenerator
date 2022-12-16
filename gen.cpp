#include <iostream>
#include <iomanip>
#include <regex>
#include <fstream>
#include <vector>
#include <string>

// Structure to hold options.
struct StorySector {
    std::string parent;
    std::string parentText;
    std::vector<std::string> children;
    std::vector<std::string> childrenText;
    bool isIntro = false;
    bool isEnding = false;
    bool isStart = false;
};

// Null story structure.
const StorySector NULL_SECTOR;

// Story structure functions.
std::ostream& operator << (std::ostream&, StorySector&);

// Constants for file operations.
const std::string UNABLE_TO_OPEN = "Error: unable to open given file.";
const std::string NOT_TXT = "Error: must enter a .txt file.";
const std::string QUIT_CHAR = "Q";
const std::string QUIT_CHAR_LOWER = "q";

// Functions for file operations.
std::fstream openFile(const std::string);
std::string getFilename(std::fstream &);
void readFile(const std::string, std::fstream &, std::vector<StorySector> &);

// Regex formulas.
const std::regex PARENT_SEARCH ("^\\{[^\\{\\}]+\\}-");
const std::regex CHILD_SEARCH ("^\\[[^\\{\\}]+\\]-");
const std::regex ENDING_SEARCH ("^<[^<>]+>-");
const std::regex COMMENT_SEARCH ("^//");
const std::regex TXT_SEARCH ("\\.txt$");

// Code generation functions.
void genFunc(const StorySector, std::fstream &);
void genCPP(const std::string, const std::vector<StorySector>);
void genH(const std::string, const std::vector<StorySector>);
void genMain(const std::string, const std::vector<StorySector>);

// Menu options.
const int CREATE_STORY = 1;
const int PLAY_STORY = 2;
const int REMOVE_STORY = 3;
const int QUIT = 4;

// String vector operations.
bool isInVector(const std::vector<std::string>, const std::string);
std::vector<std::string> addExistingStories();

int main() {
    // Variable used to keep the menu loop going.
    bool exit = false;

    // Variable to store the available stories.
    std::vector<std::string> stories = addExistingStories();

    while (!exit) {
        // Variables for the menu option.
        int userChoice;
        bool first = true;

        do {
            // If user entere an incorrect value the first time.
            if (!first) {
                std::cout << "Invalid Choice! Please try again." << std::endl;
            // Display the options the first time.
            } else {
                std::cout << "RPG Generator" << std::endl;
                std::cout << std::setw(4) << "" << CREATE_STORY << ". Add a story." << std::endl;
                std::cout << std::setw(4) << "" << PLAY_STORY << ". Play a story." << std::endl;
                std::cout << std::setw(4) << "" << REMOVE_STORY << ". Remove a story." << std::endl;
                std::cout << std::setw(4) << "" << QUIT << ". Quit." << std::endl;
            }
            first = false;

            std::cout << "Enter menu choice("+std::to_string(CREATE_STORY)+"-"+std::to_string(QUIT)+"): ";
            std::cin >> userChoice;
        } while ((userChoice < CREATE_STORY) || (userChoice > QUIT));

        std::cout << std::endl;

        switch (userChoice) {
            case CREATE_STORY: {
                // Variables for the file.
                std::string filename;
                std::string baseName;
                std::fstream storyFile;

                std::cin.ignore();
                // Setup the filename and file.
                filename = getFilename(storyFile);
                if (filename == QUIT_CHAR) {
                    std::cout << std::endl;
                    break;
                }
                baseName = filename;
                baseName.erase(baseName.length()-4);
                if (isInVector(stories, baseName)) {
                    std::cout << "Error: This file already has an entry. If you wish to recompile the story you must first delete it.\n" << std::endl;
                    break;
                }

                // Add the new story to the vector.
                stories.push_back(baseName);

                // Vector to store story paths.
                std::vector<StorySector> paths;

                // Store the data into the vector.
                readFile(filename, storyFile, paths);

                // Generate the code.
                genCPP(filename, paths);
                genH(filename, paths);
                genMain(filename, paths);

#ifdef __linux__
                // Compile and run the code.
                std::system(("g++ story_funcs.h story_funcs.cpp "+baseName+".h "+baseName+".cpp "+baseName+"Main.cpp -o "+baseName).c_str());
                // Delete the created files after being used.
                std::system(("rm "+baseName+".h "+baseName+".cpp "+baseName+"Main.cpp").c_str());
#elifdef _WIN32

#endif
                std::cout << std::endl;

                break;
            }
            case PLAY_STORY: {
                if (stories.size() == 0) {
                    std::cout << "Error: You have no stories." << std::endl;
                    break;
                }

                int game;
                first = true;

                do {
                    // If user enter an incorrect value the first time.
                    if (!first) {
                        std::cout << "Invalid Choice! Please try again." << std::endl;
                    // Display the options the first time.
                    } else {
                        std::cout << "Available Stories:" << std::endl;
                        for (int i = 0; i < stories.size(); i++) {
                            std::cout << std::setw(4) << "" << std::to_string(i+1)+". "+stories[i] << std::endl;
                        }
                        std::cout << std::setw(4) << "" << std::to_string(stories.size()+1)+". Go back to main menu." << std::endl;
                    }
                    first = false;

                    std::cout << "Enter menu choice(1-"+std::to_string(stories.size())+"): ";
                    std::cin >> game;
                } while ((game < 1) || (game > stories.size()+1));

                std::cout << std::endl;

                // If it was the last menu option, return to the main menu.
                if (game == stories.size()+1) break;
#ifdef __linux__
                // Compile and run the code.
                std::system(("./"+stories[game-1]).c_str());
#elifdef _WIN32

#endif
                std::cout << std::endl;
                break;
            }
            case REMOVE_STORY:
                if (stories.size() == 0) {
                    std::cout << "Error: You have no stories.\n" << std::endl;
                    break;
                }

                int game;
                first = true;

                do {
                    // If user enter an incorrect value the first time.
                    if (!first) {
                        std::cout << "Invalid Choice! Please try again." << std::endl;
                    // Display the options the first time.
                    } else {
                        std::cout << "Available Stories:" << std::endl;
                        for (int i = 0; i < stories.size(); i++) {
                            std::cout << std::setw(4) << "" << std::to_string(i+1)+". "+stories[i] << std::endl;
                        }
                        std::cout << std::setw(4) << "" << std::to_string(stories.size()+1)+". Go back to main menu." << std::endl;
                    }
                    first = false;

                    std::cout << "Enter menu choice(1-"+std::to_string(stories.size())+"): ";
                    std::cin >> game;
                } while ((game < 1) || (game > stories.size()+1));

                std::cout << std::endl;
                // If it was the last menu option, return to the main menu.
                if (game == stories.size()+1) break;

                std::cout << "Removed "+stories[game-1]+".\n" << std::endl;

                // Remove the chosen story.
                std::system(("rm "+stories[game-1]).c_str());
                stories.erase(stories.begin()+game-1);
                break;

            case QUIT:
                // Breaks the while loop.
                exit = true;
                break;

            default:
                std::cout << "How did you get this to output o.0" << std::endl;
                break;
        }
    }

    // Pause the output and let the computer know the code ran fine.
#ifdef _WIN32
	// This is a windows specific way to pause the output.
	std::system("pause");
#else
	// This is a general way to pause the output that should work on any OS.
    std::cin.ignore();
	std::cout << "Press any key to continue...";
	getchar();
#endif
	return 0;
}

// File operation functions.
std::fstream openFile(const std::string filename) {
    // File to read from
	std::fstream file;

	// Open the file in read only.
	file.open(filename, std::ios::in);

	if (!file) {
		// Throw an exception if the file cannot be found.
		throw UNABLE_TO_OPEN;
	} else {
		// Return the file if it is found.
		return file;
	}
}

std::string getFilename(std::fstream &file) {
    std::string filename;
    bool hasRead = false;

    // Loop until a valid file has been given.
    do {
        try {
            // Get the name of the file.
            std::cout << "Please enter the path to your story file("+QUIT_CHAR+" to exit): ";
            getline(std::cin, filename);

            if ((QUIT_CHAR == filename) || (QUIT_CHAR_LOWER == filename)) return QUIT_CHAR;
            if (!std::regex_search(filename, TXT_SEARCH)) {
                throw NOT_TXT;
            }

            try {
                // Attempt to open the file.
                file = openFile(filename);
                // This next section will end the loop if no exception is thrown.
                hasRead = true;
            } catch (std::string UNABLE_TO_OPEN) {
                std::cout << UNABLE_TO_OPEN << std::endl;
            }
        // If the file was not readable, tell the user and have them enter another filename.
        } catch (std::string NOT_TXT) {
            std::cout << NOT_TXT << std::endl;
        }
    } while (!hasRead);

    // Close the file.
    file.close();

    return filename;
}

void readFile(const std::string filename, std::fstream &file, std::vector<StorySector> &paths) {
    // Try to open the file.
    try {
        file = openFile(filename);
    } catch (std::string UNABLE_TO_OPEN) {
        std::cout << UNABLE_TO_OPEN << std::endl;
        return;
    }

    // Variable to store the current line being read.
    std::string currentLine;
    StorySector currentSector;
    std::smatch m;
    bool currentParent = false;

    // Loop through the lines in the file.
    while (getline(file, currentLine)) {
        if (!std::regex_search(currentLine, m, COMMENT_SEARCH)) {
            // See if the current line is a parent sector.
            if (std::regex_search(currentLine, m, PARENT_SEARCH) || std::regex_search(currentLine, m, ENDING_SEARCH)) {
                currentParent = true;

                // If the current sector has no parent (aka the first one.)
                if (currentSector.parent.empty()) {
                    // Check if this is an ending.
                    if (m.str(0)[0] == '<') {
                        currentSector.isEnding = true;
                    }
                    // Check if this is an intro.
                    if ((m.str(0)[0] == '{') && (m.str(0)[2] == '}') && (m.str(0)[1] == 'I')) {
                        currentSector.isIntro = true;
                    } else {
                        std::cout << "Error: your text file must begin with an intro header." << std::endl;
                    }
                    // Set the parent to the new value and remove the {}-.
                    currentSector.parent = m.str(0).substr(1, m.str(0).size() - 3);
                    // Get the text string.
                    currentSector.parentText = currentLine.erase(0, 3+currentSector.parent.length());
                // If the current sector is filled, push it to the vector.
                } else {
                    // Push the current structure to the vector.
                    paths.push_back(currentSector);
                    // Reset the variable.
                    currentSector = NULL_SECTOR;

                    // Check if this is an ending.
                    if (m.str(0)[0] == '<') {
                        currentSector.isEnding = true;
                    }
                    // Check if this is an intro.
                    if ((m.str(0)[0] == '{') && (m.str(0)[2] == '}')) {
                        if (m.str(0)[1] == 'I') {
                            currentSector.isIntro = true;
                        } else if (m.str(0)[1] == 'S') {
                            currentSector.isStart = true;
                        }
                    }
                    // Set the parent to the new value and remove the {}-.
                    currentSector.parent = m.str(0).substr(1, m.str(0).size() - 3);
                    // Get the text string.
                    currentSector.parentText = currentLine.erase(0, 3+currentSector.parent.length());
                }
            } else if (!currentSector.parent.empty() && std::regex_search(currentLine, m, CHILD_SEARCH) && !currentSector.isEnding && !currentSector.isIntro) {
                currentParent = false;
                // If a value exists, push it into the vector to store children.
                currentSector.children.push_back(m.str(0).substr(1, m.str(0).size() - 3));
                currentSector.childrenText.push_back(currentLine.erase(0, 3+currentSector.children.back().length()));
            } else if ((currentLine != "") && (currentParent)) {
                currentSector.parentText = currentSector.parentText + "\\n" + currentLine;
            }
        }
    }

    // Push the final path into the path vector.
    paths.push_back(currentSector);

    // Close the file.
    file.close();
}

// Story structure functions.
std::ostream& operator << (std::ostream& strm, StorySector& sector) {
    // Ouptut the parent.
    std::string parentName;
    if (sector.isEnding) {
        parentName = "Ending: ";
    } else if (sector.isStart) {
        parentName = "Start: ";
    } else if (sector.isIntro) {
        parentName = "Intro: ";
    } else {
        parentName = "Parent: ";
    }

    strm << parentName << sector.parent << std::endl;
    strm << sector.parentText << std::endl;

    // Loop through the children.
    if (!sector.isEnding && !sector.isIntro) {
        for (int i = 0; i<sector.children.size(); i++) {
            strm << std::setw(7) << std::left << "Child:" << sector.children[i] << std::endl;
            strm << sector.childrenText[i] << std::endl;
        }
    }

    // Return the output stream at the end.
    return strm;
}

// Code generation functions.
void genFunc(const StorySector path, std::fstream &file) {
    // Generate intro function.
    if (path.isIntro) {
        file << "void I() {\n\tstd::cout << \""+path.parentText+"\" << std::endl;\n}\n";
    // Generate ending function.
    } else if (path.isEnding) {
        file << "void " << path.parent << "() {\n\tstd::cout << \""+path.parentText+"\" << std::endl;\n}\n";
    // Generate regular function.
    } else {
        file << "void " << path.parent << "() {\n\tint userInput = numInput(1, "+std::to_string(path.children.size())+", \""+path.parentText;
        for (int i = 1; i <= path.children.size(); i++) {
            file << "\\n\\t"+std::to_string(i)+"- "+path.childrenText[i-1];
        }
        file << "\");\n\n\tswitch (userInput) {\n";
        for (int i = 1; i <= path.children.size(); i++) {
            file << "\t\tcase "+std::to_string(i)+":\n\t\t\t"+path.children[i-1]+"();\n\t\t\tbreak;\n";
        }
        file << "\t\tdefault:\n\t\t\tbreak;\n\t}\n}\n";
    }
}

void genCPP(const std::string filename, const std::vector<StorySector> paths) {
    // Setup the file.
    std::string file = filename;
    std::fstream cppFile;
    cppFile.open(file.erase(file.length()-4)+".cpp", std::ios::out);

    // Setup the includes.
    cppFile << "#include <iostream>\n#include <string>\n#include \"story_funcs.h\"\n#include \""+file+".h\"\n\n";

    // Add all the functions.
    for (auto it = paths.begin(); it < paths.end(); it++) {
        genFunc(*it, cppFile);
    }

    // Close the file.
    cppFile.close();
}

void genH(const std::string filename, const std::vector<StorySector> paths) {
    // Setup the file.
    std::string file = filename;
    std::fstream cppFile;
    cppFile.open(file.erase(file.length()-4)+".h", std::ios::out);

    // Setup the header.
    cppFile << "#ifndef "+file+"\n#define "+file+"\n\n";

    // Add all the functions.
    for (auto it = paths.begin(); it < paths.end(); it++) {
        cppFile << "void "+(it->parent)+"();\n";
    }

    // Setup the end.
    cppFile << "\n#endif\n";

    // Close the file.
    cppFile.close();
}

void genMain(const std::string filename, const std::vector<StorySector> paths) {
    // Setup the file.
    std::string file = filename;
    std::fstream cppFile;
    cppFile.open(file.erase(file.length()-4)+"Main.cpp", std::ios::out);

    // Setup the main file.
    cppFile << "#include <iostream>\n#include \""+file+".h\"\n\nint main() {\n\tI();\n\tstd::cout << std::endl;\n\tS();\n}";

    cppFile.close();
}

// String vector operations.
bool isInVector(const std::vector<std::string> vals, const std::string val) {
    for (auto it = vals.begin(); it < vals.end(); it++) {
        if (*it == val) {
            return true;
        }
    }

    return false;
}

std::vector<std::string> addExistingStories() {
    // Variables to store existing stories.
    std::vector<std::string> stories;

    // File to hold data read.
    std::fstream execs;
    std::string currentExec;

    // Get the executables in the file path of the executable.
    std::system("find . -maxdepth 1 -type f -executable > execs.txt");

    // Open the file and read the data.
    execs.open("execs.txt", std::ios::in);

    while (getline(execs, currentExec)) {
        currentExec.erase(0, 2);

        // If this isn't the executable for this file, add it to the list.
        if (currentExec != "gen") {
            stories.push_back(currentExec);
        }
    }

    execs.close();

    // Delete the file.
    std::system("rm execs.txt");

    return stories;
}
