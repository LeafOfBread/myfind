#include <filesystem>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <getopt.h>
#include <assert.h>

using namespace std;
namespace fs = std::filesystem;  // Aliasing für einfachere Verwendung

/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-h] [-v] [-f dateiname] [-d verzeichnis]\n\n", programm_name);
    return;
}

void case_insenstive_search_files_in_directory(std::filesystem::path filePath, const std::string &file_pattern)
{
    if (!fs::exists(filePath) || !fs::is_directory(filePath))
    {
        cerr << "Fehler: Das Verzeichnis '" << filePath << "' existiert nicht oder ist kein Verzeichnis.\n";
        exit(1);
    }

    cout << "Durchsuche Verzeichnis: " << filePath << "\n";
    for (const auto &entry : fs::directory_iterator(filePath))
    {
        if (entry.is_regular_file())
        {
            // Überprüfen, ob der Dateiname dem Suchmuster entspricht
            if (entry.path().filename().string().find(file_pattern) != std::string::npos)
            {
                cout << "Gefundene Datei: " << entry.path() << "\n";
            }
        }
    }
}

/* Funktion zum Suchen von Dateien */
void search_files_in_directory(std::filesystem::path filePath, const std::string &file_pattern)
{
    if (!fs::exists(filePath) || !fs::is_directory(filePath))
    {
        std::cerr << "Fehler: Das Verzeichnis '" << filePath << "' existiert nicht oder ist kein Verzeichnis.\n";
        exit(1);
    }

    std::cout << "Durchsuche Verzeichnis: " << filePath << "\n";
    for (const auto &entry : fs::directory_iterator(filePath))
    {
        if (entry.is_regular_file())
        {
            // Überprüfen, ob der Dateiname dem Suchmuster entspricht
            if (entry.path().filename().string().find(file_pattern) != std::string::npos)
            {
                std::cout << "Gefundene Datei: " << entry.path() << "\n";
            }
        }
        else if (entry.is_directory())
        {
            // Überprüfen, ob der Ordnername dem Suchmuster entspricht
            if (entry.path().filename().string().find(file_pattern) != std::string::npos)
            {
                std::cout << "Gefundener Ordner: " << entry.path() << "\n";
            }
        }
    }
}

void search_if_file_exists(std::filesystem::path filePath)
{
    if (fs::exists(filePath))
    {
        cout << "Die Datei '" << filePath << "' existiert.\n";
    }
    else
    {
        cout << "Die Datei '" << filePath << "' existiert nicht.\n";
    }
}

/* Entry Point */
int main(int argc, char *argv[])
{
    int c;
    char *dateiname = nullptr;
    char *directory = nullptr;
    char *programm_name;
    unsigned short Counter_Option_f = 0;
    unsigned short Counter_Option_h = 0;
    unsigned short Counter_Option_v = 0;
    unsigned short Counter_Option_r = 0;

    programm_name = argv[0];

    while ((c = getopt(argc, argv, "hvf:d:")) != EOF)
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
            dateiname = optarg;
            break;
        case 'd':
            directory = optarg;
            break;
        case 'R':
            Counter_Option_r++;
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

    if (directory != nullptr)
    {
        // Dateisuche nur, wenn ein Verzeichnis angegeben wurde
        if (dateiname != nullptr)
        {
            search_files_in_directory(directory, dateiname);
        }
        else
        {
            search_files_in_directory(directory, "");
        }
    }

    if (dateiname != nullptr)
    {
        search_if_file_exists(dateiname);
    }
    else
    {
        cerr << "Fehler: Keine Datei angegeben.\n";
        exit(1);
    }

    cout << argv[optind] << endl;
    if (optind < argc)
    {
        printf("ARGV Elemente ohne Optionen: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }

    printf("Es wurden %u Argumente angeben.\n", argc);
    printf("Verbose Modus ist");
    if (opt_verbose == 0)
    {
        printf(" nicht");
    }
    printf(" gesetzt.\n");
    if (Counter_Option_f > 0)
    {
        printf("Die Datei '%s' wurde mit der Option -f angegeben.\n", dateiname);
    }
    return (0);
}
