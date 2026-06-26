#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <memory>
#include <chrono>
#include <fstream>
#include <algorithm>

/**
 * TrieNode structure for efficient word storage and retrieval
 */
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    std::string word;
    int frequency;
    long long lastUsed;
    
    TrieNode() : isEndOfWord(false), frequency(0), lastUsed(0) {}
    
    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

/**
 * Word frequency data structure
 */
struct WordFrequency {
    std::string word;
    int frequency;
    long long lastUsed;
    
    WordFrequency() : frequency(0), lastUsed(0) {}
    WordFrequency(const std::string& w, int f, long long lu) 
        : word(w), frequency(f), lastUsed(lu) {}
    
    bool operator<(const WordFrequency& other) const {
        if (frequency != other.frequency) {
            return frequency > other.frequency;
        }
        return lastUsed > other.lastUsed;
    }
};

/**
 * Suggestion candidate with scoring
 */
struct SuggestionCandidate {
    std::string word;
    double score;
    
    SuggestionCandidate(const std::string& w, double s) : word(w), score(s) {}
    
    bool operator<(const SuggestionCandidate& other) const {
        return score < other.score;  // For max-heap usage
    }
};

/**
 * Trie data structure for efficient word storage and prefix-based searching
 */
class Trie {
private:
    TrieNode* root;

public:
    Trie();
    ~Trie();
    
    void insert(const std::string& word);
    bool contains(const std::string& word) const;
    std::vector<TrieNode*> search(const std::string& prefix) const;
    void clear();
    size_t size() const;
    
private:
    void collectWords(TrieNode* node, std::vector<TrieNode*>& results) const;
    size_t countWords(TrieNode* node) const;
};

/**
 * Tracks word usage frequency and recency
 */
class FrequencyTracker {
private:
    std::unordered_map<std::string, WordFrequency> frequencies;

public:
    FrequencyTracker() = default;
    ~FrequencyTracker() = default;
    
    void updateFrequency(const std::string& word);
    int getFrequency(const std::string& word) const;
    long long getLastUsed(const std::string& word) const;
    void initializeWord(const std::string& word);
    bool loadFrequencies(const std::string& filename);
    bool saveFrequencies(const std::string& filename) const;
    void clear();
    size_t size() const;
    
private:
    long long getCurrentTimestamp() const;
};

/**
 * Ranks suggestions based on frequency, recency, and alphabetical order
 */
class SuggestionRanker {
public:
    SuggestionRanker() = default;
    ~SuggestionRanker() = default;
    
    std::vector<std::string> rankSuggestions(const std::vector<std::string>& candidates, 
                                           const std::vector<int>& frequencies,
                                           const std::vector<long long>& lastUsedTimes);
    std::vector<std::string> getTopK(const std::vector<std::string>& suggestions, int k);
    double calculateScore(int frequency, long long lastUsed) const;
    
private:
    static constexpr int DEFAULT_TOP_K = 5;
    static constexpr double RECENCY_WEIGHT = 0.3;
    static constexpr double FREQUENCY_WEIGHT = 0.7;
};

/**
 * Manages dictionary persistence and loading operations
 */
class DictionaryManager {
private:
    std::shared_ptr<Trie> trie;

public:
    explicit DictionaryManager(std::shared_ptr<Trie> triePtr);
    ~DictionaryManager() = default;
    
    bool loadDictionary(const std::string& filename);
    bool saveDictionary(const std::string& filename) const;
    void initializeDefault();
    bool addWord(const std::string& word);
    bool isValidWord(const std::string& word) const;
    size_t getDictionarySize() const;
    void clearDictionary();
    
private:
    bool isValidCharacter(char c) const;
    void loadDefaultWords();
    void getAllWords(TrieNode* node, std::vector<std::string>& words) const;
};

/**
 * Core AutoComplete Engine that coordinates all components
 */
class AutoCompleteEngine {
private:
    std::shared_ptr<Trie> trie;
    std::unique_ptr<SuggestionRanker> ranker;
    std::unique_ptr<FrequencyTracker> frequencyTracker;
    std::unique_ptr<DictionaryManager> dictionaryManager;

public:
    AutoCompleteEngine();
    ~AutoCompleteEngine() = default;
    
    // Core functionality
    std::vector<std::string> getSuggestions(const std::string& prefix);
    void selectSuggestion(const std::string& word);
    bool addCustomWord(const std::string& word);
    
    // Initialization and persistence
    bool initialize(const std::string& dictionaryFile = "");
    bool saveDictionary(const std::string& filename = "");
    
    // Utility methods
    size_t getDictionarySize() const;
    void clearDictionary();
};