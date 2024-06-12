#include "Controller.h"
#include "View.h"
#include <iostream>
Controller::Controller() {}
PlayBack *Controller::g_playback = nullptr;
std::vector<MediaFile> *Controller::g_mediafiles = nullptr;
// Contructor
Controller::Controller(const std::string &baseDir) : baseDir(baseDir), currentPage(1), totalPages(1)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        // Handle SDL initialization error
    }
    // Initialize SDL_Mixer and check for errors
    int mix_flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
    int initialized_flags = Mix_Init(mix_flags);
    if ((initialized_flags & mix_flags) != mix_flags)
    {
        std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
        // Handle SDL_Mixer initialization error
    }

    // Initialize SDL_Mixer audio settings
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        // Handle SDL_Mixer audio initialization error
    }
}
// Destructor
Controller::~Controller()
{
    // Quit SDL_Mixer
    Mix_CloseAudio();
    Mix_Quit();

    // Quit SDL
    SDL_Quit();
}
//---------------------------------------Input-----------------------------------------------------
void Controller::EnterCharInput(char& command){
    while (true) {
        std::cin >> command;

        // Check if the input was valid and is a single character
        if (std::cin.fail() || std::cin.peek() != '\n') {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a single character." << std::endl;
        } else {
            break; // Exit loop if input is valid
        }
    }
}

void Controller:: EnterIntegerInput(int& num) {
    while (true) {
        std::cin >> num;
        if (std::cin.fail() || num <= 0) { 
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. Please enter a positive integer." << std::endl;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            break; 
        }
    }
}
//------------------------------Hook for music------------------------------------
void Controller::MusicFinishedCallbackWrapper()
{
    if (g_playback && g_mediafiles)
    {
        g_playback->FunctionCallback(*g_mediafiles);
    }
}
//--------------------------------------Run Tab-------------------------------------------
void Controller::RunTab()
{
    if (!actions.empty())
    {
        Action nextAction = actions.top();
        actions.pop();
        (this->*nextAction)();
    }
}
void Controller::getfilelist(std::vector<std::string> filepaths, std::vector<MediaFile> &filelist)
{
    Model model;
    // Clear filelist
    filelist.clear();
    // Get metadata from filepath
    if (!filepaths.empty())
    {
        for (int i = 0; i < filepaths.size(); i++)
        {
            model.getMetadata(filepaths[i], filelist);
        }
    }
}

////////////////////////////////////////Main process/////////////////////////////////////////////
void Controller::run()
{
    g_playback = &play_back;
    g_mediafiles = &mediafiles;
    Mix_HookMusicFinished(MusicFinishedCallbackWrapper);
    Model model;
    View view;
    mediapaths = model.getMediaFilepaths(baseDir);
    musicpaths = model.getMusicFilepaths(baseDir);
    videopaths = model.getVideoFilepaths(baseDir);
    actions.push(&Controller::HomeTab
);
    currentTab = &Controller::HomeTab
;

    while (!ex)
    {
        RunTab();
    }
    play_back.StopTimeThread();
}

//---------------------------------------Common actions----------------------------------------
void Controller::commonActions(char command, int page_num)
{
    switch (command)
    {
    case 'g': // go to page n
        play_back.StopTimeThread();
        std::cout << "Enter page: ";
        EnterIntegerInput(page_num);
        gotoPage(page_num);
        break;
    case 'n': // next page
        nextPage();
        break;
    case 'p': // previous page
        backPage();
        break;
    case 't': // remove file in list
        play_back.StopTimeThread();
        remove();
        break;
    case 'e': // edit metadata of file
        play_back.StopTimeThread();
        Edit();
        break;
    case 'a': // add file to playlist
        play_back.StopTimeThread();
        addtoPlaylist();
        break;
    default:
        if (!ex_play)
        {
            std::cout << "Unknown command" << std::endl;
        }
        break;
    }
}

