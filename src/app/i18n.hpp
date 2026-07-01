#pragma once
#include <string>
#include <unordered_map>

inline std::string t(const char* key) {
    static const std::unordered_map<std::string, std::string> tr = {
        // MainLayout
        {"Close [F1]",         "Chiudi [F1]"},
        {"Keyboard Shortcuts", "Tasti rapidi"},
        {"File Browser",       "Browser file"},
        {"Tag Editor",         "Editor tag"},
        {"Cannot read tags: ", "Impossibile leggere i tag: "},
        // FileBrowser
        {"No files selected for batch edit.", "Nessun file selezionato per la modifica in blocco."},
        {"Unsaved changes \xe2\x80\x94 quit anyway?", "Modifiche non salvate \xe2\x80\x94 uscire comunque?"},
        {" selected]",         " selezionati]"},
        // TagEditor field labels
        {"Title",   "Titolo"},
        {"Artist",  "Artista"},
        {"Album",   "Album"},
        {"Year",    "Anno"},
        {"Track",   "Traccia"},
        {"Genre",   "Genere"},
        {"Comment", "Commento"},
        {"Cover",   "Copertina"},
        // TagEditor messages
        {"No file loaded.",                      "Nessun file caricato."},
        {"Saved: ",                              "Salvato: "},
        {"ERROR: could not write tags to file.", "ERRORE: impossibile scrivere i tag nel file."},
        {"Discard unsaved changes?",             "Scartare le modifiche non salvate?"},
        {"(no file selected)",                   "(nessun file selezionato)"},
        {"(no cover art)",                       "(nessuna copertina)"},
        {"Cover  : ",                            "Copertina: "},
        {"  [F5=CoverArt]",                      "  [F5=Copertina]"},
        // StatusBar hints
        {"F1=Help  F2=Save  F5=Batch  F6=Online  F10=Quit",
         "F1=Aiuto  F2=Salva  F5=Blocco  F6=Online  F10=Esci"},
        {"F1=Help  F2=Save  F6=Online  ESC=Cancel  Tab=Switch",
         "F1=Aiuto  F2=Salva  F6=Online  ESC=Annulla  Tab=Cambia"},
        // ConfirmDialog
        {"  Yes  ", "  S\xc3\xac  "},
        {"  No   ", "  No  "},
        // BatchEditDialog
        {"Apply",         "Applica"},
        {"Cancel",        "Annulla"},
        {"current file",  "file corrente"},
        {" files",        " file"},
        {"Batch Edit \xe2\x80\x94 ", "Modifica in blocco \xe2\x80\x94 "},
        {" errors",       " errori"},
        // CoverArtDialog
        {"Load",   "Carica"},
        {"Remove", "Rimuovi"},
        {"Cover Art",   "Copertina"},
        {"Image path:", "Percorso immagine:"},
        {"/path/to/cover.jpg",                          "/percorso/alla/copertina.jpg"},
        {"Cannot open file.",                           "Impossibile aprire il file."},
        {"File is empty.",                              "Il file \xc3\xa8 vuoto."},
        {"Unsupported format (expected JPEG or PNG).",  "Formato non supportato (JPEG o PNG richiesto)."},
        // OnlineLookupDialog
        {"Search by Tags",        "Cerca per tag"},
        {"Search by Fingerprint", "Cerca per impronta"},
        {"Use this",              "Usa questo"},
        {"Download Cover",        "Scarica copertina"},
        {"Close",                 "Chiudi"},
        {"Online Metadata Search",                     "Ricerca metadati online"},
        {"  Searching...",                             "  Ricerca in corso..."},
        {"No results found.",                          "Nessun risultato trovato."},
        {"No file loaded.",                            "Nessun file caricato."},
        {"Tags pre-filled \xe2\x80\x94 press F2 to save.",
         "Tag precompilati \xe2\x80\x94 premi F2 per salvare."},
        {"No cover art available for this release.",   "Nessuna copertina disponibile per questa release."},
        {"Cover art downloaded.",                      "Copertina scaricata."},
        // KeyBindings action descriptions
        {"Navigate files",             "Naviga file"},
        {"Enter directory",            "Entra nella directory"},
        {"Parent directory",           "Directory superiore"},
        {"Select/deselect for batch",  "Seleziona/deseleziona per blocco"},
        {"Select all audio files",     "Seleziona tutti i file audio"},
        {"Deselect all",               "Deseleziona tutto"},
        {"Switch to Tag Editor",       "Passa all'editor tag"},
        {"Batch edit selected files",  "Modifica in blocco i file selezionati"},
        {"Search metadata online",     "Cerca metadati online"},
        {"Quit",                       "Esci"},
        {"Navigate fields",            "Naviga tra i campi"},
        {"Save tags to file",          "Salva tag nel file"},
        {"Cancel / discard changes",   "Annulla / scarta le modifiche"},
        {"Switch to File Browser",     "Passa al browser file"},
    };
    auto it = tr.find(key);
    return it != tr.end() ? it->second : std::string(key);
}

inline std::string t(const std::string& key) { return t(key.c_str()); }
