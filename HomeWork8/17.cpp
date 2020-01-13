#define NDEBUG

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <queue>
#include <vector>

/* Forpattern matching search */
class Bohr
{
private:
    /* Bohr node */
    struct BohrNode
    {
        char val;                                 
        int parent_;                            
        std::unordered_map<char, int> children_; 
        int suffixLink_;                         
        int entry_suf_link;                    
        int entry_;                              

        BohrNode(): suffixLink_(-1), entry_suf_link(-1), entry_(-1){}
        BohrNode(char value, int parent): val(value), parent_(parent), suffixLink_(-1), 
            entry_suf_link(-1), entry_(-1) {}
    };

    std::vector<BohrNode> nodes_;
    int entryCount_;          
    int cur_node;

    //Utility variables
    int getChildIndex(int node_ind, char child_val) const;
    int addChild(int par_node_index, char child_val);
    int getNext(int node_ind, char value);
    int getSuffixLink(int node_ind);
    int getEntrySuffixLink(int node_ind);
public:
    explicit Bohr();
    //Add new pattern to the bohr, substring case
    int addPattern(const std::string& pattern, std::size_t beginIdx, std::size_t endIdx);
    //Add new pattern to the bohr, whole string case
    int addPattern(const std::string& pattern);
    void init()
    {
        cur_node = 0;
    }
    //Push next value to the heap
    void next(char value, bool* matches, bool& hasMatches);
};

/* Token that describes pattern registered in the bohr as a part of the general pattern */
struct Searcher
{
    std::size_t offset_; // number of characters from end of the previous pattern to the last character of this pattern
    std::size_t length_; // total number of characters in this pattern
    int entry_;      // index of bohr entry

    Searcher(){}
    Searcher(std::size_t offset, std::size_t length, int entry) :
            offset_(offset), length_(length), entry_(entry){}
};

/* Token used by priority queue of the pending patterns */
struct queueToken
{
    std::size_t string_index;   // the text index where the pattern expected to be
    int entry_;             // pattern index
    std::size_t sequenceindex; // wildcard sequence token index

    queueToken(){}
    queueToken(std::size_t stringIndex, int entry, std::size_t sequenceIndex) :
            string_index(stringIndex), entry_(entry), sequenceindex(sequenceIndex){}

    friend bool operator<(const queueToken& lhs, const queueToken& rhs)
    {
        return lhs.string_index > rhs.string_index;
    }
};

/* Searches a text for the pattern with wildcards */
class WholePatternSearcher
{
public:
private:
    std::size_t size_;                    // whole pattern size
    Bohr bohr_;                           // bohr that stores partial patterns
    std::size_t wild_prefix;          // number of wildcard characters before any definite characters
    std::size_t wild_postfix;         // number of wildcard characters after any definite characters
    std::vector<Searcher> sequence; // sequence of tokens to bohr patterns
public:
    explicit WholePatternSearcher(const std::string& pattern);
    std::vector<int> search(const std::string& text);

};

