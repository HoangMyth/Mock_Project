#include "Playlist.h"
#include <fstream>
//Constructors
Playlist::Playlist() {}
//Method
std::string Playlist::getName() const { return name; }
void Playlist::setName(const std::string &name){this->name = name;}
int Playlist::getNum() const { return files.size(); }
std::vector<MediaFile>& Playlist::getFiles() { return files; }
//const std::vector<MediaFile>& Playlist::getFiles() const { return files; }
MediaFile Playlist::getFile(int num_file) const {
    return files[num_file];
}
void Playlist::addFile(const MediaFile &file) { files.push_back(file); }

void Playlist::removeFile(int num) {
    files.erase(std::begin(files) + num - 1);
}
void Playlist::deleteFile(){
    files.clear();
}
