#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cctype>
#include <bitset>

std::unordered_map<std::string, int> symbolTable = {
    {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3},
    {"R4", 4}, {"R5", 5}, {"R6", 6}, {"R7", 7},
    {"R8", 8}, {"R9", 9}, {"R10", 10}, {"R11", 11},
    {"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15},
    {"SCREEN", 16384}, {"KBD", 24576}, {"SP", 0},
    {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4}
};

int nextAvailableAddress = 16; // Next available address for user-defined symbols

std::unordered_map<std::string, std::string> compTableAZero = {
    //a=0
    {"0", "101010"}, {"1", "111111"}, {"-1", "111010"},
    {"D", "001100"}, {"A", "110000"}, {"!D", "001101"},
    {"!A", "110001"}, {"-D", "001111"}, {"-A", "110011"},
    {"D+1", "011111"}, {"A+1", "110111"}, {"D-1", "001110"},
    {"A-1", "110010"}, {"D+A", "000010"}, {"D-A", "010011"},
    {"A-D", "000111"}, {"D&A", "000000"}, {"D|A", "010101"},
};

std::unordered_map<std::string, std::string> compTableAOne = {
    //a=1
    {"M", "110000"}, {"!M", "110001"}, {"-M", "110011"},
    {"M+1", "110111"}, {"M-1", "110010"}, {"D+M", "000010"},
    {"D-M", "010011"}, {"M-D", "000111"}, {"D&M", "000000"},
    {"D|M", "010101"}
};

std::unordered_map<std::string, std::string> destTable = {
    {"", "000"}, {"M", "001"}, {"D", "010"}, {"MD", "011"},
    {"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}
};

std::unordered_map<std::string, std::string> jumpTable = {
    {"", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
};

std::string solveAInstruction(std::string &line) {
    std::string symbol = line.substr(1);
    if(isdigit(symbol[0])) {
        return std::bitset<16>(std::stoi(symbol)).to_string();
    } else {
        // If the symbol is not a number, look it up in the symbol table
        if(symbolTable.find(symbol) == symbolTable.end()) {
            // If the symbol is not in the table, add it with the next available address
            symbolTable[symbol] = nextAvailableAddress++;
        }
        return std::bitset<16>(symbolTable[symbol]).to_string();
    }
}

std::string solveCInstruction(std::string &line) {
    std::string dest = "", comp = "", jump = "";
    size_t equalPos = line.find('=');
    size_t semicolonPos = line.find(';');

    if(equalPos != std::string::npos) {
        dest = line.substr(0, equalPos);
        line = line.substr(equalPos + 1); // Remove dest part
    }

    if(semicolonPos != std::string::npos) {
        comp = line.substr(0, semicolonPos);
        jump = line.substr(semicolonPos + 1); // Remove jump part
    } else {
        comp = line;
    }

    // Determine the a bit based on the comp instruction
    std::string aBit = "0";
    if(compTableAOne.find(comp) != compTableAOne.end()) {
        aBit = "1";
    }

    // Get the binary representations
    std::string compBits = (aBit == "0") ? compTableAZero[comp] : compTableAOne[comp];
    std::string destBits = destTable[dest];
    std::string jumpBits = jumpTable[jump];

    return "111" + aBit + compBits + destBits + jumpBits;
}

void firstPass(std::string &line, int &currLine) {

    if(line.substr(0, 2) == "//") {
        return; // Skip comment lines
    }

    if(line[0] == '(') {
        std::string symbol = line.substr(1, line.length() - 2);
        if(symbolTable.find(symbol) == symbolTable.end()) {
            // If the symbol is not in the table, add it with the next available address
            symbolTable[symbol] = currLine;
        }
    } 
    else currLine++; // Increment the current line number for each line processed
}

// Function to find the type of instruction
std::string secondPass(std::string &line) {
    if(line.substr(0, 2) == "//" || line[0] == '(') {
        return ""; // Skip comment and label lines
    }

    else if(line[0] == '@') {
        return solveAInstruction(line); // A-instruction
    }

    else {
        return solveCInstruction(line); // C-instruction 
    }
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.asm> <output.hack>" << std::endl;
        return 1;
    }
    
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    
    std::ifstream input(inputPath);
    std::ofstream output(outputPath);
    
    if(!input) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    if (!output.is_open()) {
        std::cerr << "Failed to create file!" << std::endl;
        return 1;
    }

    std::string line;
    int currLine = 0;
    // first pass
    while (std::getline(input, line)) {
        if(line.empty()) continue; // skip empty lines

        line.erase(0, line.find_first_not_of(' ')); // Remove leading spaces
        line.erase(line.find_last_not_of(' ') + 1); // Remove trailing spaces

        firstPass(line, currLine);
    }

    input.clear(); // Clear EOF flag
    input.seekg(0); // Reset the stream position to the beginning

    //second pass
    while (std::getline(input, line)) {
        if(line.empty()) continue; // skip empty lines

        line.erase(0, line.find_first_not_of(' ')); // Remove leading spaces
        line.erase(line.find_last_not_of(' ') + 1); // Remove trailing spaces
        
        // Write the binary instruction to the output file
        std::string binaryInstruction = secondPass(line);
        if(!binaryInstruction.empty()) {
            std::bitset<16> binaryInstructionBitset(binaryInstruction);
            output << binaryInstructionBitset;
            if(!input.eof()) {
                output << std::endl;
            }
        }
    }
    
    return 0;
}