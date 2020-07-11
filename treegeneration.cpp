#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <queue>

using namespace std;

#define TAB "  "
#define OUT_CLASS_NAME_BAD "SensitiveWordsData"
#define IN_FILE_NAME_BAD "badworddict.txt"
#define OUT_CLASS_NAME_GOOD "SensitiveWordsDataExclude"
#define IN_FILE_NAME_GOOD "goodworddict.txt"

struct TrieNode{
    unsigned char val;
    bool hasWord;
    map<int, TrieNode*> children;
    
    TrieNode(const int val, const bool hasWord){
        this->val = val;
        this->hasWord = hasWord;
    }
};

void printTrie(TrieNode* node, int depth, string& buffer){
    for (int i=0; i<depth; i++){
        buffer += TAB;
    }
    buffer += to_string((unsigned int)(node->val));
    buffer += " => array(";
    if (node->hasWord){
        buffer += "'w' => ";
        buffer += (node->hasWord ? "true" : "false");
        buffer += ", ";
    }
    
    buffer += "'c' => array(\n";
    for (auto& it : node->children){
        printTrie(it.second, depth+1, buffer);
    }
    for (int i=0; i<depth; i++){
        buffer += TAB;
    }
    buffer += "),\n";
    for (int i=0; i<depth-1; i++){
        buffer += TAB;
    }
    buffer += "),\n";
}

TrieNode* createTrie(const vector<string>& dict){
    TrieNode* root = new TrieNode(0, false);
    TrieNode *node;
    
    for (auto& word : dict){
        node = root;
        for (int i=0, n=word.length(); i<n; i++){
            unsigned char val = word[i];
            if (node->children.find(val) == node->children.end()){
                TrieNode *newNode = new TrieNode(val, i==n-1);
                node->children[val] = newNode;
            } else {
                if (node->children[val]->hasWord){
                    // meaning if we already have "ab" existing, "abc" is redundant.
                    break;
                } else {
                    node->children[val]->hasWord = (i==n-1);
                }
            }
            node = node->children[val];
        }
    }
    return root;
}

void deleteTrie(TrieNode* node){
    for (auto& it : node->children){
        deleteTrie(it.second);
    }
    //cout << node->val << " is deleted" << endl;
    delete(node);
}

void readDict(vector<string>& dict, const string& fileName){
    ifstream file(fileName);
    string str;
    while (!file.eof()){
        getline(file, str);
        dict.push_back(str);
    }
}

void buildAndWriteFile(const string className, TrieNode* root){
    string buffer = "<?php\nclass ";
    buffer += className;
    buffer += "{\n\tstatic $data = array(\n";
    printTrie(root, 0, buffer);
    buffer += ");\n}";
    
    ofstream myfile;
    string fileName = className + ".php";
    myfile.open(fileName);
    myfile << buffer;
    myfile.close();
}

void generateClassFromFile(const string fileName, const string className){
    vector<string> dict;
    readDict(dict, fileName);
    TrieNode* root = createTrie(dict);
    buildAndWriteFile(className, root);
    deleteTrie(root);
}

int main(int argc, char** argv){
    
    generateClassFromFile(IN_FILE_NAME_GOOD, OUT_CLASS_NAME_GOOD);
    generateClassFromFile(IN_FILE_NAME_BAD, OUT_CLASS_NAME_BAD);
    
    return 0;
}
