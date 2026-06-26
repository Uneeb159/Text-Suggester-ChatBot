#include "AutoCompleteEngine.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <stdexcept>

// Trie Implementation
Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root;
}

void Trie::insert(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }
    current->isEndOfWord = true;
    current->word = word;
}

bool Trie::contains(const std::string& word) const {
    TrieNode* current = root;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children[c];
    }
    return current->isEndOfWord;
}

std::vector<TrieNode*> Trie::search(const std::string& prefix) const {
    std::vector<TrieNode*> results;
    TrieNode* current = root;
    
    // Navigate to prefix end
    for (char c : prefix) {
        if (current->children.find(c) == current->children.end()) {
            return results; // Empty results if prefix not found
        }
        current = current->children[c];
    }
    
    // Collect all words with this prefix
    collectWords(current, results);
    return results;
}

void Trie::collectWords(TrieNode* node, std::vector<TrieNode*>& results) const {
    if (node->isEndOfWord) {
        results.push_back(node);
    }
    
    for (auto& pair : node->children) {
        collectWords(pair.second, results);
    }
}

void Trie::clear() {
    delete root;
    root = new TrieNode();
}

size_t Trie::size() const {
    return countWords(root);
}

size_t Trie::countWords(TrieNode* node) const {
    size_t count = 0;
    if (node->isEndOfWord) {
        count = 1;
    }
    
    for (auto& pair : node->children) {
        count += countWords(pair.second);
    }
    return count;
}

// FrequencyTracker Implementation
void FrequencyTracker::updateFrequency(const std::string& word) {
    frequencies[word].frequency++;
    frequencies[word].lastUsed = getCurrentTimestamp();
    frequencies[word].word = word;
}

int FrequencyTracker::getFrequency(const std::string& word) const {
    auto it = frequencies.find(word);
    return (it != frequencies.end()) ? it->second.frequency : 0;
}

long long FrequencyTracker::getLastUsed(const std::string& word) const {
    auto it = frequencies.find(word);
    return (it != frequencies.end()) ? it->second.lastUsed : 0;
}

void FrequencyTracker::initializeWord(const std::string& word) {
    if (frequencies.find(word) == frequencies.end()) {
        frequencies[word] = WordFrequency(word, 1, getCurrentTimestamp());
    }
}

bool FrequencyTracker::loadFrequencies(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    frequencies.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        int freq;
        long long lastUsed;
        
        if (iss >> word >> freq >> lastUsed) {
            frequencies[word] = WordFrequency(word, freq, lastUsed);
        }
    }
    
    file.close();
    return true;
}

bool FrequencyTracker::saveFrequencies(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : frequencies) {
        const WordFrequency& wf = pair.second;
        file << wf.word << " " << wf.frequency << " " << wf.lastUsed << "\n";
    }
    
    file.close();
    return true;
}

void FrequencyTracker::clear() {
    frequencies.clear();
}

size_t FrequencyTracker::size() const {
    return frequencies.size();
}

long long FrequencyTracker::getCurrentTimestamp() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

// SuggestionRanker Implementation
std::vector<std::string> SuggestionRanker::rankSuggestions(
    const std::vector<std::string>& candidates, 
    const std::vector<int>& frequencies,
    const std::vector<long long>& lastUsedTimes) {
    
    std::vector<SuggestionCandidate> scoredCandidates;
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        double score = calculateScore(frequencies[i], lastUsedTimes[i]);
        scoredCandidates.emplace_back(candidates[i], score);
    }
    
    // Sort by score (highest first), then alphabetically for ties
    std::sort(scoredCandidates.begin(), scoredCandidates.end(), 
        [](const SuggestionCandidate& a, const SuggestionCandidate& b) {
            if (a.score != b.score) {
                return a.score > b.score;
            }
            return a.word < b.word; // Alphabetical tie-breaking
        });
    
    std::vector<std::string> result;
    for (const auto& candidate : scoredCandidates) {
        result.push_back(candidate.word);
    }
    
    return result;
}

std::vector<std::string> SuggestionRanker::getTopK(const std::vector<std::string>& suggestions, int k) {
    std::vector<std::string> result;
    int count = std::min(k, static_cast<int>(suggestions.size()));
    
    for (int i = 0; i < count; ++i) {
        result.push_back(suggestions[i]);
    }
    
    return result;
}

double SuggestionRanker::calculateScore(int frequency, long long lastUsed) const {
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Normalize recency (more recent = higher score)
    double recencyScore = (lastUsed > 0) ? 
        1.0 / (1.0 + (currentTime - lastUsed) / 1000.0) : 0.0;
    
    // Combine frequency and recency
    return FREQUENCY_WEIGHT * frequency + RECENCY_WEIGHT * recencyScore;
}

// DictionaryManager Implementation
DictionaryManager::DictionaryManager(std::shared_ptr<Trie> triePtr) : trie(triePtr) {}

