#include "AutoCompleteEngine.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "AutoComplete Engine - Console Version\n";
    std::cout << "=====================================\n\n";
    
    // Initialize the engine
    AutoCompleteEngine engine;
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize AutoComplete Engine\n";
        return 1;
    }
    
    std::cout << "Engine initialized with " << engine.getDictionarySize() << " words\n";
    std::cout << "Type a prefix to get suggestions (or 'quit' to exit):\n\n";
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        // Check if user wants to add a custom word
        if (input.substr(0, 4) == "add ") {
            std::string word = input.substr(4);
            if (engine.addCustomWord(word)) {
                std::cout << "Added '" << word << "' to dictionary\n";
            } else {
                std::cout << "Failed to add '" << word << "' (invalid or duplicate)\n";
            }
            continue;
        }
        
        // Get suggestions
        auto suggestions = engine.getSuggestions(input);
        
        if (suggestions.empty()) {
            std::cout << "No suggestions found for '" << input << "'\n";
        } else {
            std::cout << "Suggestions for '" << input << "':\n";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << suggestions[i] << "\n";
            }
            
            // Allow user to select a suggestion
            std::cout << "Select a suggestion (1-" << suggestions.size() << ") or press Enter to continue: ";
            std::string selection;
            std::getline(std::cin, selection);
            
            if (!selection.empty()) {
                try {
                    int index = std::stoi(selection) - 1;
                    if (index >= 0 && index < static_cast<int>(suggestions.size())) {
                        engine.selectSuggestion(suggestions[index]);
                        std::cout << "Selected: " << suggestions[index] << "\n";
                    }
                } catch (const std::exception&) {
                    // Invalid selection, ignore
                }
            }
        }
        std::cout << "\n";
    }
    
    // Save dictionary before exit
    std::cout << "Saving dictionary and frequency data...\n";
    if (engine.saveDictionary()) {
        std::cout << "Dictionary saved successfully\n";
    } else {
        std::cout << "Warning: Could not save dictionary\n";
    }
    
    std::cout << "Goodbye!\n";
    return 0;
}