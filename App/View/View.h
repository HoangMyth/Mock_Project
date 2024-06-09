#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <vector>
#include "Mediafile.h"
#include "Playlist.h"
#include <iomanip> 
class View {
public:
    std::string convertSecondsToTimeString(int totalSeconds);
    void printLine(char c, int width);
    void clearScreen();
    //Tab
    void displayTab();
    void displayMenu();
    void displayMenuforplaylist(void);
    void displayPlayback();
    void displayMessage(const std::string &message);
    void displayMediaFiles(const std::vector<MediaFile> &files, int page, int pagesize ,int totalpages);
    void displayPlaylists(const std::vector<Playlist> &playlists, int page, int page_size, int totalpages);
    void displayMetadata(const MediaFile &mediaFile);
};

#endif // VIEW_H
