#include "View.h"
#include <iostream>
#include <locale>
#include <codecvt>

std::string View::convertSecondsToTimeString(int totalSeconds) {
    int hours = totalSeconds / 3600;    
    int minutes = (totalSeconds % 3600) / 60;  
    int seconds = totalSeconds % 60;    

    std::ostringstream oss;
    oss //<< std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}
void View::printLine(char c, int width){
    std::string line(width, c);
    std::cout << line << std::endl;
}
//Display Tab bar
void View::displayTab(){
    printLine('=',200);
    std::cout << std::left
    << std::setw(1) << "|"
    << std::setw(33) << "Back[b]" 
    << std::setw(33) << "Home[f]"
    << std::setw(33) << "Music library[r]"
    << std::setw(33) << "Video library[d]"
    << std::setw(33) << "Playlists[y]" 
    << std::setw(33) << "Quit[q]" 
    << std::setw(1) << "|"
    << std::endl;
    printLine('-',200);
}
//Display Menu
void View::displayMenu(void){
    std::cout << std::left
    << std::setw(1) << "|"
    << std::setw(33) << "Remove [t]"
    << std::setw(33) << "Add to playlist[a]"
    << std::setw(33) << "Edit [e]" 
    << std::setw(33) << "Previous page[p]"
    << std::setw(33) << "Goto page[g]"
    << std::setw(33) << "Next page[n]"
    << std::right
    << std::setw(1) << "|"
    << std::endl;
    printLine('-',200);
} 
void View::displayMenuforplaylist(void){
    std::cout << std::left
    << std::setw(4) << "|"
    << std::setw(24) << "Creat playlist [c]"
    << std::setw(24) << "Choose playlist [i]"
    << std::setw(24) << "Remove [t]"
    << std::setw(24) << "Add to playlist[a]"
    << std::setw(24) << "Edit [e]" 
    << std::setw(24) << "Previous page[p]"
    << std::setw(24) << "Goto page[g]"
    << std::setw(24) << "Next page[n]"
    << std::right
    << std::setw(4) << "|"
    << std::endl;
    printLine('-',200);
} 
void View::displayPlayback(void){
    std::cout << std::left
    << std::setw(1) << "|"
    << std::setw(33) << "Previous[j]"
    << std::setw(33) << "Play[x]" 
    << std::setw(33) << "Pause[o]"
    << std::setw(33) << "Next[h]"
    << std::setw(33) << "Repeat[m]"
    << std::setw(33) << "Volume[+][-]"
    << std::right 
    << std::setw(1) << "|"
    << std::endl;
    printLine('=',200);
}   
//Clear screen
void View::clearScreen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the screen and move the cursor to the top-left corner
}  

//Display MediaFiles
void View::displayMediaFiles(const std::vector<MediaFile> &files, int page, int page_size, int totalpages) {
    if(files.size() != 0){
        int start = (page - 1) * page_size;
        int end = std::min(page * page_size, static_cast<int>(files.size()));
        std::cout << std::left
        << std::setw(6)  <<"| Num " 
        << std::setw(53) << "| Title" 
        << std::setw(40) << "| Artist" 
        << std::setw(30) << "| Album" 
        << std::setw(30) << "| Genre" 
        << std::setw(20) << "| Year" 
        << std::setw(20) << "| Duration" 
        << std::right 
        << std::setw(1) << "|"
        << std::endl;
        printLine('-',200);
        for (int i = start; i < end; ++i) {
            //50 characters title
            std::string title = files[i].getTitle();
            if (title.length() > 50) {
                title = title.substr(0, 50);
            }
            std::cout << std::left
            << "| " << std::setw(4) << (i+1) 
            << "| " << std::setw(51) << title  
            << "| " << std::setw(38) << files[i].getArtist() 
            << "| " << std::setw(28) << files[i].getAlbum()
            << "| " << std::setw(28) << files[i].getGenre() 
            << "| " << std::setw(18) << files[i].getYear()
            << "| " << std::setw(18) << convertSecondsToTimeString(files[i].getDuration()) 
            << std::right
            <<std::setw(1) << "|"
            << std::endl;
            printLine('-',200);  
        }
        std::cout << std::left
        << std::setw(90) << "|"
        << std::setw(5) <<"Page "
        << std::right 
        << std::setw(7) << page  
        << std::setw(1) << "/" 
        << std::setw(7) << totalpages  
        << std::setw(90) << "|" 
        << std::endl;
        printLine('-',200);
    }
    else {
        std::cout << std::left
        << std::setw(96) << "|"
        << std::setw(8) << "No file." 
        << std::right
        << std::setw(96) << "|"
        << std::endl;
        printLine('-',200);
    }
}
 
void View::displayMetadata(const MediaFile &mediaFile) {
    std::cout << "Path: " << mediaFile.getPath() << std::endl;
    std::cout << "Title: " << mediaFile.getTitle() << std::endl;
    std::cout << "Artist: " << mediaFile.getArtist() << std::endl;
    std::cout << "Album: " << mediaFile.getAlbum() << std::endl;
    std::cout << "Genre: " << mediaFile.getGenre() << std::endl;
    std::cout << "Year: " << mediaFile.getYear() << std::endl;
}

//Display Playlist list
void View::displayPlaylists(const std::vector<Playlist> &playlists, int page, int page_size, int totalpages) {
    if (playlists.size() == 0){
    std::cout << std::left
        << std::setw(94) << "|"
        << std::setw(12) << "No playlist." 
        << std::right
        << std::setw(94) << "|"
        << std::endl;
    printLine('-',200); 
    }
    else{
        int start = (page - 1) * page_size;
        int end = std::min(page * page_size, static_cast<int>(playlists.size()));
        std::cout << "| " << std::left << std::setw(197) << "Playlists" 
         << "|" 
         << std::endl;
        printLine('-',200);   
        std::cout << std::left
        << std::setw(6)  <<"| Num " 
        << std::setw(183) << "| Playlist" 
        << std::setw(10) << "| Item" 
        << std::right 
        << std::setw(1) << "|"
        << std::endl;
        printLine('-',200);
        for (int i = start; i < end; ++i){
            std::cout    << std::left
            << "| " << std::setw(4) << (i+1)  
            << "| " << std::setw(181) << playlists[i].getName() 
            << "| " << std::setw(8) << playlists[i].getNum()
            << std::right
            <<std::setw(1) << "|"
            << std::endl;
            printLine('-',200); 
        } 
        std::cout << std::left
        << std::setw(90) << "|"
        << std::setw(5) <<"Page "
        << std::right 
        << std::setw(7) << page  
        << std::setw(1) << "/" 
        << std::setw(7) << totalpages  
        << std::setw(90) << "|" 
        << std::endl;
        printLine('-',200);}
}