int Bohr::getChildIndex(int node_ind, char child_val) const
{
    assert(node_ind >= 0 && node_ind < nodes_.size());
    const BohrNode& node = nodes_[static_cast<std::size_t>(node_ind)];
    auto found = node.children_.find(child_val);
    return found == node.children_.end() ? -1l : found->second;
}
int Bohr::addChild(int par_node_index, char child_val)
{
    assert(par_node_index >= 0 && par_node_index < nodes_.size());
    assert(getChildIndex(par_node_index, child_val) < 0);
    BohrNode& parentNode = nodes_[static_cast<std::size_t>(par_node_index)];
    int result = static_cast<int>(nodes_.size());
    parentNode.children_.insert({child_val, nodes_.size()});
    nodes_.emplace_back(child_val, par_node_index);
    return result;
}
int Bohr::getNext(int node_ind, char value)
{
    int child_ind = getChildIndex(node_ind, value);
    if (child_ind != -1)
        return child_ind;
    if (!node_ind)
        return 0;
    int result = getNext(getSuffixLink(node_ind), value);
    addChild(result, value);
    return result;
}
int Bohr::getSuffixLink(int node_ind)
{
    assert(node_ind >= 0 && node_ind < nodes_.size());
    BohrNode& node = nodes_[static_cast<std::size_t>(node_ind)];
    if (node.suffixLink_ != -1)
        return node.suffixLink_;
    else
        return node.suffixLink_ = node.parent_ ? getNext(getSuffixLink(node.parent_), node.val) : 0;
}
int Bohr::getEntrySuffixLink(int node_ind)
{
    assert(node_ind >= 0 && node_ind < nodes_.size());
    BohrNode& node = nodes_[static_cast<std::size_t>(node_ind)];
    if (node.entry_suf_link != -1)
    {
        return node.entry_suf_link;
    }
    else
    {
        int suffixLink = getSuffixLink(node_ind);
        return node.entry_suf_link = nodes_[static_cast<std::size_t>(suffixLink)].entry_ >= 0 ? suffixLink : getEntrySuffixLink(suffixLink);
    }
}
Bohr::Bohr() :
        entryCount_(0), cur_node(0)
{
    // init root node
    nodes_.emplace_back();
    nodes_[0].suffixLink_ = 0;
    nodes_[0].entry_suf_link = 0;
}
int Bohr::addPattern(const std::string& pattern, std::size_t beginIdx, std::size_t endIdx)
{
    int currentNode = 0;
    for (std::size_t i = beginIdx; i < endIdx; ++i)
    {
        char ch = pattern[i];
        int child = getChildIndex(currentNode, ch);
        currentNode = child >= 0 ? child : addChild(currentNode, ch);
    }
    if (nodes_[static_cast<std::size_t>(currentNode)].entry_ == -1)
    {
        nodes_[static_cast<std::size_t>(currentNode)].entry_ = entryCount_++;
    }
    return nodes_[static_cast<std::size_t>(currentNode)].entry_;
}
int Bohr::addPattern(const std::string& pattern)
{
    return addPattern(pattern, 0, pattern.size());
}
void Bohr::next(char value, bool* matches, bool& hasMatches)
{
    int child_ind;
    assert(cur_node >= 0 && cur_node < nodes_.size());
    while (-1 == (child_ind = getChildIndex(cur_node, value)) && cur_node)
    {
        cur_node = getSuffixLink(cur_node);
        if (nodes_[static_cast<std::size_t>(cur_node)].entry_ != -1)
        {
            matches[nodes_[static_cast<std::size_t>(cur_node)].entry_] = hasMatches = true;
        }
    }
    assert(child_ind >= 0 || !cur_node);
    if (child_ind >= 0)
    {
        cur_node = child_ind;
    }
    if (cur_node)
    {
        if (nodes_[static_cast<std::size_t>(cur_node)].entry_ != -1)
        {
            matches[nodes_[static_cast<std::size_t>(cur_node)].entry_] = hasMatches = true;
        }
        while ((child_ind = getEntrySuffixLink(child_ind)))
        {
            if (nodes_[static_cast<std::size_t>(child_ind)].entry_ != -1)
            {
                matches[nodes_[static_cast<std::size_t>(child_ind)].entry_] = hasMatches = true;
            }
        }
    }
}

