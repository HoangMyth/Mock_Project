#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "Mediafile.h"
#include "Playlist.h"

class Model {
public:
    //Get media file in a directory
    std::vector<MediaFile> getMediaFiles(const std::string &directory);
    std::vector<MediaFile> getMusicFiles(const std::string &directory);
    std::vector<MediaFile> getVideoFiles(const std::string &directory);
    //Get mediafile path in a directory
    std::vector<std::string> getMediaFilepaths(const std::string &directory);
    std::vector<std::string> getMusicFilepaths(const std::string &directory);
    std::vector<std::string> getVideoFilepaths(const std::string &directory);
    //Get Playlist 
    std::vector<Playlist> getPlaylistLists();
    //Get and Edit Metadata of file
    void getMetadata(const std::string& filePath, std::vector<MediaFile>& files);
    void editMediaFileMetadata(const MediaFile &mediaFile);
    //Playlist getPlaylist(const std::string &name);
};

#endif // MODEL_H