//---------------------------------------Playback actions------------------------------------------
void Controller::playbackActions(char command)
{
    // Implement selection in playback: resume, pause, next, previous
    switch (command)
    {
    case 'x':
        ex_play = true;
        Play();
        break;
    case 'o': // pause music
        ex_play = true;
        play_back.PauseMusic();
        // ReloadScreen();
        break;
    case 'l': // repeat all track
        ex_play = true;
        play_back.RepeatSong();
        // ReloadScreen();
        break;
    case 'm': // repeat one track
        ex_play = true;
        play_back.AllSong();
        break;
    case 'k': // resume music
        ex_play = true;
        play_back.ResumeMusic();
        ReloadScreen();
        break;
    case 'h': // next track
        ex_play = true;
        Next();
        break;
    case 'j': // previous Track
        ex_play = true;
        Previous();
        break;
    case '+':
        ex_play = true;
        play_back.VolumeUp();
        break;
    case '-':
        ex_play = true;
        play_back.VolumeDown();
        break;
    default:
        break;
    }
}
//////----------------------------------------Tabs----------------------------------------------
void Controller::HomeTab()
{
    View view;
    current_playlist = 0;
    currentTab = &Controller::HomeTab
;
    char command;
    int page_num;
    Home();
    while (true)
    {
        std::cout << "Enter command: ";
        EnterCharInput(command);
        switch (command)
        {
        case 'r':
            actions.push(currentTab);
            actions.push(&Controller::MusicTab);
            return;
        case 'd':
            actions.push(currentTab);
            actions.push(&Controller::VideoTab);
            return;
        case 'y':
            actions.push(currentTab);
            actions.push(&Controller::PlaylistTab);
            return;
        case 'b': // return previous action
            if (!actions.empty())
            {
                return;
            }
            else
            {
                break;
            }
        case 'q': // Quit
            ex = true;
            g_playback = NULL;
            play_back.StopTimeThread();
            return;
        default:
            playbackActions(command);
            commonActions(command, page_num);
            break;
        }
    }
    if (ex != true)
    {
        RunTab();
    }
}
void Controller::MusicTab()
{
    current_playlist = 0;
    currentTab = &Controller::MusicTab;
    Musiclibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        EnterCharInput(command);
        switch (command)
        {
        case 'f':
            actions.push(currentTab);
            actions.push(&Controller::HomeTab
        );
            return;
        case 'd':
            actions.push(currentTab);
            actions.push(&Controller::VideoTab);
            return;
        case 'y':
            actions.push(currentTab);
            actions.push(&Controller::PlaylistTab);
            return;
        case 'b': // return previous action
            if (!actions.empty())
            {
                return;
            }
            else
            {
                break;
            }
            play_back.VolumeDown();
            break;
        case 'q': // Quit
            ex = true;
            g_playback = NULL;
            play_back.StopTimeThread();
            return;
        default:
            playbackActions(command);
            commonActions(command, page_num);
            break;
        }
    }
    if (ex != true)
    {
        RunTab();
    }
}
void Controller::VideoTab()
{
    current_playlist = 0;
    currentTab = &Controller::VideoTab;
    Videolibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        EnterCharInput(command);
        switch (command)
        {
        case 'f':
            actions.push(currentTab);
            actions.push(&Controller::HomeTab
        );
            return;
        case 'r':
            actions.push(currentTab);
            actions.push(&Controller::MusicTab);
            return;
        case 'y':
            actions.push(currentTab);
            actions.push(&Controller::PlaylistTab);
            return;
        case 'b': // return previous action
            if (!actions.empty())
            {
                return;
            }
            else
            {
                break;
            }
        case 'q': // Quit
            ex = true;
            g_playback = NULL;
            play_back.StopTimeThread();
            return;
        default:
            playbackActions(command);
            commonActions(command, page_num);
            break;
        }
    }
    if (ex != true)
    {
        RunTab();
    }
}
void Controller::PlaylistTab()
{
    current_playlist = 0;
    currentTab = &Controller::PlaylistTab;
    Playlists();
    char command;
    int page_num;
    Model model;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        EnterCharInput(command);
        switch (command)
        {
        case 'f':
            actions.push(currentTab);
            actions.push(&Controller::HomeTab
        );
            return;
        case 'r':
            actions.push(currentTab);
            actions.push(&Controller::MusicTab);
            return;
        case 'd':
            actions.push(currentTab);
            actions.push(&Controller::VideoTab);
            return;
        case 'i':
            actions.push(currentTab);
            if (playlists.size() != 0)
            {
                std::cout << "Choose your playlist: ";
                EnterIntegerInput(current_playlist);
            }
            if ((current_playlist > 0) && (current_playlist <= playlists.size()))
            {
                actions.push(&Controller::PlaylistLibTab);
            }
            else
            {
                std::cout << "No playlist." << std::endl;
            }
            return;
        case 'c':
            createPlaylist();
            break;
        case 'b': // return previous action
            if (!actions.empty())
            {
                return;
            }
            else
            {
                break;
            }
        case 'q': // Quit
            ex = true;
            g_playback = NULL;
            play_back.StopTimeThread();
            return;
        default:
            playbackActions(command);
            commonActions(command, page_num);
            break;
        }
    }
    if (ex != true)
    {
        RunTab();
    }
}
void Controller::PlaylistLibTab()
{
    currentTab = &Controller::PlaylistLibTab;
    Playlistlibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        EnterCharInput(command);
        switch (command)
        {
        case 'f':
            actions.push(currentTab);
            actions.push(&Controller::HomeTab
        );
            return;
        case 'r':
            actions.push(currentTab);
            actions.push(&Controller::MusicTab);
            return;
        case 'd':
            actions.push(currentTab);
            actions.push(&Controller::VideoTab);
            return;
        case 'y':
            actions.push(currentTab);
            actions.push(&Controller::PlaylistTab);
            return;
        case 'b': // return previous action
            if (!actions.empty())
            {
                return;
            }
            else
            {
                break;
            }
        case 'q': // Quit
            ex = true;
            g_playback = NULL;
            play_back.StopTimeThread();
            return;
        default:
            playbackActions(command);
            commonActions(command, page_num);
            break;
        }
    }
    if (ex != true)
    {
        RunTab();
    }
}

