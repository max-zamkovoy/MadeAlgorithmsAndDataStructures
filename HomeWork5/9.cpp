#include <iostream>
#include <unordered_map>
#include "Huffman.h"
using namespace std;
using uchar = unsigned char;
using code_dict = unordered_map<char, long>;
using alpha_dict = unordered_map<uchar, long>;


struct HeapNode {
    //Node of min-heap
    uchar symb = 0;
    size_t freq = 0;
    HeapNode* left = nullptr;
    HeapNode* right = nullptr;

    HeapNode(uchar symbol, size_t count = 0) : symb(symbol), freq(count), left(nullptr), right(nullptr) {}
    HeapNode(HeapNode* left, HeapNode* right);
    bool IsLeftNode() const;
};

struct Comparator {
    //Comparator utility
    bool operator()(const HeapNode* left, const HeapNode* right) {
        return left->freq > right->freq;
    }
};

HeapNode::HeapNode(HeapNode* left_, HeapNode* right_) {
    //Node constructor
    symb = 0;
    freq = left_->freq + right_->freq;
    left = left_;
    right = right_;
}

bool HeapNode::IsLeftNode() const {
    //Boolean utility
    return (left == nullptr);
}

struct Heap {
    //Heap structure
    unsigned size;
    unsigned capacity;
    struct HeapNode** array;
};




struct Heap* createHeap(unsigned capacity){
    //Heap constructor
    struct Heap* minHeap
            = (struct Heap*)malloc(sizeof(struct Heap));

    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array
            = (struct HeapNode**)malloc(minHeap->
            capacity * sizeof(struct HeapNode*));
    return minHeap;
}

void swapNodes(struct HeapNode** a,struct HeapNode** b){
    //Swapping utility
    struct HeapNode* t = *a;
    *a = *b;
    *b = t;
}

void heapify(struct Heap* minHeap, int index){
    //Utility for creating a heap
    int least = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < minHeap->size && minHeap->array[left]->
            freq < minHeap->array[least]->freq)
        least = left;

    if (right < minHeap->size && minHeap->array[right]->
            freq < minHeap->array[least]->freq)
        least = right;

    if (least != index) {
        swapNodes(&minHeap->array[least],
                        &minHeap->array[index]);
        heapify(minHeap, least);
    }
}

int isOneEl(struct Heap* minHeap){
    //Boolean utility
    return minHeap->size == 1;
}

struct HeapNode* getMin(struct Heap* minHeap){
    //Function to get min
    struct HeapNode* temp = minHeap->array[0];
    minHeap->array[0]
            = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    heapify(minHeap, 0);
    return temp;
}

