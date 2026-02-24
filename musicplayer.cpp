#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <SFML/Audio.hpp>

using namespace std;

// ================= SONG NODE =================
struct Song {
    string name;
    string path;
    int duration; // seconds
    Song* prev;
    Song* next;

    Song(string n, string p, int d)
        : name(n), path(p), duration(d), prev(NULL), next(NULL) {}
};

// ================= MUSIC PLAYER CLASS =================
class MusicPlayer {
private:
    Song* head;
    Song* tail;
    Song* current;

    stack<string> recentlyPlayed;
    queue<Song*> playNextQueue;

    bool shuffleMode = false;
    bool repeatOne = false;
    bool repeatAll = false;

    sf::Music music;

public:
    MusicPlayer() {
        head = tail = current = NULL;
    }

    // Add Song
    void addSong(string name, string path) {
        sf::Music temp;
        if (!temp.openFromFile(path)) {
            cout << "Invalid MP3 file!\n";
            return;
        }

        int duration = temp.getDuration().asSeconds();
        Song* newSong = new Song(name, path, duration);

        if (!head) {
            head = tail = newSong;
        } else {
            tail->next = newSong;
            newSong->prev = tail;
            tail = newSong;
        }

        cout << "Song added successfully! Duration: "
             << duration / 60 << ":" << duration % 60 << endl;
    }

    // Display Playlist
    void displayPlaylist() {
        Song* temp = head;
        cout << "\n--- PLAYLIST ---\n";
        while (temp) {
            cout << temp->name << " ("
                 << temp->duration / 60 << ":"
                 << temp->duration % 60 << ")\n";
            temp = temp->next;
        }
    }

    // Search Song
    Song* searchSong(string keyword) {
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        Song* temp = head;

        while (temp) {
            string tempName = temp->name;
            transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);

            if (tempName.find(keyword) != string::npos) {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    // Play Song
    void playSong(Song* song) {
        if (!song) return;

        current = song;
        music.openFromFile(song->path);
        music.play();

        recentlyPlayed.push(song->name);

        cout << "Now Playing: " << song->name << endl;
    }

    // Next Song
    void nextSong() {
        if (repeatOne && current) {
            playSong(current);
            return;
        }

        if (shuffleMode) {
            vector<Song*> songs;
            Song* temp = head;
            while (temp) {
                songs.push_back(temp);
                temp = temp->next;
            }

            shuffle(songs.begin(), songs.end(), default_random_engine(time(0)));
            playSong(songs[0]);
            return;
        }

        if (current && current->next) {
            playSong(current->next);
        } else if (repeatAll) {
            playSong(head);
        } else {
            cout << "No next song.\n";
        }
    }

    // Previous Song
    void prevSong() {
        if (current && current->prev)
            playSong(current->prev);
        else
            cout << "No previous song.\n";
    }

    // Toggle Modes
    void toggleShuffle() {
        shuffleMode = !shuffleMode;
        cout << "Shuffle: " << (shuffleMode ? "ON\n" : "OFF\n");
    }

    void toggleRepeatOne() {
        repeatOne = !repeatOne;
        cout << "Repeat One: " << (repeatOne ? "ON\n" : "OFF\n");
    }

    void toggleRepeatAll() {
        repeatAll = !repeatAll;
        cout << "Repeat All: " << (repeatAll ? "ON\n" : "OFF\n");
    }

    // Recently Played
    void showRecentlyPlayed() {
        stack<string> temp = recentlyPlayed;
        cout << "\n--- Recently Played ---\n";
        while (!temp.empty()) {
            cout << temp.top() << endl;
            temp.pop();
        }
    }

    // Save Playlist
    void saveToFile() {
        ofstream file("playlist.txt");
        Song* temp = head;
        while (temp) {
            file << temp->name << "," << temp->path << endl;
            temp = temp->next;
        }
        file.close();
    }

    // Load Playlist
    void loadFromFile() {
        ifstream file("playlist.txt");
        string line;

        while (getline(file, line)) {
            size_t pos = line.find(",");
            string name = line.substr(0, pos);
            string path = line.substr(pos + 1);
            addSong(name, path);
        }
        file.close();
    }
};

// ================= MAIN =================
int main() {
    MusicPlayer player;
    player.loadFromFile();

    int choice;
    string name, path;

    do {
        cout << "\n===== MUSIC PLAYER =====\n";
        cout << "1. Add Song\n";
        cout << "2. Display Playlist\n";
        cout << "3. Search Song\n";
        cout << "4. Next\n";
        cout << "5. Previous\n";
        cout << "6. Toggle Shuffle\n";
        cout << "7. Toggle Repeat One\n";
        cout << "8. Toggle Repeat All\n";
        cout << "9. Recently Played\n";
        cout << "10. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter Song Name: ";
                getline(cin, name);
                cout << "Enter Full MP3 Path: ";
                getline(cin, path);
                player.addSong(name, path);
                break;

            case 2:
                player.displayPlaylist();
                break;

            case 3:
                cout << "Enter search keyword: ";
                getline(cin, name);
                if (player.searchSong(name))
                    cout << "Song Found!\n";
                else
                    cout << "Not Found!\n";
                break;

            case 4:
                player.nextSong();
                break;

            case 5:
                player.prevSong();
                break;

            case 6:
                player.toggleShuffle();
                break;

            case 7:
                player.toggleRepeatOne();
                break;

            case 8:
                player.toggleRepeatAll();
                break;

            case 9:
                player.showRecentlyPlayed();
                break;

            case 10:
                player.saveToFile();
                cout << "Goodbye!\n";
                break;
        }

    } while (choice != 10);

    return 0;
}   