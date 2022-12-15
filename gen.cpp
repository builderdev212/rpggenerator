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

// Constants for file operations.
const std::string UNABLE_TO_OPEN = "Error: unable to open given file.";

// Functions for file operations.
std::fstream openFile(const std::string name);
void getFilename(std::string &, std::fstream &);
void readFile(const std::string, std::fstream &, std::vector<StorySector> &);

// Regular Expression formulas.
const std::regex PARENT_SEARCH ("^\\{[^\\{\\}]+\\}-");
const std::regex CHILD_SEARCH ("^\\[[^\\{\\}]+\\]-");
const std::regex ENDING_SEARCH ("^<[^<>]+>-");
const std::regex COMMENT_SEARCH ("^//");

// Constants for file operations.
const int NO_READ = -1;
const int PARENT_READ = 0;
const int CHILD_READ = 1;

// Story structure functions.
std::ostream& operator << (std::ostream&, StorySector&);

// Story vector functions.
void dispVector(std::vector<StorySector>);

int main() {
    // Get the name of the story file.
    std::string filename;

    // Variable for file.
    std::fstream storyFile;

    // Setup the filename and file.
    getFilename(filename, storyFile);

    // Vector to store story paths.
    std::vector<StorySector> paths;

    // Store the data into the vector.
    readFile(filename, storyFile, paths);

    // Display the vector.
    dispVector(paths);
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

            // Attempt to open the file.
            file = openFile(filename);
            // This next section will end the loop if no exception is thrown.
            hasRead = true;

        // If the file was not readable, tell the user and have them enter another filename.
        } catch (std::string UNABLE_TO_OPEN) {
            std::cout << UNABLE_TO_OPEN << std::endl;
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
    int currentFind = NO_READ;

    // Loop through the lines in the file.
    while (getline(file, currentLine)) {
        if (!std::regex_search(currentLine, m, COMMENT_SEARCH)) {
            // See if the current line is a parent sector.
            if (std::regex_search(currentLine, m, PARENT_SEARCH) || std::regex_search(currentLine, m, ENDING_SEARCH)) {
                currentFind = PARENT_READ;

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
                currentFind = CHILD_READ;
                // If a value exists, push it into the vector to store children.
                currentSector.children.push_back(m.str(0).substr(1, m.str(0).size() - 3));
                currentSector.childrenText.push_back(currentLine.erase(0, 3+currentSector.children.back().length()));
            } else if ((currentLine != "") && (currentFind != NO_READ)) {
                if (currentFind == PARENT_READ) {
                    currentSector.parentText = currentSector.parentText + "\n" + currentLine;
                }
            }
        }
    }


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

// Story vector functions.
void dispVector(std::vector<StorySector> paths) {
    for (auto it = paths.begin(); it < paths.end(); it++) {
        std::cout << *it;
    }
}
