#include "Playback.h"
#include "Controller.h"

PlayBack ::PlayBack() : quitTimeThread(false), timeUpdated(false) {}
PlayBack::~PlayBack() {
    // Clean up resources
    if (music_Data.music != nullptr) {
        Mix_FreeMusic(music_Data.music);
        music_Data.music = nullptr;
    }
    Mix_CloseAudio();
    SDL_Quit();
}
void PlayBack::setTrack(int track){
        this->music_Data.currentTrack =track;
    }
bool PlayBack :: init()

{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void PlayBack :: close()
{
    // Clean up resources
    Mix_CloseAudio();
    SDL_Quit();
}

void PlayBack :: playMusic(const std::string &filepath, std::string title) {
    // Stop if anyway
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    // Free current music
    if (music_Data.music != nullptr) {
        Mix_FreeMusic(music_Data.music);
        music_Data.music = nullptr;
    }
    // Load new music
    music_Data.music = Mix_LoadMUS(filepath.c_str());
    if (music_Data.music == nullptr) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        return;
    }
    // Start playing music
    if (Mix_PlayMusic(music_Data.music, 1) == -1) {
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
        return;
    }
    // Set start time
    music_Data.startTime = SDL_GetTicks();
    std::cout << "Playing: " << music_Data.currentTrack + 1 << ". " << title << std::endl<<std::endl;
    std::cout << std::endl;
}

void PlayBack :: FunctionCallback(std::vector<MediaFile> files) {
    if (!flag) {
        if (repeatSingleSong) {
            playMusic(files[music_Data.currentTrack].getPath(), files[music_Data.currentTrack].getTitle());

        } else {
            music_Data.currentTrack = (music_Data.currentTrack + 1) % files.size();
            playMusic(files[music_Data.currentTrack].getPath(), files[music_Data.currentTrack].getTitle());
        }
    }
}

void PlayBack :: displayCurrentTime() {
    while (!quitTimeThread) {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait_for(lk, std::chrono::seconds(1));

        if (quitTimeThread) break;
        if (music_Data.music != nullptr && Mix_PlayingMusic() && !isPaused) {
            Uint32 currentTimeMs = SDL_GetTicks() - music_Data.startTime;
            int currentTimeSec = currentTimeMs / 1000;
            int minutes = currentTimeSec / 60;
            int seconds = currentTimeSec % 60;
            std::string currentTime = std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
            std::cout << "\033[B\033[F\033[B";
            std::cout << "\rCurrent Time: " << currentTime << std::flush;
            std::cout << "\033[F\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C\033[C"<< std::flush;
        }
    }
}
void PlayBack::PlayMedia(std::vector<MediaFile>& files){
    if(!files.empty()){
        int num;
        std::cout <<"Enter Song: ";
        std::cin >> num;
        isPaused = false; 
        //check num
        if((num > 0) && (num <= files.size())){
            setTrack(num - 1);
            setflag(true);
            playMusic(files[num - 1].getPath(), files[num - 1].getTitle());
            setflag(false);
            //std::cout << (music_Data.currentTrack + 1)  <<". "<<files[num -1].getTitle() << std::endl;
        }
    }
}

void PlayBack::RepeatSong() {
    std::cout << "Mode play all track: ON" << std::endl;
    repeatSingleSong = false;
}

void PlayBack::AllSong() {
    std::cout << "Mode play single track: ON" << std::endl;
    repeatSingleSong = true;
}

void PlayBack::NextMedia(std::vector<MediaFile>& files){
    setflag(true);  
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "\033[F\033[2KNext Track\n";
    std::cout << std::endl;
    //std::cout << "Next Track" << std::endl;
    isPaused = false;  
    music_Data.currentTrack = (music_Data.currentTrack + 1) % files.size();
    playMusic(files[music_Data.currentTrack].getPath(), files[music_Data.currentTrack].getTitle());
    std::cout << (music_Data.currentTrack + 1)  <<". "<< files[music_Data.currentTrack].getTitle() << std::endl;
    setflag(false);
}

void PlayBack::PreviousMedia(std::vector<MediaFile>& files){
    setflag(true);
    std::cout << std::endl;
    std::cout << std::endl;
    // std::cout << "Previous Track" << std::endl;
    std::cout << "\033[F\033[2KPrevious Track\n";
    std::cout << std::endl;
    isPaused = false;
    music_Data.currentTrack = (music_Data.currentTrack - 1 + files.size()) % files.size();
    playMusic(files[music_Data.currentTrack].getPath(), files[music_Data.currentTrack].getTitle());
    std::cout << (music_Data.currentTrack + 1) <<". "<<files[music_Data.currentTrack].getTitle() << std::endl;
    setflag(false);
}

void PlayBack :: setflag(bool a){
    this->flag = a;
    }

void PlayBack :: PauseMusic() {
    std::cout<<std::endl;
    std::cout << "\nMusic Paused\n";
    Mix_PauseMusic();
    music_Data.pausedTime = SDL_GetTicks() - music_Data.startTime;
    isPaused = true;
    }
void PlayBack :: ResumeMusic() {
    std::cout<<std::endl;
    std::cout << "\nMusic Resumed\n";
    Mix_ResumeMusic();
    music_Data.startTime = SDL_GetTicks() - music_Data.pausedTime;
    isPaused = false;
    }
void PlayBack :: VolumeUp(){
    volume = std::min(MIX_MAX_VOLUME, volume + Volume_Step);
    Mix_VolumeMusic(volume);
    std::cout << "Volume Increased: " << volume << "\n";
}
void PlayBack :: VolumeDown(){
    volume = std::max(0, volume - Volume_Step);
    Mix_VolumeMusic(volume);
    std::cout << "Volume Decreased: " << volume << "\n";
}


void PlayBack :: RunMusic(const std::vector<MediaFile> &files) {

    if (!init()) {
        std::cerr << "Failed to initialize" << std::endl;
        return;
    }
    // Get music from directory
    if (files.empty()) {
        std::cerr << "No music found in the directory!" << std::endl;
        close();
        return;
    }
    music_Data.currentTrack = current;
    // Load first track
    music_Data.music = nullptr; // Object Mix_Music often used with other function of SDL_mixer to play music
    playMusic(files[music_Data.currentTrack].getPath(), files[music_Data.currentTrack].getTitle());
    Mix_VolumeMusic(volume); // Set initial volume

    close();
}