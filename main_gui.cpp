#include "src/gui/GUIInterface.h"
#include <iostream>

int main() {
    std::cout << "AutoComplete Engine - GUI Version\n";
    std::cout << "==================================\n";
    
    try {
        GUIInterface gui;
        gui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Make sure SFML is properly installed and configured." << std::endl;
        return 1;
    }
    
    return 0;
}