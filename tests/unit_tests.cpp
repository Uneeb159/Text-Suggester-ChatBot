#include "../AutoCompleteEngine.h"
#include <gtest/gtest.h>

// Test fixture for AutoComplete Engine tests
class AutoCompleteEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<AutoCompleteEngine>();
        engine->initialize();
    }
    
    void TearDown() override {
        engine.reset();
    }
    
    std::unique_ptr<AutoCompleteEngine> engine;
};

// Basic functionality tests
TEST_F(AutoCompleteEngineTest, InitializationTest) {
    EXPECT_GT(engine->getDictionarySize(), 0);
}

TEST_F(AutoCompleteEngineTest, EmptyPrefixReturnsNoSuggestions) {
    auto suggestions = engine->getSuggestions("");
    EXPECT_TRUE(suggestions.empty());
}

TEST_F(AutoCompleteEngineTest, ValidPrefixReturnsSuggestions) {
    auto suggestions = engine->getSuggestions("th");
    EXPECT_FALSE(suggestions.empty());
    EXPECT_LE(suggestions.size(), 5); // Should return at most 5 suggestions
}

TEST_F(AutoCompleteEngineTest, AddCustomWordWorks) {
    std::string customWord = "testword123";
    EXPECT_TRUE(engine->addCustomWord(customWord));
    
    auto suggestions = engine->getSuggestions("testw");
    EXPECT_FALSE(suggestions.empty());
    
    bool found = false;
    for (const auto& suggestion : suggestions) {
        if (suggestion == customWord) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(AutoCompleteEngineTest, DuplicateWordRejected) {
    std::string word = "duplicate";
    EXPECT_TRUE(engine->addCustomWord(word));
    EXPECT_FALSE(engine->addCustomWord(word)); // Should reject duplicate
}

TEST_F(AutoCompleteEngineTest, InvalidWordRejected) {
    EXPECT_FALSE(engine->addCustomWord("")); // Empty word
    EXPECT_FALSE(engine->addCustomWord("123")); // Numbers only
    EXPECT_FALSE(engine->addCustomWord("test@word")); // Invalid characters
}

TEST_F(AutoCompleteEngineTest, SelectionUpdatesFrequency) {
    auto suggestions = engine->getSuggestions("th");
    ASSERT_FALSE(suggestions.empty());
    
    std::string selectedWord = suggestions[0];
    engine->selectSuggestion(selectedWord);
    
    // After selection, the word should still appear in suggestions
    auto newSuggestions = engine->getSuggestions("th");
    bool found = false;
    for (const auto& suggestion : newSuggestions) {
        if (suggestion == selectedWord) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

// Trie-specific tests
class TrieTest : public ::testing::Test {
protected:
    void SetUp() override {
        trie = std::make_unique<Trie>();
    }
    
    std::unique_ptr<Trie> trie;
};

TEST_F(TrieTest, InsertAndContains) {
    trie->insert("hello");
    EXPECT_TRUE(trie->contains("hello"));
    EXPECT_FALSE(trie->contains("hell"));
    EXPECT_FALSE(trie->contains("world"));
}

TEST_F(TrieTest, PrefixSearch) {
    trie->insert("hello");
    trie->insert("help");
    trie->insert("world");
    
    auto results = trie->search("hel");
    EXPECT_EQ(results.size(), 2);
    
    auto noResults = trie->search("xyz");
    EXPECT_TRUE(noResults.empty());
}

TEST_F(TrieTest, SizeTracking) {
    EXPECT_EQ(trie->size(), 0);
    
    trie->insert("word1");
    EXPECT_EQ(trie->size(), 1);
    
    trie->insert("word2");
    EXPECT_EQ(trie->size(), 2);
    
    trie->insert("word1"); // Duplicate shouldn't increase size
    EXPECT_EQ(trie->size(), 2);
}

// FrequencyTracker tests
class FrequencyTrackerTest : public ::testing::Test {
protected:
    void SetUp() override {
        tracker = std::make_unique<FrequencyTracker>();
    }
    
    std::unique_ptr<FrequencyTracker> tracker;
};

TEST_F(FrequencyTrackerTest, InitialFrequencyIsZero) {
    EXPECT_EQ(tracker->getFrequency("nonexistent"), 0);
}

TEST_F(FrequencyTrackerTest, UpdateFrequency) {
    tracker->updateFrequency("test");
    EXPECT_EQ(tracker->getFrequency("test"), 1);
    
    tracker->updateFrequency("test");
    EXPECT_EQ(tracker->getFrequency("test"), 2);
}

TEST_F(FrequencyTrackerTest, InitializeWord) {
    tracker->initializeWord("newword");
    EXPECT_EQ(tracker->getFrequency("newword"), 1);
    EXPECT_GT(tracker->getLastUsed("newword"), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}