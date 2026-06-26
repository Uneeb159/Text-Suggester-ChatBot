#include "GUIInterface.h"
#include <iostream>

GUIInterface::GUIInterface() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "AutoComplete Engine")
    , selectedSuggestionIndex(-1)
    , inputFocused(true) {
    
    // Initialize engine
    engine = std::make_unique<AutoCompleteEngine>();
    engine->initialize();
    
    // Load font and setup UI
    if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Using default font." << std::endl;
    }
    
    setupUI();
    window.setFramerateLimit(60);
}

void GUIInterface::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            handleInput(event);
        }
        
        render();
    }
}

void GUIInterface::handleInput(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
            
        case sf::Event::TextEntered:
            if (inputFocused && event.text.unicode < 128) {
                handleTextInput(static_cast<char>(event.text.unicode));
            }
            break;
            
        case sf::Event::KeyPressed:
            handleKeyPress(event.key.code);
            break;
            
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
            break;
            
        default:
            break;
    }
}

void GUIInterface::handleTextInput(char character) {
    if (character == '\b') { // Backspace
        if (!currentInput.empty()) {
            currentInput.pop_back();
            updateSuggestions();
        }
    } else if (character >= 32 && character < 127) { // Printable characters
        currentInput += character;
        updateSuggestions();
    }
    
    // Update input text display
    inputText.setString(currentInput + "|"); // Add cursor
}

void GUIInterface::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Up:
            if (selectedSuggestionIndex > 0) {
                selectedSuggestionIndex--;
            } else if (!currentSuggestions.empty()) {
                selectedSuggestionIndex = static_cast<int>(currentSuggestions.size()) - 1;
            }
            break;
            
        case sf::Keyboard::Down:
            if (selectedSuggestionIndex < static_cast<int>(currentSuggestions.size()) - 1) {
                selectedSuggestionIndex++;
            } else if (!currentSuggestions.empty()) {
                selectedSuggestionIndex = 0;
            }
            break;
            
        case sf::Keyboard::Enter:
            if (selectedSuggestionIndex >= 0 && selectedSuggestionIndex < static_cast<int>(currentSuggestions.size())) {
                selectSuggestion(selectedSuggestionIndex);
            }
            break;
            
        case sf::Keyboard::Escape:
            clearInput();
            break;
            
        default:
            break;
    }
}

void GUIInterface::handleMouseClick(int x, int y) {
    // Check if click is on input box
    sf::FloatRect inputBounds = inputBox.getGlobalBounds();
    if (inputBounds.contains(static_cast<float>(x), static_cast<float>(y))) {
        inputFocused = true;
        return;
    }
    
    // Check if click is on any suggestion box
    for (int i = 0; i < static_cast<int>(currentSuggestions.size()) && i < 5; ++i) {
        sf::FloatRect suggestionBounds = suggestionBoxes[i].getGlobalBounds();
        if (suggestionBounds.contains(static_cast<float>(x), static_cast<float>(y))) {
            selectSuggestion(i);
            return;
        }
    }
    
    inputFocused = false;
}

void GUIInterface::updateSuggestions() {
    if (currentInput.empty()) {
        currentSuggestions.clear();
        selectedSuggestionIndex = -1;
        lastProcessedInput.clear();
        
        // Clear suggestion text displays
        for (int i = 0; i < 5; ++i) {
            suggestionTexts[i].setString("");
        }
        return;
    }
    
    // Debouncing: only process if enough time has passed and input has changed
    if (currentInput == lastProcessedInput || 
        debounceTimer.getElapsedTime().asSeconds() < DEBOUNCE_TIME) {
        return;
    }
    
    // Check cache first
    auto cacheIt = suggestionCache.find(currentInput);
    if (cacheIt != suggestionCache.end()) {
        currentSuggestions = cacheIt->second;
    } else {
        // Get suggestions from engine and cache them
        currentSuggestions = engine->getSuggestions(currentInput);
        
        // Cache the result (limit cache size to prevent memory issues)
        if (suggestionCache.size() < 1000) {
            suggestionCache[currentInput] = currentSuggestions;
        }
    }
    
    selectedSuggestionIndex = currentSuggestions.empty() ? -1 : 0;
    lastProcessedInput = currentInput;
    debounceTimer.restart();
    
    // Update suggestion text displays
    for (int i = 0; i < 5; ++i) {
        if (i < static_cast<int>(currentSuggestions.size())) {
            suggestionTexts[i].setString(currentSuggestions[i]);
        } else {
            suggestionTexts[i].setString("");
        }
    }
}

