#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <cstdint>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

// ==========================================
// 1. ANALIZZA LA CARTELLA TEMP DI WINDOWS
// ==========================================
void analizzaTemp(const string& percorso) {
    int fileTrovati = 0;
    uintmax_t spazioTotaleBytes = 0;

    cout << "\nAnalisi in corso..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(percorso, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                fileTrovati++;
                std::error_code ec;
                uintmax_t dim = fs::file_size(entry.path(), ec);
                if (!ec) {
                    spazioTotaleBytes += dim;
                }
            }
        }

        double spazioMB = static_cast<double>(spazioTotaleBytes) / (1024.0 * 1024.0);

        cout << "\n=================================" << endl;
        cout << "       RISULTATO ANALISI         " << endl;
        cout << "=================================" << endl;
        cout << "File temporanei trovati: " << fileTrovati << endl;
        cout << "Spazio occupato stimato: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 2.SPOSTA I FILE TEMP NEL CESTINO SICURO
// ==========================================
void spostaNelCestino(const string& percorsoTemp, const string& percorsoCestino) {
    int fileSpostati = 0;
    uintmax_t spazioSpostatoBytes = 0;
    std::error_code ec;

    // Crea la cartella del cestino se non esiste
    if (!fs::exists(percorsoCestino)) {
        fs::create_directories(percorsoCestino, ec);
    }

    cout << "\nSpostamento dei file nel Cestino di sicurezza..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(percorsoTemp, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                std::error_code ecSize;
                uintmax_t dim = fs::file_size(entry.path(), ecSize);
                
                fs::path destinazione = fs::path(percorsoCestino) / entry.path().filename();
                
                // Se nel cestino esiste già un file con lo stesso nome, lo rimuoviamo prima di spostarlo
                if (fs::exists(destinazione)) {
                    fs::remove(destinazione, ec);
                }

                // Sposta il file nel cestino
                fs::rename(entry.path(), destinazione, ec);
                if (!ec) {
                    fileSpostati++;
                    if (!ecSize) spazioSpostatoBytes += dim;
                }
            }
        }

        double spazioMB = static_cast<double>(spazioSpostatoBytes) / (1024.0 * 1024.0);

        cout << "\n=================================" << endl;
        cout << "       SPOSTAMENTO AL CESTINO    " << endl;
        cout << "=================================" << endl;
        cout << "File spostati nel Cestino: " << fileSpostati << endl;
        cout << "Spazio liberato dalla Temp: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 3. GESTIONE DEL CESTINO (Svuota o Visualizza)
// ==========================================
void gestisciCestino(const string& percorsoCestino) {
    std::error_code ec;
    if (!fs::exists(percorsoCestino)) {
        cout << "\nIl Cestino e' attualmente vuoto." << endl;
        return;
    }

    int sceltaCestino;
    do {
        cout << "\n=================================" << endl;
        cout << "      GESTIONE CESTINO SICURO    " << endl;
        cout << "=================================" << endl;
        cout << "1. Elenca file nel Cestino" << endl;
        cout << "2. Svuota Cestino (Elimina definitivamente)" << endl;
        cout << "3. Torna al menu principale" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> sceltaCestino)) {