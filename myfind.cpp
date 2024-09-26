#include <filesystem>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <getopt.h>
#include <assert.h>
#include <algorithm>
#include <cctype>
#include <sys/wait.h>

using namespace std;
namespace fs = std::filesystem; // Aliasing für einfachere Verwendung

/* Globale Variablen */
unsigned short Counter_Option_f = 0;
unsigned short Counter_Option_h = 0;
unsigned short Counter_Option_r = 0;
unsigned short Counter_Option_i = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-h] [-v] [-d verzeichnis] [-f dateiname] [-i] [-r] \n\n", programm_name);
    return;
}

std::string toLower(const std::string &str) // Funktion zum Umwandeln von Groß- in Kleinbuchstaben (Case-Insensitive)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return result;
}

// Funktion zum Suchen von Dateien im Verzeichnis
bool search_files_in_directory(std::filesystem::path filePath, const std::string &file_pattern)
{
    bool fileFound = false; // tracken ob file gefunden wurde
    string lowerName = toLower(file_pattern);

    if (!fs::exists(filePath) || !fs::is_directory(filePath))
    {
        std::cerr << "Fehler: Das Verzeichnis '" << filePath << "' existiert nicht oder ist kein Verzeichnis.\n";
        return false; // return false wenn Verzeichnis nicht existiert oder kein Verzeichnis ist
    }

    for (const auto &entry : fs::directory_iterator(filePath))
    {
        const std::string filename = entry.path().filename().string();
        bool fileMatches = (Counter_Option_i) ? toLower(filename) == lowerName : filename == file_pattern;

        if (entry.is_regular_file())
        {
            if (!fileMatches)
                continue;
            std::cout << "Gefundene Datei: " << fs::absolute(entry.path()) << "\n";
            fileFound = true; // fileFound true setzen wenn Datei gefunden wurde
        }
        else if (entry.is_directory())
        {
            if (filename.find(file_pattern) != std::string::npos)
            {
                std::cout << "Gefundener Ordner: " << entry.path() << "\n";
            }
            if (Counter_Option_r > 0)
            {
                // rekursiver Aufruf
                if (search_files_in_directory(entry.path(), file_pattern))
                {
                    fileFound = true; // falls file gefunden nach rekursiver suche, setze fileFound auf true
                }
            }
        }
        else
        {
            std::cerr << "Fehler: Datei '" << entry.path() << "' ist kein reguläres File oder Verzeichnis.\n";
            return false;
        }
    }
    return fileFound; // return true, falls fileFound true ist
}

/* Entry Point */
int main(int argc, char *argv[])
{
    int c;
    std::vector<std::string> dateiname;
    char *directory = nullptr;
    char *programm_name;

    programm_name = argv[0];

    while ((c = getopt(argc, argv, "hvf:d:iR")) != EOF)
    {
        switch (c)
        {
        case '?':
            fprintf(stderr, "%s error: Unknown option.\n", programm_name);
            print_usage(programm_name);
            exit(1);
            break;
        case 'h':
            Counter_Option_h++;
            print_usage(programm_name);
            exit(0);
            break;
        case 'f':
            Counter_Option_f++;
            dateiname.push_back(optarg);
            for (int i = optind; i < argc && argv[i][0] != '-'; i++)
            {
                dateiname.push_back(argv[i]);
                optind++;
            }
            break;
        case 'd':
            directory = optarg;
            break;
        case 'R':
            Counter_Option_r++;
            break;
        case 'i':
            Counter_Option_i++;
            break;
        default:
            assert(0);
        }
    }
    if ((Counter_Option_f > 1) || (Counter_Option_h > 1) || (Counter_Option_r > 1) || (Counter_Option_i > 1))
    {
        fprintf(stderr, "%s Fehler: Optionen wurden mehrfach verwendet.\n", programm_name);
        exit(1);
    }

    for (int i = 0; i < dateiname.size(); i++)
    {
        pid_t pid = fork(); // fuer jede Datei ein Kindprozess erstellen

        if (pid < 0)
        {
            // Fork fehlgeschlagen
            std::cerr << "Error: Unable to fork.\n";
            exit(1);
        }
        else if (pid == 0)
        {
            // Child process
            if (directory != nullptr)
            {
                cout << "PID: " << getpid() << " ";
                if (!search_files_in_directory(directory, dateiname[i]))
                    cout << "Datei " << dateiname[i] << " nicht gefunden.\n";
            }
            else
            {
                cout << "PID: " << getpid() << " ";
                if (!search_files_in_directory("./", dateiname[i]))
                    cout << "Datei " << dateiname[i] << " nicht gefunden.\n";
            }
            exit(0); // kill child process nach suche
        }
    }
    return (0);
}
