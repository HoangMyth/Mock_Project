#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <vector>
#include <stack>
#include "Model.h"
#include <thread>
#include "Playback.h"


class Controller {
public:
    Controller(const std::string &baseDir);
     ~Controller();
     void ReloadScreen();
     void getfilelist(std::vector<std::string> filepaths, std::vector<MediaFile>& filelist);
    //Method to take list of media, music, video
    void Home();                                    
    void Musiclibrary();                            
    void Videolibrary();                            
    void Playlists();                               
    void Playlistlibrary();
    void commonActions(char command, int page_num); 
    void playbackActions(char command);
    void Play();
    void PlayAll(); 
    void Next();   
    void Previous();                 
    //Method with page
    void gotoPage(int page_num);                                //[g]
    void nextPage();                                            //[n]
    void backPage();                                            //[p]
    //Method with playlist
    void createPlaylist();                                      //[c]
    void renamePlaylist(); 
    void addtoPlaylist();                                       //[a]
    void addfiletoPlaylist(std::vector<MediaFile>& files);
    void addplaylisttoplaylist();                           
    void removePlaylist();                           
    //Method with file   
    void remove();                                              //[t]
    void Edit();                                                //[e]
    void editMetadata(std::vector<MediaFile>& files);
    template <typename T>                       
    void removeMediafile(std::vector<T>& files);                           
    //Method  action
    void run();
    void Homeaction();                                          //[f] 
    void Musicaction();                                         //[r]
    void Videoaction();                                         //[d]
    void Playlistaction();                                      //[y]
    void PlaylistLibaction();                                   //[i]
    bool ex = false;
    static void MusicFinishedCallbackWrapper();
private:
    std::string baseDir;
    int currentPage;
    int totalPages;
    int PAGE_SIZE = 3;
    int current_playlist;
    std::vector<std::string> mediapaths;
    std::vector<std::string> musicpaths;
    std::vector<std::string> videopaths;
    std::vector<MediaFile> mediafiles;
    std::vector<MediaFile> musicfiles;
    std::vector<MediaFile> videofiles;
    std::vector<Playlist>  playlists;
    typedef void(Controller::*Action)();
    Action currentAction;
    std::stack<Action> actions; 
    void callNextFunc();
    PlayBack play_back;
    std::thread timeThread;
    bool ex_play = false;
    static PlayBack* g_playback;
    static std::vector<MediaFile>* g_mediafiles ;

};

#endif // CONTROLLER_
