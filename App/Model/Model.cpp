#include "Model.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <taglib/fileref.h>
#include <taglib/tag.h>

namespace fs = std::filesystem;
//Get Media file in a directory
std::vector<MediaFile> Model::getMediaFiles(const std::string &directory) {
    std::vector<MediaFile> files;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".mp4" || ext == ".avi" || ext == ".mkv") {
                MediaFile file(entry.path().string(), entry.path().filename().string());
                std::string filePath = file.getPath();
                    TagLib::FileRef f(entry.path().string().c_str());
                    if (!f.isNull() && f.tag()) {
                        TagLib::Tag *tag = f.tag();
                        file.setTitle(tag->title().toCString(true));
                        file.setArtist(tag->artist().toCString(true));
                        file.setAlbum(tag->album().toCString(true));
                        file.setGenre(tag->genre().toCString(true));
                        file.setYear(tag->year());
                        file.setDuration(f.audioProperties()->length());
                    //return MediaFile file(entry.path().string(), tag->title().to8Bit(), tag->artist().to8Bit(), tag->album().to8Bit(), tag->genre().to8Bit(), tag->year(), f.audioProperties()->length());                           
                    }
                files.push_back(file);
            }
        }
    }
    return files;
}
//Get music file in a directory
std::vector<MediaFile> Model::getMusicFiles(const std::string &directory) {
    std::vector<MediaFile> files;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp3" ) {
                MediaFile file(entry.path().string(), entry.path().filename().string());
                std::string filePath = file.getPath();
                    TagLib::FileRef f(entry.path().string().c_str());
                    if (!f.isNull() && f.tag()) {
                        TagLib::Tag *tag = f.tag();
                        file.setTitle(tag->title().toCString(true));
                        file.setArtist(tag->artist().toCString(true));
                        file.setAlbum(tag->album().toCString(true));
                        file.setGenre(tag->genre().toCString(true));
                        file.setYear(tag->year());
                        file.setDuration(f.audioProperties()->length());
                    //return MediaFile file(entry.path().string(), tag->title().to8Bit(), tag->artist().to8Bit(), tag->album().to8Bit(), tag->genre().to8Bit(), tag->year(), f.audioProperties()->length());                           
                    }
                files.push_back(file);
            }
        }
    }
    return files;
}
//Get Video file in a directory
std::vector<MediaFile> Model::getVideoFiles(const std::string &directory) {
    std::vector<MediaFile> files;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp4" ) {
                MediaFile file(entry.path().string(), entry.path().filename().string());
                std::string filePath = file.getPath();
                    TagLib::FileRef f(entry.path().string().c_str());
                    if (!f.isNull() && f.tag()) {
                        TagLib::Tag *tag = f.tag();
                        file.setTitle(tag->title().to8Bit());
                        file.setArtist(tag->artist().to8Bit());
                        file.setAlbum(tag->album().to8Bit());
                        file.setGenre(tag->genre().to8Bit());
                        file.setYear(tag->year());
                        file.setDuration(f.audioProperties()->length());
                    }
                files.push_back(file);
            }
        }
    }
    return files;
}

//Get Media file in a directory
std::vector<std::string> Model::getMediaFilepaths(const std::string &directory) {
    std::vector<std::string> filepaths;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".mp4" || ext == ".avi" || ext == ".mkv") {
                filepaths.push_back(entry.path().string());
            }
        }
    }
    return filepaths;
}
//Get music file in a directory
std::vector<std::string> Model::getMusicFilepaths(const std::string &directory) {
    std::vector<std::string> filepaths;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp3") {
                filepaths.push_back(entry.path().string());
            }
        }
    }
    return filepaths;
}
//Get Video file in a directory
std::vector<std::string> Model::getVideoFilepaths(const std::string &directory) {
    std::vector<std::string> filepaths;
    for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp4") {
                filepaths.push_back(entry.path().string());
            }
        }
    }
    return filepaths;
}

//Get Metadata of file
void Model::getMetadata(const std::string& filePath, std::vector<MediaFile>& files) {
    TagLib::FileRef f(filePath.c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        MediaFile file;
        file.setPath(filePath);
        file.setTitle(tag->title().to8Bit());
        file.setArtist(tag->artist().to8Bit());
        file.setAlbum(tag->album().to8Bit());
        file.setGenre(tag->genre().to8Bit());
        file.setYear(tag->year());
        file.setDuration(f.audioProperties()->length());
        files.push_back(file);
    } else {
        std::cerr << "Could not read metadata from " << filePath << std::endl;
    }
}
void Model::editMediaFileMetadata(const MediaFile &mediaFile) {
    // Edit metadata of the media file
    TagLib::FileRef f(mediaFile.getPath().c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        tag->setTitle(mediaFile.getTitle());
        tag->setArtist(mediaFile.getArtist());
        tag->setAlbum(mediaFile.getAlbum());
        tag->setGenre(mediaFile.getGenre());
        tag->setYear(mediaFile.getYear());
        f.save();
    }
    // Placeholder implementation
}
