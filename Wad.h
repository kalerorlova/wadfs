#pragma once
#include <string>
#include <vector>
using namespace std;

struct Node {
    int offset = -1;
    int len = -1;
    string name = "";
    string data = "";
    string type = "dir";
    vector<Node*> children;
};

class Wad {
    int fd;
    string magic;
    int n;
    int start;
    Node* root;
    vector<string> titles;
    //Wad(const string &path);

    public:
    static Wad* loadWad(const string &path);
    string getMagic();
    bool isContent(const string &path);
    bool isDirectory(const string &path);
    int getSize(const string &path);
    int getContents(const string &path, char* buffer, int length, int offset = 0);
    int getDirectory(const string &path, vector<string> *directory);
    void createDirectory(const string &path);
    void createFile(const string &path);
    int writeToFile(const string &path, const char* buffer, int length, int offset = 0);

    ~Wad();
    void deleteTree(Node* root);
    void printTree(Node* root, string &x);
};