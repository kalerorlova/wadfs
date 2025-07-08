#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <vector>
#include "../libWad/Wad.h"
using namespace std;

Wad* wad;

static int f_data(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    const string myPath(path);
    if (wad->isDirectory(myPath)) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
        return 0;
    }
    if (wad->isContent(myPath)) {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = wad->getSize(myPath);
        return 0;
    }
    return -ENOENT;
}

int f_mkFile(const char* path, mode_t mode, dev_t dev) {
    wad->createFile(path);
    return (wad->isContent(path)) ? 0 : -ENOENT;
}

int f_mkDir(const char* path, mode_t mode = 0777) {
    wad->createDirectory(path);
    return (wad->isDirectory(path)) ? 0 : -ENOENT;
}

static int f_readFile(const char* path, char* buf, size_t size, off_t offset, 
    struct fuse_file_info *fi) {
    const string myPath(path);
    return wad->getContents(myPath, buf, size, offset);
}


int f_writeFile(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    return wad->writeToFile(path, buf, size, offset);
}

int f_readDir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, 
    struct fuse_file_info* fi) {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    vector<string> dirs;
    int ret = wad->getDirectory(path, &dirs);
    if (ret == -1) {
        return -ENOENT;
    }
    for (int i = 0; i < dirs.size(); i++) {
        filler(buf, (dirs[i]).c_str(), NULL, 0);
    }
    return 0;    
}

struct fuse_operations f_oper = {
    .getattr = f_data,
    .mknod = f_mkFile,
    .mkdir = f_mkDir,
    .read = f_readFile,
    .write = f_writeFile,
    .readdir = f_readDir,
}; 

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "The args format: program name, -s, target WAD, mount dir" << endl;
        return 0;
    }
    string wadName = argv[2];
    string dirPath = argv[3];
    wad = Wad::loadWad(wadName);
    argv[2] = argv[3];
    argv[3] = NULL;
    argc--; 
    return fuse_main(argc, argv, &f_oper, wad);
}