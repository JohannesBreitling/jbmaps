
#include <string>
#include <iostream>
#include <unordered_map>

#include "string_util.hpp"

#pragma once

struct Command {
    std::string keyword;
    int opcode;
};



struct CommandResult {
    CommandResult() : opcode(-1), parameters() {}
    CommandResult(const int opcode_, const std::vector<std::string> &parameters_) : opcode(opcode_), parameters(parameters_) {}

    int opcode;
    std::vector<std::string> parameters;
};

class CommandLineParser {

public:
    CommandLineParser(const std::vector<Command> &commands, const std::string &prefix_) : prefix(prefix_) {
        for (const auto &command : commands) {
            commandToOpcode[command.keyword] = command.opcode;
        }
    }

    bool isRunning() {
        return running;
    }

    CommandResult nextCommand() {
        std::string nextLine;
        std::cout << prefix << std::flush;
        std::getline(std::cin, nextLine);

        if (nextLine == "exit") {
            std::cout << "Exiting now. Have a nice day!\n";
            running = false;
            return {};
        }
        
        const auto parts = split_string(nextLine, " ");
        if (!commandToOpcode.count(parts[0])) {
            std::cout << "The command " << parts[0] << " is not known.\n";
            return {};
        }
        
        const auto result = std::vector<std::string>(parts.begin() + 1, parts.end());
        return CommandResult(commandToOpcode[parts[0]], result);
    }

    std::string nextLine() {
        std::string nextLine;
        std::cout << "Enter the next command: " << std::flush;
        std::getline(std::cin, nextLine);
        
        if (nextLine == "exit")
            running = false;

        return nextLine;
    }

private:
    bool running = true;
    std::string prefix;
    std::unordered_map<std::string, int> commandToOpcode;

};