//------------------------------------Method to take list of media, music, video----------------------------
void Controller::Home()
{
    // Get mediafile list
    getfilelist(mediapaths, mediafiles);
    View view;
    currentPage = 1;
    if (mediafiles.empty())
    {
        totalPages = 0;
    }
    else
    {
        totalPages = 1 + (mediafiles.size() - 1) / PAGE_SIZE;
    }
    view.clearScreen();
    view.displayTab();
    view.displayMenu();

    view.displayMediaFiles(mediafiles, currentPage, PAGE_SIZE, totalPages);
    view.displayPlayback();
}
void Controller::Musiclibrary()
{
    // Get musicfile list
    getfilelist(musicpaths, musicfiles);
    View view;
    currentPage = 1;
    if (musicfiles.empty())
    {
        totalPages = 0;
    }
    else
    {
        totalPages = 1 + (musicfiles.size() - 1) / PAGE_SIZE;
    }
    view.clearScreen();
    view.displayTab();
    view.displayMenu();
    view.displayMediaFiles(musicfiles, currentPage, PAGE_SIZE, totalPages);
    view.displayPlayback();
}
void Controller::Videolibrary()
{
    // Get videofile list
    getfilelist(videopaths, videofiles);
    View view;
    currentPage = 1;
    if (videofiles.empty())
    {
        totalPages = 0;
    }
    else
    {
        totalPages = 1 + (videofiles.size() - 1) / PAGE_SIZE;
    }
    view.clearScreen();
    view.displayTab();
    view.displayMenu();
    view.displayMediaFiles(videofiles, currentPage, PAGE_SIZE, totalPages);
    view.displayPlayback();
}
void Controller::Playlists()
{
    View view;
    currentPage = 1;
    if (playlists.empty())
    {
        totalPages = 0;
    }
    else
    {
        totalPages = 1 + (playlists.size() - 1) / PAGE_SIZE;
    }
    // List empty => creat playlist
    view.clearScreen();
    view.displayTab();
    view.displayMenuforplaylist();
    view.displayPlaylists(playlists, currentPage, PAGE_SIZE, totalPages);
    view.displayPlayback();
}
void Controller::Playlistlibrary()
{
    View view;
    currentPage = 1;
    if (playlists[current_playlist - 1].getNum() == 0)
    {
        totalPages = 0;
    }
    else
    {
        totalPages = 1 + (playlists[current_playlist - 1].getNum() - 1) / PAGE_SIZE;
    }
    view.clearScreen();
    view.displayTab();
    view.displayMenu();
    std::cout << std::left
              << std::setw(10) << "|"
              << std::setw(189) << playlists[current_playlist - 1].getName()
              << "|"
              << std::endl;
    view.printLine('-', 200);
    view.displayMediaFiles(playlists[current_playlist - 1].getFiles(), currentPage, PAGE_SIZE, totalPages);
    view.displayPlayback();
}

