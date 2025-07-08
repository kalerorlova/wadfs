#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int main() {
    /*
    fstream file;
    file.open("./testfiles/sample1.wad", ios::in | ios::out | ios::binary);

    char magic[5];
    magic[4] = '\0';
    int n;
    int start;
    file.read(magic, 4);
    file.read((char*) &n, 4);
    file.read((char*) &start, 4);
    cout << "Magic: " << magic << endl;
    cout << "N: " << n << endl;
    cout << "start: " << start << endl;*/

    /*
    const string path = "/E1M0/05.txt/";
    vector<string> traverse;
    int startInd = 0;
    if (!path.empty() && path.at(0) == '/') {
        startInd = 1;
    }
    while ((path.substr(startInd)).find("/") != string::npos) {
        string name = path.substr(startInd, (path.substr(startInd)).find("/"));


        //string name = path.substr(startInd, (path.substr(startInd)).find("/") - startInd);
        traverse.push_back(name);
        startInd += name.length() + 1;
        if (startInd >= path.length()) {
            break;
        }
    }
    if (startInd < path.length()) {
        traverse.push_back(path.substr(startInd));
    }
    for (int i = 0; i < traverse.size(); i++) {
        cout << traverse[i] << endl;
    }
    cout << "size: " << traverse.size() << endl;*/

    vector<int> vec{10, 20, 30, 40};
    int key = 30;
    auto it = find(vec.begin(), vec.end(), key);
    int dist = distance(vec.begin(), it);
    //cout << "found at: " << it - vec.begin() + 1<< endl;
    cout << "dist: " << dist << endl;

    return 0;
}