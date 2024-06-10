#include "Controller.h"
#include "View.h"
#include <iostream>
Controller::Controller(){}
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
    // Stop any playing music and clean up
    // stopMusic();

    // Quit SDL_Mixer
    Mix_CloseAudio();
    Mix_Quit();

    // Quit SDL
    SDL_Quit();
}
// hook for music
void Controller::MusicFinishedCallbackWrapper()
{
    if (g_playback && g_mediafiles)
    {
        g_playback->FunctionCallback(*g_mediafiles);
    }
}

void Controller::callNextFunc()
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
    actions.push(&Controller::Homeaction);
    currentAction = &Controller::Homeaction;

    while (!ex)
    {
        callNextFunc();
    }
    play_back.StopTimeThread();
}

//---------------------------------------Common actions----------------------------------------
void Controller::commonActions(char command, int page_num)
{
    switch (command)
    {
    case 'g': // go to page n
        std::cout << "Enter page: ";
        std::cin >> page_num;
        gotoPage(page_num);
        break;
    case 'n': // next page
        nextPage();
        break;
    case 'p': // previous page
        backPage();
        break;
    case 't': // remove file in list
        remove();
        break;
    case 'e': // edit metadata of file
        Edit();
        break;
    case 'a': // add file to playlist
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
        //ReloadScreen();
        break;
    case 'l': //repeat all track
        ex_play = true;
        play_back.RepeatSong();
        //ReloadScreen();
        break;
    case 'm': //repeat one track
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
//////----------------------------------------Actions----------------------------------------------
void Controller::Homeaction()
{
    View view;
    current_playlist = 0;
    currentAction = &Controller::Homeaction;
    char command;
    int page_num;
    Home();
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> command;
        switch (command)
        {
        case 'r':
            actions.push(currentAction);
            actions.push(&Controller::Musicaction);
            return;
        case 'd':
            actions.push(currentAction);
            actions.push(&Controller::Videoaction);
            return;
        case 'y':
            actions.push(currentAction);
            actions.push(&Controller::Playlistaction);
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
        callNextFunc();
    }
}
void Controller::Musicaction()
{
    current_playlist = 0;
    currentAction = &Controller::Musicaction;
    Musiclibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> command;
        switch (command)
        {
        case 'f':
            actions.push(currentAction);
            actions.push(&Controller::Homeaction);
            return;
        case 'd':
            actions.push(currentAction);
            actions.push(&Controller::Videoaction);
            return;
        case 'y':
            actions.push(currentAction);
            actions.push(&Controller::Playlistaction);
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
        callNextFunc();
    }
}
void Controller::Videoaction()
{
    current_playlist = 0;
    currentAction = &Controller::Videoaction;
    Videolibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> command;
        switch (command)
        {
        case 'f':
            actions.push(currentAction);
            actions.push(&Controller::Homeaction);
            return;
        case 'r':
            actions.push(currentAction);
            actions.push(&Controller::Musicaction);
            return;
        case 'y':
            actions.push(currentAction);
            actions.push(&Controller::Playlistaction);
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
        callNextFunc();
    }
}
void Controller::Playlistaction()
{
    current_playlist = 0;
    currentAction = &Controller::Playlistaction;
    Playlists();
    char command;
    int page_num;
    Model model;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> command;
        switch (command)
        {
        case 'f':
            actions.push(currentAction);
            actions.push(&Controller::Homeaction);
            return;
        case 'r':
            actions.push(currentAction);
            actions.push(&Controller::Musicaction);
            return;
        case 'd':
            actions.push(currentAction);
            actions.push(&Controller::Videoaction);
            return;
        case 'i':
            actions.push(currentAction);
            if (playlists.size() != 0)
            {
                std::cout << "Choose your playlist: ";
                std::cin >> current_playlist;
            }
            if ((current_playlist > 0) && (current_playlist <= playlists.size()))
            {
                actions.push(&Controller::PlaylistLibaction);
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
        callNextFunc();
    }
}
void Controller::PlaylistLibaction()
{
    currentAction = &Controller::PlaylistLibaction;
    Playlistlibrary();
    char command;
    int page_num;
    View view;
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> command;
        switch (command)
        {
        case 'f':
            actions.push(currentAction);
            actions.push(&Controller::Homeaction);
            return;
        case 'r':
            actions.push(currentAction);
            actions.push(&Controller::Musicaction);
            return;
        case 'd':
            actions.push(currentAction);
            actions.push(&Controller::Videoaction);
            return;
        case 'y':
            actions.push(currentAction);
            actions.push(&Controller::Playlistaction);
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
        callNextFunc();
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
        if (currentAction == &Controller::Homeaction)
        {
            view.displayMediaFiles(mediafiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentAction == &Controller::Musicaction)
        {
            view.displayMediaFiles(musicfiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentAction == &Controller::Videoaction)
        {
            view.displayMediaFiles(videofiles, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentAction == &Controller::Playlistaction)
        {
            view.displayPlaylists(playlists, page_num, PAGE_SIZE, totalPages);
        }
        else if (currentAction == &Controller::PlaylistLibaction)
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
            std::cin >> choice;
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
    if (currentAction == &Controller::Homeaction)
    {
        addfiletoPlaylist(mediafiles);
    }
    else if (currentAction == &Controller::Musicaction)
    {
        addfiletoPlaylist(musicfiles);
    }
    else if (currentAction == &Controller::Videoaction)
    {
        addfiletoPlaylist(videofiles);
    }
    else if (currentAction == &Controller::Playlistaction)
    {
        addplaylisttoplaylist();
    }
    else if (currentAction == &Controller::PlaylistLibaction)
    {
        addfiletoPlaylist(playlists[current_playlist - 1].getFiles());
    }
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
            std::cin >> file_num;
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
                    std::cin >> playlist_num;
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
                                std::cin >> choice3;
                                std::cin.ignore();
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
                            std::cin >> choice1;
                            std::cin.ignore();
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
                std::cin >> choice;
                std::cin.ignore();
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after removing file
        ReloadScreen();
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
            std::cin >> file_num;
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
                std::cin >> playlist_num;
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
                    std::cin >> choice1;
                    std::cin.ignore();
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
                std::cin >> choice;
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after removing file
        ReloadScreen();
    }
}

//---------------------------------Method remove: remove playlist, mediafile-------------------------------
void Controller::remove()
{
    if (currentAction == &Controller::Homeaction)
    {
        removeMediafile(mediapaths);
    }
    else if (currentAction == &Controller::Musicaction)
    {
        removeMediafile(musicpaths);
    }
    else if (currentAction == &Controller::Videoaction)
    {
        removeMediafile(videopaths);
    }
    else if (currentAction == &Controller::Playlistaction)
    {
        removePlaylist();
    }
    else if (currentAction == &Controller::PlaylistLibaction)
    {
        removeMediafile(playlists[current_playlist - 1].getFiles());
    }
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
            std::cin >> num;
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
                std::cin >> choice;
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after removing file
        ReloadScreen();
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
            std::cin >> num;
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
                std::cin >> choice;
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after removing playlist
        ReloadScreen();
    }
}

//-----------------------------Method edit: edit metadata, rename playlist------------------------------------
void Controller::Edit()
{
    if (currentAction == &Controller::Homeaction)
    {
        editMetadata(mediafiles);
    }
    else if (currentAction == &Controller::Musicaction)
    {
        editMetadata(musicfiles);
    }
    else if (currentAction == &Controller::Videoaction)
    {
        editMetadata(videofiles);
    }
    else if (currentAction == &Controller::Playlistaction)
    {
        renamePlaylist();
    }
    else if (currentAction == &Controller::PlaylistLibaction)
    {
        editMetadata(playlists[current_playlist - 1].getFiles());
    }
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
            std::cin >> num;
            // Check num
            if ((num <= files.size()) && (num > 0))
            {
                std::cout << "Enter new title (current: " << files[num - 1].getTitle() << "): ";
                std::getline(std::cin >> std::ws, title);
                std::cout << "Enter new artist (current: " << files[num - 1].getArtist() << "): ";
                std::getline(std::cin, artist);
                std::cout << "Enter new album (current: " << files[num - 1].getAlbum() << "): ";
                std::getline(std::cin, album);
                std::cout << "Enter new genre (current: " << files[num - 1].getGenre() << "): ";
                std::getline(std::cin, genre);
                std::cout << "Enter new year (current: " << files[num - 1].getYear() << "): ";
                std::cin >> year;
                if (!title.empty())
                    files[num - 1].setTitle(title);
                if (!artist.empty())
                    files[num - 1].setArtist(artist);
                if (!album.empty())
                    files[num - 1].setAlbum(album);
                if (!genre.empty())
                    files[num - 1].setGenre(genre);
                files[num - 1].setYear(year);
                model.editMediaFileMetadata(files[num - 1]);
                break;
            }
            else
            {
                char choice;
                std::cout << "Invalid choice." << std::endl;
                std::cout << "Do you want to continue to edit? (y/n)" << std::endl;
                std::cin >> choice;
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after editing file
        ReloadScreen();
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
                    std::cin >> choice;
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
                std::cin >> choice;
                if (choice == 'n')
                {
                    break;
                }
            }
        }
        // ReloadScreen after editing file
        ReloadScreen();
    }
}
//------------------------Method with playback----------------------------------
// Play media file or playlist
void Controller::Play()
{
    if (currentAction == &Controller::Homeaction)
    {   
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(mediafiles);
        if(new_tab){
            preAction = currentAction;
            new_tab = false; 
        }
    }
    else if (currentAction == &Controller::Musicaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(musicfiles);
        if(new_tab){
            preAction = currentAction;
            new_tab = false; 
        }
    }
    else if (currentAction == &Controller::Videoaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(videofiles);
        if(new_tab){
            preAction = currentAction;
            new_tab = false; 
        }
    }
    else if (currentAction == &Controller::Playlistaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        PlayAll();
    }
    else if (currentAction == &Controller::PlaylistLibaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        new_tab = play_back.PlayMedia(playlists[current_playlist - 1].getFiles());
        if(new_tab){
            curPlist = current_playlist;
            preAction = currentAction;
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
            if(new_tab){
                preAction = currentAction;
                new_tab = false; 
            }
        }
    }
}
// Next mediafile
void Controller::Next()
{
    if (preAction == &Controller::Homeaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(mediafiles);
    }
    else if (preAction == &Controller::Musicaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(musicfiles);
    }
    else if (preAction == &Controller::Videoaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(videofiles);
    }
    else if ((preAction == &Controller::Playlistaction) || (preAction == &Controller::PlaylistLibaction))
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.NextMedia(playlists[curPlist - 1].getFiles());
    }
}
// Previous mediafile
void Controller::Previous()
{
    if (preAction == &Controller::Homeaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(mediafiles);
    }
    else if (preAction == &Controller::Musicaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(musicfiles);
    }
    else if (preAction == &Controller::Videoaction)
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(videofiles);
    }
    else if ((preAction == &Controller::Playlistaction) || (preAction == &Controller::PlaylistLibaction))
    {
        play_back.StopTimeThread();
        ReloadScreen();
        play_back.PreviousMedia(playlists[curPlist- 1].getFiles());
    }
}
// Reload screen
void Controller::ReloadScreen()
{
    if (currentAction == &Controller::Homeaction)
    {
        Home();
    }
    else if (currentAction == &Controller::Musicaction)
    {
        Musiclibrary();
    }
    else if (currentAction == &Controller::Videoaction)
    {
        Videolibrary();
    }
    else if (currentAction == &Controller::Playlistaction)
    {
        Playlists();
    }
    else if (currentAction == &Controller::PlaylistLibaction)
    {
        Playlistlibrary();
    }
}