//-----------------------------------------Method with page-----------------------------------------
void Controller::gotoPage(int page_num)
{
    Model model;
    View view;
    if ((1 <= page_num) && (page_num <= totalPages))
    {
        currentPage = page_num;
        view.clearScreen();
        view.displayTab();
        view.displayMenu();
        if (currentTab == &Controller::HomeTab
    )
        {
            view.displayMediaFiles(mediafiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentTab == &Controller::MusicTab)
        {
            view.displayMediaFiles(musicfiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentTab == &Controller::VideoTab)
        {
            view.displayMediaFiles(videofiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentTab == &Controller::PlaylistTab)
        {
            view.displayPlaylists(playlists, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentTab == &Controller::PlaylistLibTab)
        {
            view.displayMediaFiles(playlists[current_playlist - 1].getFiles(), page_num, PAGE_SIZE, totalPages);
        }
        view.displayPlayback();
    }
    else
    {
        std::cout << "No page." << std::endl;
    }
}
void Controller::nextPage()
{
    if (currentPage < totalPages)
    {
        currentPage++;
        gotoPage(currentPage);
    }
    else
    {
        std::cout << "No more pages." << std::endl;
    }
}
void Controller::backPage()
{
    if (currentPage > 1)
    {
        currentPage--;
        gotoPage(currentPage);
    }
    else
    {
        std::cout << "No more pages." << std::endl;
    }
}
//-------------------------------------Creat new playlist-------------------------------------------
void Controller::createPlaylist()
{
    std::cout << "Creat new playlist" << std::endl;
    std::string name;
    Playlist playlist;
    while (true)
    {
        bool exist = false;
        std::cout << "Enter name of playlist: ";
        std::cin.ignore();
        std::getline(std::cin, name);
        // Check same playlist
        if (!playlists.empty())
        {
            for (int i = 0; i < playlists.size(); i++)
            {
                if (playlists[i].getName() == name)
                {
                    exist = true;
                    break;
                }
            }
        }
        if (exist != true)
        {
            playlist.setName(name);
            playlists.push_back(playlist);
            std::cout << "Creat new playlist success." << std::endl;
            break;
        }
        else
        {
            char choice;
            std::cout << "Same name with existed playlist." << std::endl;
            std::cout << "Do you want to continute creat new playlist? (y/n): ";
            EnterCharInput(choice);
            if (choice == 'n')
            {
                break;
            }
        }
    }
    ReloadScreen();
}

//-------------------------------------Add media file to playlist-----------------------------------------
void Controller::addtoPlaylist()
{
    if (currentTab == &Controller::HomeTab
)
    {
        addfiletoPlaylist(mediafiles);
    }
    else if (currentTab == &Controller::MusicTab)
    {
        addfiletoPlaylist(musicfiles);
    }
    else if (currentTab == &Controller::VideoTab)
    {
        addfiletoPlaylist(videofiles);
    }
    else if (currentTab == &Controller::PlaylistTab)
    {
        addplaylisttoplaylist();
    }
    else if (currentTab == &Controller::PlaylistLibTab)
    {
        addfiletoPlaylist(playlists[current_playlist - 1].getFiles());
    }
    // ReloadScreen after removing playlist
    ReloadScreen();
}
// Add media file to playlist
void Controller::addfiletoPlaylist(std::vector<MediaFile> &files)
{
    if (!files.empty())
    {
        int file_num;
        int playlist_num;
        while (true)
        {
            bool existfile = false;
            // Choose file to add
            std::cout << "Choose file to add: ";
            EnterIntegerInput(file_num);
            // check file_num
            if ((0 < file_num) && (file_num <= files.size()))
            {
                // No play list then make new playlist
                if (playlists.empty())
                {
                    std::cout << "No playlist." << std::endl;
                    createPlaylist();
                    playlist_num = 1;
                    // Add file to playlist
                    playlists[playlist_num - 1].addFile(files[file_num - 1]);
                    std::cout << "Add file success." << std::endl;
                    break;
                }
                else
                {
                    // Available playlist of make new playlist option
                    int i;
                    // Display playlists options
                    std::cout << "Playlists:" << std::endl;
                    for (i = 0; i < playlists.size(); i++)
                    {
                        std::cout << (i + 1) << ". " << playlists[i].getName() << std::endl;
                    }
                    std::cout << (i + 1) << ". " << "New playlist" << std::endl;
                    // Choose option to add
                    std::cout << "Choose playlist to add: ";
                    EnterIntegerInput(playlist_num);
                    // Check options
                    // Option creat new playlist
                    if (playlist_num == (i + 1))
                    {
                        createPlaylist();
                        if (playlists.size() != (i + 1))
                        {
                            break;
                        }
                        else
                        {
                            // Add file to playlist
                            playlists[playlist_num - 1].addFile(files[file_num - 1]);
                            std::cout << "Add file success." << std::endl;
                            break;
                        }
                    }
                    else
                    {
                        // Option available playlist
                        if ((playlist_num <= playlists.size()) && (playlist_num > 0))
                        {
                            // Check file in playlist
                            if (playlists[playlist_num - 1].getNum() != 0)
                            {
                                for (int i = 0; i < playlists[playlist_num - 1].getNum(); i++)
                                {
                                    if (playlists[playlist_num - 1].getFile(i).getPath() == files[file_num - 1].getPath())
                                    {
                                        existfile = true;
                                        break;
                                    }
                                }
                            }
                            // Add file to playlist
                            if (!existfile)
                            {
                                playlists[playlist_num - 1].addFile(files[file_num - 1]);
                                std::cout << "Add file success." << std::endl;
                                break;
                            }
                            else
                            {
                                char choice3;
                                std::cout << "Exist file in playlist." << std::endl;
                                std::cout << "Do you want to continute to add file to playlist? (y/n): ";
                                EnterCharInput(choice3);
                                if (choice3 == 'n')
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // Number of playlist is not correct
                            char choice1;
                            std::cout << "Invalid choice." << std::endl;
                            std::cout << "Do you want to continute to add file to playlist? (y/n): ";
                            EnterCharInput(choice1);
                            if (choice1 == 'n')
                            {
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                // Number of file is not correct
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continute to add file to playlist? (y/n): ";
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}
// Add playlist to playlist
void Controller::addplaylisttoplaylist()
{
    if (playlists.size() > 1)
    {
        int file_num;
        int playlist_num;
        while (true)
        {
            bool existfile = false;
            int j;
            // Choose file to add
            std::cout << "Choose file to add: ";
            EnterIntegerInput(file_num);
            // check file_num
            if ((0 < file_num) && (file_num <= playlists.size()))
            {
                // Available playlist of make new playlist option
                int i;
                // Display playlists options
                std::cout << "Playlists:" << std::endl;
                for (i = 0; i < playlists.size(); i++)
                {
                    std::cout << (i + 1) << ". " << playlists[i].getName() << std::endl;
                }
                std::cout << (i + 1) << ". " << "New playlist" << std::endl;
                // Choose option to add
                std::cout << "Choose playlist to add: ";
                EnterIntegerInput(playlist_num);
                // Check options
                // Option creat new playlist
                if (playlist_num == (i + 1))
                {
                    createPlaylist();
                    if (playlists.size() != (i + 1))
                    {
                        break;
                    }
                    else
                    {
                        // Add all file in playlist to other playlist
                        for (int i = 0; i < playlists[file_num - 1].getNum(); i++)
                        {
                            playlists[playlist_num - 1].addFile(playlists[file_num - 1].getFile(i));
                        }
                        std::cout << "Add file success." << std::endl;
                        break;
                    }
                }
                // Option available playlist
                if ((playlist_num <= playlists.size()) && (playlist_num > 0) && (file_num != playlist_num))
                {
                    // //Check file in playlist and save if correct
                    if (!existfile)
                    {
                        for (int i = 0; i < playlists[file_num - 1].getNum(); i++)
                        {
                            existfile = false;
                            for (j = 0; j < playlists[playlist_num - 1].getNum(); j++)
                            {
                                if (playlists[file_num - 1].getFile(i).getPath() == playlists[playlist_num - 1].getFile(j).getPath())
                                {
                                    existfile = true;
                                    std::cout << "File exist: " << playlists[file_num - 1].getFile(i).getTitle() << std::endl;
                                    break;
                                }
                            }
                            // Add file to playlist
                            if (existfile != true)
                            {
                                playlists[playlist_num - 1].addFile(playlists[file_num - 1].getFile(i));
                                std::cout << "Add file success." << std::endl;
                            }
                        }
                        break;
                    }
                }
                else
                {
                    // Number of playlist is not correct
                    char choice1;
                    std::cout << "Invalid choice." << std::endl;
                    std::cout << "Do you want to continute to add file to playlist? (y/n): ";
                    EnterCharInput(choice1);
                    if (choice1 == 'n')
                    {
                        break;
                    }
                }
            }
            else
            {
                // Number of file is not correct
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continute to add playlist to playlist? (y/n): ";
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}

//---------------------------------Method remove: remove playlist, mediafile-------------------------------
void Controller::remove()
{
    if (currentTab == &Controller::HomeTab
)
    {
        removeMediafile(mediapaths);
    }
    else if (currentTab == &Controller::MusicTab)
    {
        removeMediafile(musicpaths);
    }
    else if (currentTab == &Controller::VideoTab)
    {
        removeMediafile(videopaths);
    }
    else if (currentTab == &Controller::PlaylistTab)
    {
        removePlaylist();
    }
    else if (currentTab == &Controller::PlaylistLibTab)
    {
        removeMediafile(playlists[current_playlist - 1].getFiles());
    }
    // ReloadScreen after removing playlist
    ReloadScreen();   
}
// remove mediafile
template <typename T>
void Controller::removeMediafile(std::vector<T> &files)
{
    if (!files.empty())
    {
        int num;
        while (true)
        {
            std::cout << "Choose num of mediafile to remove: ";
            EnterIntegerInput(num);
            if ((0 < num) && (num <= files.size()))
            {
                files.erase(files.begin() + num - 1);
                break;
            }
            else
            {
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continue to remove? (y/n)" << std::endl;
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}
// Remove playlist
void Controller::removePlaylist()
{
    if (!playlists.empty())
    {
        int num;
        while (true)
        {
            std::cout << "Choose num of playlist to delete: ";
            EnterIntegerInput(num);
            // Check num
            if ((num <= playlists.size()) && (num > 0))
            {
                // Delete all file in playlist
                playlists[num - 1].deleteFile();
                // Delete playlist
                playlists.erase(playlists.begin() + num - 1);
                break;
            }
            else
            {
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continue to remove? (y/n)" << std::endl;
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}

//-----------------------------Method edit: edit metadata, rename playlist------------------------------------
void Controller::Edit()
{
    if (currentTab == &Controller::HomeTab
)
    {
        editMetadata(mediafiles);
    }
    else if (currentTab == &Controller::MusicTab)
    {
        editMetadata(musicfiles);
    }
    else if (currentTab == &Controller::VideoTab)
    {
        editMetadata(videofiles);
    }
    else if (currentTab == &Controller::PlaylistTab)
    {
        renamePlaylist();
    }
    else if (currentTab == &Controller::PlaylistLibTab)
    {
        editMetadata(playlists[current_playlist - 1].getFiles());
    }
    // ReloadScreen after removing playlist
    ReloadScreen();
}
// Edit metadata
void Controller::editMetadata(std::vector<MediaFile> &files)
{
    if (!files.empty())
    {
        Model model;
        int num;
        std::string title, artist, album, genre;
        int year;
        while (true)
        {
            std::cout << "Choose file to edit metadata: ";
            EnterIntegerInput(num);
            // Check num
            if ((num <= files.size()) && (num > 0))
            {
            char choice;
            while (true) {
                std::cout << "Select the metadata field to edit:\n";
                std::cout << "1. Title\n";
                std::cout << "2. Artist\n";
                std::cout << "3. Album\n";
                std::cout << "4. Genre\n";
                std::cout << "5. Year\n";
                std::cout << "6. Done\n";
                std::cout << "Enter choice: ";
                EnterCharInput(choice);
                std::string inputStr;
                int inputInt;
                switch (choice) {
                    case '1':
                        std::cout << "Enter new title (current: " << files[num - 1].getTitle() << "): ";
                        std::getline(std::cin >> std::ws, inputStr);
                        if (!inputStr.empty())
                            files[num - 1].setTitle(inputStr);
                        break;
                    case '2':
                        std::cout << "Enter new artist (current: " << files[num - 1].getArtist() << "): ";
                        std::getline(std::cin >> std::ws, inputStr);
                        if (!inputStr.empty())
                            files[num - 1].setArtist(inputStr);
                        break;
                    case '3':
                        std::cout << "Enter new album (current: " << files[num - 1].getAlbum() << "): ";
                        std::getline(std::cin >> std::ws, inputStr);
                        if (!inputStr.empty())
                            files[num - 1].setAlbum(inputStr);
                        break;
                    case '4':
                        std::cout << "Enter new genre (current: " << files[num - 1].getGenre() << "): ";
                        std::getline(std::cin >> std::ws, inputStr);
                        if (!inputStr.empty())
                            files[num - 1].setGenre(inputStr);
                        break;
                    case '5':
                        std::cout << "Enter new year (current: " << files[num - 1].getYear() << "): ";
                        std::cin >> inputInt;
                        if (!std::cin.fail()) // Basic validation
                            files[num - 1].setYear(inputInt);
                        std::cin.clear(); // Clear any error flags
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                        break;
                    case '6':
                        model.editMediaFileMetadata(files[num - 1]);
                        return; // Exit edit
                    default:
                        std::cout << "Invalid choice. Please select again." << std::endl;
                        break;
                }
            }
                model.editMediaFileMetadata(files[num - 1]);
                break;
            }
            else
            {
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continue to edit? (y/n)" << std::endl;
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}
// Rename playlist
void Controller::renamePlaylist()
{
    if (playlists.size() != 0)
    {
        int num;
        while (true)
        {
            bool exist = false;
            std::cout << "Enter num of playlist to rename: ";
            std::cin >> num;
            // Check number of playlist
            if ((num <= playlists.size()) && (num > 0))
            {
                std::string name;
                // Enter new name
                std::cout << "Enter new name (current: " << playlists[num - 1].getName() << "): ";
                std::cin.ignore();
                std::getline(std::cin, name);
                // Check name of playlist
                for (int i = 0; i < playlists.size(); i++)
                {
                    if (playlists[i].getName() == name)
                    {
                        exist = true;
                        break;
                    }
                }
                if (exist != true)
                {
                    // Save new name for playlist
                    if (!name.empty())
                        playlists[num - 1].setName(name);
                    break;
                }
                else
                {
                    char choice;
                    std::cout << "Same name with existed playlist." << std::endl;
                    std::cout << "Do you want to continute rename playlist? (y/n): ";
                    EnterCharInput(choice);
                    if (choice == 'n')
                    {
                        break;
                    }
                }
            }
            else
            {
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continue to rename playlist? (y/n)" << std::endl;
                EnterCharInput(choice);
                if (choice == 'n')
                {
                    break;
                }
            }
        }
    }
}
//------------------------Method with playback----------------------------------
// Play media file or playlist
void Controller::Play()
{
    if (currentTab == &Controller::HomeTab
)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(mediafiles);
        if (new_tab)
        {
            preTab = currentTab;
            new_tab = false;
        }
    }
    else if (currentTab == &Controller::MusicTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(musicfiles);
        if (new_tab)
        {
            preTab = currentTab;
            new_tab = false;
        }
    }
    else if (currentTab == &Controller::VideoTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(videofiles);
        if (new_tab)
        {
            preTab = currentTab;
            new_tab = false;
        }
    }
    else if (currentTab == &Controller::PlaylistTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        PlayAll();
    }
    else if (currentTab == &Controller::PlaylistLibTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(playlists[current_playlist - 1].getFiles());
        if (new_tab)
        {
            curPlist = current_playlist;
            preTab = currentTab;
            new_tab = false;
        }
    }
}
void Controller::PlayAll()
{
    if (!playlists.empty())
    {
        int num;
        std::cout << "Enter playlist: ";
        std::cin >> num;
        curPlist = num;
        if ((num > 0) && (num <= playlists.size()) && (playlists[num - 1].getNum() != 0))
        {
            play_back.StopTimeThread();
            ReloadScreen();
            new_tab = play_back.PlayMedia(playlists[num - 1].getFiles());
            if (new_tab)
            {
                preTab = currentTab;
                new_tab = false;
            }
        }
    }
}
// Next mediafile
void Controller::Next()
{
    if (preTab == &Controller::HomeTab
)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(mediafiles);
    }
    else if (preTab == &Controller::MusicTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(musicfiles);
    }
    else if (preTab == &Controller::VideoTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(videofiles);
    }
    else if ((preTab == &Controller::PlaylistTab) || (preTab == &Controller::PlaylistLibTab))
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(playlists[curPlist - 1].getFiles());
    }
}
// Previous mediafile
void Controller::Previous()
{
    if (preTab == &Controller::HomeTab
)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(mediafiles);
    }
    else if (preTab == &Controller::MusicTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(musicfiles);
    }
    else if (preTab == &Controller::VideoTab)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(videofiles);
    }
    else if ((preTab == &Controller::PlaylistTab) || (preTab == &Controller::PlaylistLibTab))
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(playlists[curPlist - 1].getFiles());
    }
}
// Reload screen
void Controller::ReloadScreen()
{
    if (currentTab == &Controller::HomeTab
)
    {
        Home();
    }
    else if (currentTab == &Controller::MusicTab)
    {
        Musiclibrary();
    }
    else if (currentTab == &Controller::VideoTab)
    {
        Videolibrary();
    }
    else if (currentTab == &Controller::PlaylistTab)
    {
        Playlists();
    }
    else if (currentTab == &Controller::PlaylistLibTab)
    {
        Playlistlibrary();
    }
}