bool DictionaryManager::loadDictionary(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string word;
    while (std::getline(file, word)) {
        if (isValidWord(word)) {
            trie->insert(word);
        }
    }
    
    file.close();
    return true;
}

bool DictionaryManager::saveDictionary(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }
        
        // Get all words from trie and save them
        std::vector<std::string> allWords;
        getAllWords(trie->root, allWords);
        
        for (const auto& word : allWords) {
            file << word << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving dictionary: " << e.what() << std::endl;
        return false;
    }
}

void DictionaryManager::initializeDefault() {
    loadDefaultWords();
}

bool DictionaryManager::addWord(const std::string& word) {
    if (!isValidWord(word)) {
        return false;
    }
    
    if (trie->contains(word)) {
        return false; // Duplicate prevention
    }
    
    trie->insert(word);
    return true;
}

bool DictionaryManager::isValidWord(const std::string& word) const {
    if (word.empty()) {
        return false;
    }
    
    for (char c : word) {
        if (!isValidCharacter(c)) {
            return false;
        }
    }
    
    return true;
}

size_t DictionaryManager::getDictionarySize() const {
    return trie->size();
}

void DictionaryManager::clearDictionary() {
    trie->clear();
}

bool DictionaryManager::isValidCharacter(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '\'' || c == '-';
}

void DictionaryManager::loadDefaultWords() {
    // Load some common English words as default
    std::vector<std::string> defaultWords = {
        "the", "and", "for", "are", "but", "not", "you", "all", "can", "had",
        "her", "was", "one", "our", "out", "day", "get", "has", "him", "his",
        "how", "man", "new", "now", "old", "see", "two", "way", "who", "boy",
        "did", "its", "let", "put", "say", "she", "too", "use", "hello", "world",
        "computer", "programming", "algorithm", "data", "structure", "software",
        "development", "application", "interface", "system"
    };
    
    for (const std::string& word : defaultWords) {
        trie->insert(word);
    }
}

void DictionaryManager::getAllWords(TrieNode* node, std::vector<std::string>& words) const {
    if (node->isEndOfWord) {
        words.push_back(node->word);
    }
    
    for (auto& pair : node->children) {
        getAllWords(pair.second, words);
    }
}

// AutoCompleteEngine Implementation
AutoCompleteEngine::AutoCompleteEngine() {
    trie = std::make_shared<Trie>();
    ranker = std::make_unique<SuggestionRanker>();
    frequencyTracker = std::make_unique<FrequencyTracker>();
    dictionaryManager = std::make_unique<DictionaryManager>(trie);
}

std::vector<std::string> AutoCompleteEngine::getSuggestions(const std::string& prefix) {
    if (prefix.empty()) {
        return {};
    }
    
    // Get matching nodes from trie
    std::vector<TrieNode*> matchingNodes = trie->search(prefix);
    
    if (matchingNodes.empty()) {
        return {};
    }
    
    // Extract words and their metadata
    std::vector<std::string> candidates;
    std::vector<int> frequencies;
    std::vector<long long> lastUsedTimes;
    
    for (TrieNode* node : matchingNodes) {
        candidates.push_back(node->word);
        frequencies.push_back(frequencyTracker->getFrequency(node->word));
        lastUsedTimes.push_back(frequencyTracker->getLastUsed(node->word));
    }
    
    // Rank suggestions
    std::vector<std::string> rankedSuggestions = ranker->rankSuggestions(
        candidates, frequencies, lastUsedTimes);
    
    // Return top 5
    return ranker->getTopK(rankedSuggestions, 5);
}

void AutoCompleteEngine::selectSuggestion(const std::string& word) {
    frequencyTracker->updateFrequency(word);
}

bool AutoCompleteEngine::addCustomWord(const std::string& word) {
    bool added = dictionaryManager->addWord(word);
    if (added) {
        frequencyTracker->initializeWord(word);
    }
    return added;
}

bool AutoCompleteEngine::initialize(const std::string& dictionaryFile) {
    if (!dictionaryFile.empty()) {
        if (!dictionaryManager->loadDictionary(dictionaryFile)) {
            dictionaryManager->initializeDefault();
        }
    } else {
        dictionaryManager->initializeDefault();
    }
    
    // Try to load frequency data
    frequencyTracker->loadFrequencies("frequencies.dat");
    
    return true;
}

bool AutoCompleteEngine::saveDictionary(const std::string& filename) {
    std::string dictFile = filename.empty() ? "dictionary.txt" : filename;
    bool dictSaved = dictionaryManager->saveDictionary(dictFile);
    bool freqSaved = frequencyTracker->saveFrequencies("frequencies.dat");
    
    return dictSaved && freqSaved;
}

size_t AutoCompleteEngine::getDictionarySize() const {
    return dictionaryManager->getDictionarySize();
}

void AutoCompleteEngine::clearDictionary() {
    dictionaryManager->clearDictionary();
    frequencyTracker->clear();
}