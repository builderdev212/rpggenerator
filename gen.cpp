#include <iostream>
#include <iomanip>
#include <cctype>
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

// Functions for file operations.
std::fstream openFile(const std::string);
void getFilename(std::string &, std::fstream &);
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

int main() {
    // Variables for the file.
    std::string filename;
    std::fstream storyFile;

    // Setup the filename and file.
    getFilename(filename, storyFile);

    // Vector to store story paths.
    std::vector<StorySector> paths;

    // Store the data into the vector.
    readFile(filename, storyFile, paths);

    // Generate the code.
    genCPP(filename, paths);
    genH(filename, paths);
    genMain(filename, paths);

    // Create name used for new filename.
    std::string codeFile = filename;
    codeFile.erase(codeFile.length()-4);

    // add a newline between the inital input and this.
    std::cout << std::endl;
#ifdef __linux__
    // Compile and run the code.
    std::system(("g++ story_funcs.h story_funcs.cpp "+codeFile+".h "+codeFile+".cpp "+codeFile+"Main.cpp -o rpg; ./rpg").c_str());
    // Delete the created files after being used.
    std::system(("rm story_funcs.h story_funcs.cpp "+codeFile+".h "+codeFile+".cpp "+codeFile+"Main.cpp rpg").c_str());
#endif

    // Pause the output and let the computer know the code ran fine.
#ifdef _WIN32
	// This is a windows specific way to pause the output.
	std::system("pause");
#else
	// This is a general way to pause the output that should work on any OS.
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

void getFilename(std::string &filename, std::fstream &file) {
    bool hasRead = false;

    // Loop until a valid file has been given.
    do {
        try {
            // Get the name of the file.
            std::cout << "Please enter the path to your story file: ";
            getline(std::cin, filename);
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
}

void genMain(const std::string filename, const std::vector<StorySector> paths) {
    // Setup the file.
    std::string file = filename;
    std::fstream cppFile;
    cppFile.open(file.erase(file.length()-4)+"Main.cpp", std::ios::out);

    // Setup the main file.
    cppFile << "#include <iostream>\n#include \""+file+".h\"\n\nint main() {\n\tI();\n\tstd::cout << std::endl;\n\tS();\n}";
}