void heapAdd(struct Heap* minHeap,struct HeapNode* minHeapNode){
    //Function to add an element to heap
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildHeap(struct Heap* minHeap){
    //Function to build a heap
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        heapify(minHeap, i);
}

int isLeaf(struct HeapNode* root){
    // Utility function to check if this node is leaf
    return !(root->left) && !(root->right);
}

struct Heap* createAndBuildMinHeap(char data[], int freq[], int size){
    //Creating the min heap
    struct Heap* minHeap = createHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = new HeapNode(data[i], freq[i]);
    minHeap->size = size;
    buildHeap(minHeap);
    return minHeap;
}


struct HeapNode* buildHuffmanTree(char data[], int freq[], int size){
    // The main function that builds Huffman tree
    struct HeapNode *left, *right, *top;
    struct Heap* minHeap = createAndBuildMinHeap(data, freq, size);
    // Iterate while size of heap doesn't become 1
    while (!isOneEl(minHeap)) {
        left = getMin(minHeap);
        right = getMin(minHeap);
        top = new HeapNode(left,right);
        top->left = left;
        top->right = right;
        heapAdd(minHeap, top);
    }
    return getMin(minHeap);
}

void createCodeTable(HeapNode* node, vector<uchar>& code, unordered_map< uchar, vector<uchar> >& table) {
    //Creating code table out of Huffman tree
    if (node->left) {
        code.push_back(0);
        createCodeTable(node->left, code, table);
    }
    if (node->right) {
        code.push_back(1);
        createCodeTable(node->right, code, table);
    }

    if (node->symb || (!node->symb && node->left == NULL))
        table[node->symb] = code;
    if (code.size())
        code.pop_back();
}

static void copyStream(IInputStream& input, IOutputStream& output)
{
    uchar value;
    while (input.Read(value))
    {
        output.Write(value);
    }
}



void AssignCodeToNode(HeapNode* node, vector<bool>& tree) {
    //A function to assign a code to node
    if (node->left == NULL) {
        tree.push_back(1);
        uchar symb = node->symb;
        for (size_t i = 0; i < 8; i++) {
            tree.push_back(symb & 1);
            symb >>= 1;
        }
    }
    else {
        tree.push_back(0);
        AssignCodeToNode(node->left, tree);
        AssignCodeToNode(node->right, tree);
    }
}

HeapNode* ReconstructTree(vector<bool>& tree, size_t& index) {
    //Reconstruct a tree for decoding purposes
    if (tree[index++] == 1) {
        uchar symbol = 0;
        for (size_t i = 0; i < 8; i++) {
            symbol |= tree[index] << i;
            index++;
        }
        return new HeapNode(symbol);
    }
    else {
        HeapNode* left = ReconstructTree(tree, index);
        HeapNode* right = ReconstructTree(tree, index);
        return new HeapNode(left, right);
    }
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
    // Calculate symbol frequency
    alpha_dict symbols;
    uchar value = 0;
    std::string input;
    while (original.Read(value)) {
        symbols[value]++;
        input += value;
    }
    char symbs[symbols.size()];
    int freqs[symbols.size()];
    int ind = 0;
    for( auto const& [key, val] : symbols)
    {
        symbs[ind] = key;
        freqs[ind] = val;
        ind++;
    }

    // Build encoding tree
    HeapNode* root = buildHuffmanTree(symbs,freqs,sizeof(symbs) / sizeof(symbs[0]));
    vector<uchar> code;
    unordered_map< uchar, vector<uchar> > encoding_table;
    createCodeTable(root, code, encoding_table);

    vector<bool> message_encoded;
    AssignCodeToNode(root, message_encoded);

    for (uchar c : input) {
        vector<uchar>& encoded_symbol = encoding_table[c];
        for (size_t j = 0; j < encoding_table[c].size(); j++)
            message_encoded.push_back(encoding_table[c][j]);
    }
    message_encoded.push_back(true);

    //  Compress and save data
    size_t i = 0;
    while (i < message_encoded.size()) {
        value = 0;
        for (size_t j = 0; (j < 8) & (i < message_encoded.size()); j++) {
            value |= message_encoded[i] << j;
            i++;
        }
        compressed.Write(value);
    }
}



void Decode(IInputStream& compressed, IOutputStream& original)
{
    // Read compressed data
    vector<bool> message_encoded;
    uchar symb;
    while (compressed.Read(symb)) {
        for (size_t i = 0; i < 8; i++) {
            message_encoded.push_back(symb & 1);
            symb >>= 1;
        }
    }

    while (!message_encoded.back())
        message_encoded.pop_back();
    message_encoded.pop_back();

    size_t index = 0;
    //Reconstruct tree
    HeapNode* root = ReconstructTree(message_encoded, index);

    //Write the decoded message
    vector<uchar> output;
    HeapNode* node = root;
    while (index < message_encoded.size()) {
        if (!message_encoded[index++]) {
            node = node->left;
        }
        else {
            node = node->right;
        }
        if (node->IsLeftNode()) {
            output.push_back(node->symb);
            node = root;
        }
    }

    for (size_t i = 0; i < output.size(); i++)
        original.Write(output[i]);
}