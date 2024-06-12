#ifndef _PLAYBACK_
#define _PLAYBACK_

#include <iostream>
#include "Mediafile.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <ctime>
namespace fs = std::filesystem;
#define current 0;

struct MusicData {
    int currentTrack;
    Mix_Music* music = nullptr;
    int startTime;
    int pausedTime;
};

class PlayBack {
public:
    PlayBack();
    ~PlayBack();
    bool PlayMedia(std::vector<MediaFile>& files);
    void NextMedia(std::vector<MediaFile>& files);
    void PreviousMedia(std::vector<MediaFile>& files);
    bool init();
    void setflag(bool a);
    void playMusic(const std::string &filepath);
    void FunctionCallback(std::vector<MediaFile> songlist);
    void displayCurrentTime(std::string title, int duration);
    void PauseMusic();
    void ResumeMusic();
    void VolumeUp();
    void VolumeDown();
    void setTrack(int track);
    void RepeatSong();
    void AllSong();
    void StartTimeThread(std::string title, int duration);
    void StopTimeThread();
    std::atomic<bool> quitTimeThread;
    bool isPaused = false;
private:
    MusicData music_Data;
    const int Volume_Step = 1;
    bool flag = false;
    bool repeatSingleSong = false;
    int volume = MIX_MAX_VOLUME / 3;
    //std::atomic<bool> quitTimeThread;
    std::condition_variable cv;
    std::mutex cv_m;
    std::thread timeThread;
    std::atomic<bool> timeUpdated;
};

#endif
