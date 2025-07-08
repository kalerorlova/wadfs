#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stack>
#include <regex>
#include <algorithm>
#include "Wad.h"
using namespace std;

/*
Wad::Wad(const string &path) {
    fstream wad(path);
    wad.read((char*) &magic, 4);
    wad.read((char*) &n, 4);
    wad.read((char*) &start, 4);
    root = nullptr;
}
*/


Wad::~Wad() {
    deleteTree(root);
    root = nullptr;
    close(fd);
}

void Wad::deleteTree(Node* root) {
    if (root == nullptr) {
        return;
    }
    else {
        for (int i = 0; i < root->children.size(); i++) {
            deleteTree(root->children[i]);
        }
        root->children.clear();
        delete root;
    }
}

void Wad::printTree(Node* root, string &x) {
    if (root == nullptr) {
        return;
    }
    else {
        for (int i = 0; i < root->children.size(); i++) {
            printTree(root->children[i], x);
        }
        x += root->name;
    }
}


Wad* Wad::loadWad(const string &path) {
    //Wad* wad = new Wad(path);
    Wad* wad = new Wad();
    wad->fd = open(path.c_str(), O_RDWR);
    char m[5];
    m[4] = '\0';
    read(wad->fd, &(m), 4);
    wad->magic = string(m);
    read(wad->fd, &(wad->n), 4);
    read(wad->fd, &(wad->start), 4);
    wad->root = new Node();
    //stack<Node*> s;
    stack<Node*>* s = new stack<Node*>();
    s->push(wad->root);

    //process descriptors
    int inEM = 0;
    lseek(wad->fd, wad->start, SEEK_SET);
    for (int i = 0; i < wad->n; i++) {
        int tempOffset;
        int tempLen;
        char buf[9];
        buf[8] = '\0';
        read(wad->fd, &tempOffset, 4);
        read(wad->fd, &tempLen, 4);
        read(wad->fd, &buf, 8);
        string tempName = string(buf);
        wad->titles.push_back(tempName);

        if (tempName.find("_START") != string::npos) {
            Node* temp = new Node();
            temp->offset = tempOffset;
            temp->len = tempLen;
            temp->name = tempName.substr(0, tempName.find("_START"));
            temp->type = "dir";
            (s->top()->children).push_back(temp);
            s->push(temp);
        }
        else if (tempName.length() > 3 && tempName.substr(tempName.length() - 4) == "_END") {
            s->pop();
        }
        else if (regex_match(tempName, regex("(E)[0-9](M)[0-9]"))) {
            Node* temp = new Node();
            temp->offset = tempOffset;
            temp->len = tempLen;
            temp->name = tempName;
            temp->type = "dir";
            (s->top()->children).push_back(temp);
            s->push(temp);
            inEM = 10;
        }
        else if (inEM > 0) {
            Node* temp = new Node();
            temp->offset = tempOffset;
            temp->len = tempLen;
            temp->name = tempName;
            temp->type = "file";
            (s->top()->children).push_back(temp);
            if (inEM == 1) {
                s->pop();
            }
            inEM--;
        }
        else {
            Node* temp = new Node();
            temp->offset = tempOffset;
            temp->len = tempLen;
            temp->name = tempName;
            temp->type = "file";
            (s->top()->children).push_back(temp);
        }     
    }
    //close(fd);
    wad->root = s->top();
    s->pop();
    delete s;
    return wad;
}

string Wad::getMagic() {
    //return to_string(n);
    return magic;
    //string x = "";
    //printTree(root, x);
    //for (int i = 0; i < titles.size(); i++) {
    //    x += titles[i];
    //}
    //return x;
}

bool Wad::isContent(const string &path) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    if (temp->type == "file") {
        return true;
    }
    return false;
}

bool Wad::isDirectory(const string &path) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    if (temp->type == "dir" && !path.empty()) {
        return true;
    }
    return false;
}

int Wad::getSize(const string &path) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return -1;
        }
    }
    if (temp->type == "file") {
        return temp->len;
    }
    return -1;
}

int Wad::getContents(const string &path, char* buffer, int length, int offset) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return -1;
        }
    }
    if (temp->type == "file") {
        if (length + offset <= temp->len) {
            lseek(fd, temp->offset + offset, SEEK_SET);
            temp->data = read(fd, buffer, length + offset);
            return length;
        }
        else if (length > temp->len && offset < temp->len){
            lseek(fd, temp->offset + offset, SEEK_SET);
            temp->data = read(fd, buffer, temp->len - offset);
            return temp->len - offset;
        }
        else {
            return 0;
        }
    }
    return -1;
}

int Wad::getDirectory(const string &path, vector<string> *directory) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return -1;
        }
    }
    if (temp->type == "dir" && !path.empty()) {
        for (int i = 0; i < temp->children.size(); i++) {
            directory->push_back(temp->children[i]->name);
        }
        return temp->children.size();
    }
    return -1;
}

