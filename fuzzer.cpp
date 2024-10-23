#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>

int randomNumber(int max) {
    return rand() % max;
}

std::string randomOperator() {
    const std::string operators[] = {"+", "-", "*"};
    int index = rand() % 3;
    return operators[index];
}

std::string generateExpression(int depth) {
    if (depth <= 0) {
        return std::to_string(randomNumber(5000));
    }
    int random = randomNumber(100);
    if (random <= 75) {
        std::string op = randomOperator();
        std::string left_operand = generateExpression(depth - 1);
        std::string right_operand = generateExpression(depth - 1);

        return "(" + op + " " + left_operand + " " + right_operand + ")";
    } else {
        std::string op = "-";
        std::string child = generateExpression(depth - 1);

        return "(" + op + " " + child + ")";
    }
}

std::string generateStatement(int max_depth) {
    return "(simplify " + generateExpression(max_depth) + ")";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number-of-statements> <output-file-name>" << std::endl;
        return 1;
    }
    int statements = std::stoi(argv[1]);

    std::ofstream outfile(argv[2]);

    if (!outfile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    srand(time(0));

    for (int i = 0; i < statements; ++i) {
        // int max_depth = randomNumber(3);
        int max_depth = 1; // Hard coded for exercise
        std::string line = generateStatement(max_depth);
        outfile << line << std::endl;
    }

    outfile.close();
    return 0;
}