void GUIInterface::render() {
    // Check if we need to update suggestions (for debouncing)
    if (currentInput != lastProcessedInput && 
        debounceTimer.getElapsedTime().asSeconds() >= DEBOUNCE_TIME) {
        updateSuggestions();
    }
    
    window.clear(sf::Color(240, 240, 240)); // Light gray background
    
    // Draw title and instructions
    window.draw(titleText);
    window.draw(instructionText);
    
    // Draw input box
    inputBox.setFillColor(inputFocused ? sf::Color::White : sf::Color(250, 250, 250));
    inputBox.setOutlineColor(inputFocused ? sf::Color::Blue : sf::Color::Black);
    window.draw(inputBox);
    window.draw(inputText);
    
    // Draw suggestion boxes and text
    for (int i = 0; i < static_cast<int>(currentSuggestions.size()) && i < 5; ++i) {
        // Highlight selected suggestion
        if (i == selectedSuggestionIndex) {
            suggestionBoxes[i].setFillColor(sf::Color(200, 220, 255));
            suggestionBoxes[i].setOutlineColor(sf::Color::Blue);
        } else {
            suggestionBoxes[i].setFillColor(sf::Color::White);
            suggestionBoxes[i].setOutlineColor(sf::Color::Black);
        }
        
        window.draw(suggestionBoxes[i]);
        window.draw(suggestionTexts[i]);
    }
    
    window.display();
}

void GUIInterface::setupUI() {
    // Setup title text
    titleText.setFont(font);
    titleText.setString("AutoComplete Engine");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::Black);
    titleText.setPosition(50, 30);
    
    // Setup instruction text
    instructionText.setFont(font);
    instructionText.setString("Type to get suggestions. Use arrow keys to navigate, Enter to select, Esc to clear.");
    instructionText.setCharacterSize(16);
    instructionText.setFillColor(sf::Color(100, 100, 100));
    instructionText.setPosition(50, 80);
    
    // Setup input box
    inputBox.setSize(sf::Vector2f(WINDOW_WIDTH - 100, INPUT_BOX_HEIGHT));
    inputBox.setPosition(50, INPUT_BOX_Y);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);
    
    // Setup input text
    inputText.setFont(font);
    inputText.setCharacterSize(20);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(60, INPUT_BOX_Y + 10);
    inputText.setString("|"); // Initial cursor
    
    // Setup suggestion boxes and texts
    for (int i = 0; i < 5; ++i) {
        float yPos = SUGGESTIONS_START_Y + i * (SUGGESTION_BOX_HEIGHT + 5);
        
        suggestionBoxes[i].setSize(sf::Vector2f(WINDOW_WIDTH - 100, SUGGESTION_BOX_HEIGHT));
        suggestionBoxes[i].setPosition(50, yPos);
        suggestionBoxes[i].setFillColor(sf::Color::White);
        suggestionBoxes[i].setOutlineThickness(1);
        suggestionBoxes[i].setOutlineColor(sf::Color::Black);
        
        suggestionTexts[i].setFont(font);
        suggestionTexts[i].setCharacterSize(18);
        suggestionTexts[i].setFillColor(sf::Color::Black);
        suggestionTexts[i].setPosition(60, yPos + 5);
    }
}

bool GUIInterface::loadFont() {
    // Try to load a system font
    if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return true;
    }
    
    // Try alternative font paths
    if (font.loadFromFile("arial.ttf") || 
        font.loadFromFile("fonts/arial.ttf") ||
        font.loadFromFile("/System/Library/Fonts/Arial.ttf") ||
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        return true;
    }
    
    // Use default font if no external font is found
    return false;
}

void GUIInterface::selectSuggestion(int index) {
    if (index >= 0 && index < static_cast<int>(currentSuggestions.size())) {
        std::string selectedWord = currentSuggestions[index];
        
        // Update engine with selection for learning
        engine->selectSuggestion(selectedWord);
        
        // Replace current input with selected word
        currentInput = selectedWord;
        inputText.setString(currentInput + "|");
        
        // Clear suggestions
        currentSuggestions.clear();
        selectedSuggestionIndex = -1;
        
        // Update display
        for (int i = 0; i < 5; ++i) {
            suggestionTexts[i].setString("");
        }
    }
}

void GUIInterface::clearInput() {
    currentInput.clear();
    currentSuggestions.clear();
    selectedSuggestionIndex = -1;
    inputText.setString("|");
    
    for (int i = 0; i < 5; ++i) {
        suggestionTexts[i].setString("");
    }
}