void Wad::createDirectory(const string &path) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size() - 1; i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return;
        }
    }
    if (temp->type == "dir" && !path.empty() && (temp->name.length() == 2 || temp->name.length() == 0)) {
        int ind = traverse.size() - 1;
        if (traverse[ind].size() != 2) {
            return;
        }
        Node* dir = new Node();
        dir->offset = 0;
        dir->len = 0;
        dir->name = traverse[ind];
        dir->type = "dir";
        temp->children.push_back(dir);
        lseek(fd, 0, SEEK_END);
        char buf[16];
        memset(buf, 0, sizeof(buf));
        write(fd, buf, sizeof(buf));
        write(fd, buf, sizeof(buf));
        auto it = titles.begin();
        for (int i = 0; i < traverse.size() - 1; i++) {
            it = find(it, titles.end(), traverse[i] + "_START");
        }
        it = find(it, titles.end(), temp->name + "_END");
        int mv = titles.size() - distance(titles.begin(), it);
        titles.insert(it, dir->name + "_START");
        titles.insert(it + 1, dir->name + "_END");
        lseek(fd, start + (n - 1) * 16, SEEK_SET);
        for (int i = 0; i < mv; i++) {
            read(fd, buf, 16);
            lseek(fd, 16, SEEK_CUR);
            write(fd, buf, 16);
            lseek(fd, -64, SEEK_CUR);
        }
        lseek(fd, 16, SEEK_CUR);
        write(fd, &(dir->offset), 4);
        write(fd, &(dir->len), 4);
        write(fd, (dir->name + "_START").c_str(), 8);
        write(fd, &(dir->offset), 4);
        write(fd, &(dir->len), 4);
        write(fd, (dir->name + "_END").c_str(), 8);
        lseek(fd, 4, SEEK_SET);
        n += 2;
        write(fd, &n, 4);
        fsync(fd);
    }
    return;
}

void Wad::createFile(const string &path) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size() - 1; i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return;
        }
    }
    if (temp->type == "dir" && !path.empty() && (temp->name.length() == 2 || temp->name.length() == 0)) {
        int ind = traverse.size() - 1;
        if (traverse[ind].size() > 8 || traverse[ind].find("_START") != string::npos
            || (traverse[ind].length() > 3 && traverse[ind].substr(traverse[ind].length() - 4) == "_END")
            || regex_match(traverse[ind], regex("(E)[0-9](M)[0-9]"))) {
            return;
        }
        Node* file = new Node();
        file->offset = 0;
        file->len = 0;
        file->name = traverse[ind];
        file->type = "file";
        temp->children.push_back(file);
        lseek(fd, 0, SEEK_END);
        char buf[16];
        memset(buf, 0, sizeof(buf));
        write(fd, buf, sizeof(buf));
        auto it = titles.begin();
        for (int i = 0; i < traverse.size() - 1; i++) {
            it = find(it, titles.end(), traverse[i] + "_START");
        }
        it = find(it, titles.end(), temp->name + "_END");
        int mv = titles.size() - distance(titles.begin(), it);
        titles.insert(it, file->name);
        lseek(fd, start + (n - 1) * 16, SEEK_SET);
        for (int i = 0; i < mv; i++) {
            read(fd, buf, 16);
            write(fd, buf, 16);
            lseek(fd, -48, SEEK_CUR);
        }
        lseek(fd, 16, SEEK_CUR);
        write(fd, &(file->offset), 4);
        write(fd, &(file->len), 4);
        write(fd, (file->name).c_str(), 8);
        lseek(fd, 4, SEEK_SET);
        n += 1;
        write(fd, &n, 4);
        fsync(fd);
    }
    return;
}

int Wad::writeToFile(const string &path, const char* buffer, int length, int offset) {
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    Node* temp = root;
    bool found = false;
    for (int i = 0; i < traverse.size(); i++) {
        found = false;
        for (int j = 0; j < temp->children.size(); j++) {
            if (temp->children[j]->name == traverse[i]) {
                found = true;
                temp = temp->children[j];
                break;
            }
        }
        if (!found) {
            return -1;
        }
    }
    if (temp->type == "file") {
        if (temp->len > 0) {
            return 0;
        }
        lseek(fd, 0, SEEK_END);
        char buf[length];
        memset(buf, 0, sizeof(buf));
        write(fd, buf, sizeof(buf));
        int sz = 16 * n;
        char descs[sz];
        lseek(fd, start, SEEK_SET);
        read(fd, descs, sz);
        lseek(fd, -sz, SEEK_END);
        write(fd, descs, sz);
        int ofst = (offset == 0) ? start : offset;
        start += length;
        lseek(fd, 8, SEEK_SET);
        write(fd, &start, 4);
        auto it = titles.begin();
        for (int i = 0; i < traverse.size() - 1; i++) {
            it = find(it, titles.end(), traverse[i] + "_START");
        }
        it = find(it, titles.end(), temp->name);
        int mv = distance(titles.begin(), it);
        lseek(fd, start + 16 * mv, SEEK_SET);
        write(fd, &ofst, 4);
        write(fd, &length, 4);
        temp->len = length;
        temp->offset = ofst;
        lseek(fd, ofst, SEEK_SET);
        write(fd, buffer, length);
        fsync(fd);
        return length;
    }
    return -1;
}
