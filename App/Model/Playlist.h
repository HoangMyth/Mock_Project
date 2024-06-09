#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include <vector>
#include "Mediafile.h"

class Playlist {
public:
//Constructors
    Playlist();
//Methods
    std::vector<MediaFile>& getFiles();
    //const std::vector<MediaFile>& getFiles() const;
    std::string getName() const;
    void setName(const std::string &name);
    int getNum() const;
    void addFile(const MediaFile &file);
    void removeFile(int num);
    void deleteFile();
    MediaFile getFile(int num_file)const;

private:
    std::string name;
    std::vector<MediaFile> files;
};

#endif // PLAYLIST_H
