#include <filesystem>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <getopt.h>
#include <assert.h>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = std::filesystem; // Aliasing für einfachere Verwendung

unsigned short Counter_Option_f = 0;
unsigned short Counter_Option_h = 0;
unsigned short Counter_Option_v = 0;
unsigned short Counter_Option_r = 0;
unsigned short Counter_Option_i = 0;

/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-h] [-v] [-d verzeichnis] [-f dateiname] [-i] [-r] \n\n", programm_name);
    return;
}

std::string toLower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return result;
}

/* Funktion zum Suchen von Dateien */
void search_files_in_directory(std::filesystem::path filePath, const std::string &file_pattern)
{
    string lowerName = toLower(file_pattern);

    if (!fs::exists(filePath) || !fs::is_directory(filePath))
    {
        std::cerr << "Fehler: Das Verzeichnis '" << filePath << "' existiert nicht oder ist kein Verzeichnis.\n";
        exit(1);
    }

    for (const auto &entry : fs::directory_iterator(filePath))
    {
        const std::string filename = entry.path().filename().string();

        if (entry.is_regular_file())
        {
            bool fileMatches = (Counter_Option_i) ? toLower(filename) == lowerName : filename == file_pattern;
            if (!fileMatches) continue;

            std::cout << "Gefundene Datei: " << fs::absolute(entry.path()) << "\n";
        }
        else if (entry.is_directory())
        {
            if (filename.find(file_pattern) != std::string::npos)
            {
                std::cout << "Gefundener Ordner: " << entry.path() << "\n";
            }
            if (Counter_Option_r > 0)
            {
                search_files_in_directory(entry.path(), file_pattern);
            }
        }
    }
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
        case 'v':
            Counter_Option_v++;
            opt_verbose = 1;
            break;
        case 'f':
            Counter_Option_f++;
            dateiname.push_back(optarg);
            for (int i = optind; i < argc && argv[i][0] != '-'; i++)
            {
                dateiname.push_back(argv[i]);
                optind++; // Move the optind forward
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
    if ((Counter_Option_f > 1) || (Counter_Option_h > 1) || (Counter_Option_v > 1))
    {
        fprintf(stderr, "%s Fehler: Optionen wurden mehrfach verwendet.\n", programm_name);
        exit(1);
    }

    for (int i = 0; i < dateiname.size(); i++)
    {
        if (directory != nullptr)
        {
            // Dateisuche nur, wenn ein Verzeichnis angegeben wurde
            if (i < dateiname.size())
                search_files_in_directory(directory, dateiname[i]);

            else
                search_files_in_directory(directory, "");
        }

        if (i < dateiname.size() && directory == nullptr)
        {
            search_files_in_directory("./", dateiname[i]);
        }
        else if (i > dateiname.size() && directory != nullptr)
        {
            cerr << "Fehler: Keine Datei angegeben.\n";
            exit(1);
        }
    }

    if (optind < argc)
    {
        printf("ARGV Elemente ohne Optionen: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }

    // printf("Es wurden %u Argumente angeben.\n", argc);
    // printf("Verbose Modus ist");
    if (opt_verbose == 0)
    {
        // printf(" nicht");
    }
    // printf(" gesetzt.\n");
    return (0);
}