WholePatternSearcher::WholePatternSearcher(const std::string& pattern):
        size_(pattern.size())
{
    std::size_t offsetStart = 0;  // end index of the previous pattern
    std::size_t pattern_start;     // start index of current pattern
    bool readPattern = pattern[0] != '?'; // true, if the last symbol is not a wildcard
    if (readPattern)
    {
        pattern_start = 0;
    }
    std::size_t i;
    for (i = 0; i < size_; ++i)
    {
        bool isPatternChar = pattern[i] != '?';
        if (isPatternChar && !readPattern)
        {
            pattern_start = i;
            readPattern = true;
        }
        else if (!isPatternChar && readPattern)
        {
            long bohrEntryIndex = bohr_.addPattern(pattern, pattern_start, i);
            sequence.emplace_back(i - offsetStart, i - pattern_start, bohrEntryIndex);
            offsetStart = i;
            readPattern = false;
        }
    }
    if (readPattern) // in the end is a pattern, put it into the bohr
    {
        long bohrEntryIndex = bohr_.addPattern(pattern, pattern_start, size_);
        sequence.emplace_back(size_ - offsetStart, size_ - pattern_start, bohrEntryIndex);
        wild_prefix = sequence[0].offset_ - sequence[0].length_;
        wild_postfix = 0;
    }
    else if (sequence.size()) // in the end are some wildcards
    {
        wild_prefix =  sequence[0].offset_ - sequence[0].length_;
        wild_postfix = size_ - offsetStart;
    }
    else // all pattern consists of wildcards
    {
        wild_prefix =  size_;
        wild_postfix = 0;
    }
}

/* Search a text for the wildcard pattern */
std::vector<int> WholePatternSearcher::search(const std::string& text)
{
    std::vector<int> result;
    if (text.size() < size_)
    {
        return result; // text is smaller than pattern
    }
    else if (!sequence.size())
    {
        for (std::size_t i = 0; i < text.size() - wild_prefix + 1; ++i)
        {
            result.push_back(i);
        }
        return result; // pattern contains only wildcards
    }

    bohr_.init();
    auto initialEntry = sequence[0];
    bool hasMatches = false;
    bool* matches = new bool[sequence.size()];
    std::fill(matches, matches + sequence.size(), false);

    if (sequence.size() == 1)
    {
        for (std::size_t i = wild_prefix; i < text.size() - wild_postfix; ++i)
        {
            hasMatches = false;
            bohr_.next(text[i], matches, hasMatches);
            if (hasMatches)
            {
                result.push_back(i + 1 - initialEntry.length_ - wild_prefix);
                matches[0] = false;
            }
        }

        delete[] matches;

        return result; 
    }

    std::priority_queue<queueToken> timers; 
    for (std::size_t i = wild_prefix; i < text.size() - wild_postfix; ++i)
    {
        // 0. check if there are any expected patterns at symbol (i - 1)
        std::stack<std::size_t> newTimers; // new items for the timer queue
        while (!timers.empty() && timers.top().string_index == i)
        {
            if (matches[timers.top().entry_])
            {
                std::size_t sequenceIndex = timers.top().sequenceindex;
                if (sequenceIndex == sequence.size() - 1)
                {
                    assert(i >= size_ - wild_postfix);
                    result.push_back(i + wild_postfix - size_);
                }
                else
                {
                    ++sequenceIndex;
                    newTimers.emplace(sequenceIndex);
                }
            }
            timers.pop();
        }
        // 1. put next expected patterns in the queue
        while(!newTimers.empty())
        {
            timers.emplace(i + sequence[newTimers.top()].offset_, sequence[newTimers.top()].entry_, newTimers.top());
            newTimers.pop();

        }
        // 2. check the bohr if there are any matches
        if (hasMatches)
        {
            std::fill(matches, matches + sequence.size(), false);
            hasMatches = false;
        }
        bohr_.next(text[i], matches, hasMatches);
        if (matches[0])
        {
            timers.emplace(i + sequence[1].offset_ + 1, sequence[1].entry_, 1);
        }
    }

    // process the terminal matches
    if (matches[timers.top().entry_])
    {
        while (!timers.empty() && timers.top().string_index == text.size() - wild_postfix)
        {
            if (timers.top().sequenceindex == sequence.size() - 1)
            {
                result.push_back(text.size() - wild_postfix + wild_postfix - size_);
                break;
            }
            timers.pop();
        }
    }

    delete[] matches;

    return result;
}

int main(void)
{
    std::string pattern, text;
    std::iostream::sync_with_stdio(false);

    std::cin >> pattern >> text;

    WholePatternSearcher searcher(pattern);
    auto result = searcher.search(text);
    for (auto index : result)
    {
        std::cout << index << ' ';
    }

    return 0;
}