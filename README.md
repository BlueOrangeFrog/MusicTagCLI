# MusicTagCLI

> **[English](#english) | [Italiano](#italiano)**

A terminal-based music metadata editor with a Midnight Commander–style TUI, online metadata lookup, and batch editing — written in C++17.

Un editor di metadati musicali da terminale con TUI in stile Midnight Commander, ricerca online e modifica in blocco — scritto in C++17.

---

<a name="english"></a>
## 🇬🇧 English

### Overview

MusicTagCLI is an interactive terminal application for reading and editing audio file tags (ID3, Vorbis Comments, MP4 atoms, etc.) directly from the command line, without a desktop environment. It provides a dual-panel interface — file browser on the left, tag editor on the right — with support for online metadata lookup via MusicBrainz and AcoustID fingerprinting.

```
┌──────────────────────────────────────────────────────────────────────────┐
│  MusicTagCLI                                                             │
├──────────────────────────┬───────────────────────────────────────────────┤
│  /home/user/Music/       │  track01.mp3                                  │
│  [DIR] ..                │                                               │
│  [DIR] Albums/           │  Titolo    : [ Bohemian Rhapsody           ]  │
│ *[MP3] track01.mp3  [*]  │  Artista   : [ Queen                       ]  │
│  [FLAC] album.flac       │  Album     : [ A Night at the Opera        ]  │
│  [OGG] brano.ogg         │  Anno      : [ 1975 ]                         │
│  [M4A] pezzo.m4a         │  Traccia   : [ 01 ]                           │
│  [OPUS] audio.opus       │  Genere    : [ Rock                        ]  │
│                          │  Commento  : [                             ]  │
│  [2 selezionati]         │  Copertina : image/jpeg (243 KB) [F5=Copertina]│
├──────────────────────────┴───────────────────────────────────────────────┤
│  /home/user/Music/track01.mp3   F1=Aiuto F2=Salva F5=Blocco F10=Esci   │
└──────────────────────────────────────────────────────────────────────────┘
```

### Features

- **Dual-panel TUI** — Midnight Commander–style layout with file browser and tag editor side by side
- **Multi-format support** — MP3 (ID3v1/v2), FLAC, OGG Vorbis, Opus, M4A/AAC and more
- **Online metadata** — Search MusicBrainz by title/artist/album and pre-fill tag fields
- **Audio fingerprinting** — Identify files by audio content via AcoustID (no tags needed)
- **Cover art** — Download from Cover Art Archive, or load from a local JPEG/PNG file
- **Batch editing** — Select multiple files and apply chosen fields to all at once
- **Italian UI** — Full Italian language interface
- **Zero runtime deps** — All libraries (FTXUI, TagLib, nlohmann/json, cpp-httplib) are fetched and linked statically at build time; only OpenSSL needs to be installed system-wide

### Supported Formats

| Format | Extension(s) | Tag Standard |
|--------|-------------|--------------|
| MP3 | `.mp3` | ID3v1, ID3v2.3, ID3v2.4 |
| FLAC | `.flac` | Vorbis Comments |
| OGG Vorbis | `.ogg`, `.oga` | Vorbis Comments |
| Opus | `.opus` | Vorbis Comments |
| M4A / AAC | `.m4a`, `.aac`, `.mp4` | MP4 atoms (iTunes-compatible) |
| WAV | `.wav`, `.wave` | ID3v2 (RIFF INFO chunk) |

Additional formats are available through TagLib (APE, WavPack, AIFF, WAV, etc.).

### Requirements

| Dependency | Purpose | How to install |
|-----------|---------|----------------|
| g++ ≥ 10 or clang++ ≥ 11 | C++17 compiler | `sudo apt install build-essential` |
| CMake ≥ 3.16 | Build system | `sudo apt install cmake` |
| Git | Fetching dependencies | `sudo apt install git` |
| libssl-dev | HTTPS support | `sudo apt install libssl-dev` |
| fpcalc (optional) | Audio fingerprinting | `sudo apt install chromaprint-tools` |

All C++ libraries (FTXUI, TagLib, nlohmann/json, cpp-httplib, Catch2) are downloaded and compiled automatically by CMake — no manual installation needed.

### Build

```bash
# Clone the repository
git clone https://github.com/<your-username>/MusicTagCLI.git
cd MusicTagCLI

# Configure (Release build recommended)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile (uses all available CPU cores)
cmake --build build -j$(nproc)

# Optional: run tests
cmake -B build -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

The resulting binary is at `build/musictagcli`.

### Installation

```bash
# Install to /usr/local/bin (requires root)
sudo cmake --install build

# Or just copy the binary manually
cp build/musictagcli ~/.local/bin/
```

### Usage

```bash
# Open current directory
musictagcli

# Open a specific directory
musictagcli ~/Music

# Open a specific file's directory
musictagcli ~/Music/album/
```

On first run, a configuration file is automatically created at `~/.config/musictagcli/config.ini`.

### Keyboard Shortcuts

#### File Browser (left panel)

| Key | Action |
|-----|--------|
| ↑ / ↓ or j / k | Navigate files |
| Enter or → | Enter directory |
| ← or Backspace | Go to parent directory |
| Space | Select / deselect file for batch editing |
| a | Select all audio files in current directory |
| A | Deselect all |
| Tab | Switch focus to Tag Editor |
| F5 | Open batch editor for selected files |
| F6 | Search metadata online |
| F10 | Quit |

#### Tag Editor (right panel)

| Key | Action |
|-----|--------|
| ↑ / ↓ or Tab | Move between fields |
| F2 | Save tags to file |
| F5 | Open cover art dialog |
| F6 | Search metadata online |
| ESC | Discard unsaved changes |
| Tab | Switch focus to File Browser |

### Online Metadata Lookup

Press **F6** on any audio file to open the online search dialog. Two search modes are available:

1. **Search by Tags** — Queries MusicBrainz using the current title, artist, and album fields. No API key required.
2. **Search by Fingerprint** — Analyses the audio content via AcoustID to identify the track even if tags are missing or wrong. Requires `fpcalc` installed and an AcoustID API key (see [Configuration](#configuration)).

Results are shown in a scrollable list with confidence scores. Select a result and press **Use this** to pre-fill the tag fields; then press **F2** to save.

### Cover Art

- **Download** — Select a result in the online dialog and press **Download Cover** to fetch the front artwork from the [Cover Art Archive](https://coverartarchive.org/).
- **Load from file** — Press **F5** in the Tag Editor to open the cover art dialog and enter a local JPEG or PNG path.
- **Remove** — Open the cover art dialog and press **Remove**.

### Batch Editing

1. Navigate to the files you want to edit in the File Browser.
2. Press **Space** to select individual files, or **a** to select all.
3. Press **F5** to open the Batch Edit dialog.
4. For each field, use the checkbox to choose whether to apply it.
5. Press **Apply** to write the selected fields to all chosen files.

### Configuration

The config file is created automatically at `~/.config/musictagcli/config.ini` on first run:

```ini
# MusicTagCLI configuration
# Register a free AcoustID API key at https://acoustid.org/api-key
# and paste it here to enable fingerprint-based metadata lookup.
acoustid_api_key=YOUR_KEY_HERE
```

Replace `YOUR_KEY_HERE` with your AcoustID API key to enable fingerprint search. Registration is free at [acoustid.org](https://acoustid.org/api-key).

### Project Structure

```
MusicTagCLI/
├── CMakeLists.txt
├── cmake/
│   └── Dependencies.cmake      # FetchContent declarations for all libraries
├── src/
│   ├── main.cpp
│   ├── app/
│   │   ├── App.hpp             # Shared application state
│   │   ├── i18n.hpp            # Italian translation strings
│   │   └── KeyBindings.hpp     # Keybinding documentation
│   ├── core/
│   │   ├── FileEntry.hpp       # File/directory entry type
│   │   ├── FormatDetector.hpp  # Extension → AudioFormat mapping
│   │   ├── DirectoryScanner.*  # Directory listing (dirs first, then audio)
│   │   ├── TagData.*           # Tag value object + audio properties
│   │   ├── TagReader.*         # TagLib → TagData
│   │   └── TagWriter.*         # TagData → TagLib → file
│   ├── online/
│   │   ├── SearchResult.hpp    # MusicBrainz/AcoustID result struct
│   │   ├── HttpClient.*        # cpp-httplib wrapper (HTTPS, retry, rate limit)
│   │   ├── MusicBrainzClient.* # Search + fetch recording metadata
│   │   ├── AcoustIdClient.*    # Fingerprint lookup via fpcalc
│   │   └── CoverArtClient.*    # Cover Art Archive downloader
│   └── ui/
│       ├── MainLayout.*        # Top-level layout + modal stacking
│       ├── FileBrowser.*       # Left panel
│       ├── TagEditor.*         # Right panel
│       ├── StatusBar.*         # Bottom status line
│       ├── widgets/TagField.*  # Labelled input row
│       └── dialogs/            # ConfirmDialog, BatchEditDialog,
│                               # CoverArtDialog, OnlineLookupDialog
└── tests/
    ├── CMakeLists.txt
    ├── test_tag_reader.cpp
    ├── test_tag_writer.cpp
    └── test_directory_scanner.cpp
```

### Libraries Used

| Library | Version | License | Role |
|---------|---------|---------|------|
| [FTXUI](https://github.com/ArthurSonzogni/FTXUI) | 6.1.9 | MIT | Terminal UI |
| [TagLib](https://github.com/taglib/taglib) | 2.0.2 | LGPL 2.1 / MPL 1.1 | Audio tag I/O |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.11.3 | MIT | JSON parsing |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | 0.40.0 | MIT | HTTP/HTTPS client |
| [Catch2](https://github.com/catchorg/Catch2) | 3.5.2 | BSL-1.0 | Unit tests |
| OpenSSL | system | Apache 2.0 | TLS for HTTPS |

---

<a name="italiano"></a>
## 🇮🇹 Italiano

### Panoramica

MusicTagCLI è un'applicazione terminale interattiva per leggere e modificare i tag dei file audio (ID3, Vorbis Comments, atomi MP4, ecc.) direttamente da riga di comando, senza bisogno di un ambiente desktop. Offre un'interfaccia a doppio pannello — browser file a sinistra, editor tag a destra — con supporto per la ricerca di metadati online tramite MusicBrainz e il riconoscimento audio tramite AcoustID.

```
┌──────────────────────────────────────────────────────────────────────────┐
│  MusicTagCLI                                                             │
├──────────────────────────┬───────────────────────────────────────────────┤
│  /home/utente/Musica/    │  traccia01.mp3                                │
│  [DIR] ..                │                                               │
│  [DIR] Album/            │  Titolo    : [ Bohemian Rhapsody           ]  │
│ *[MP3] traccia01.mp3 [*] │  Artista   : [ Queen                       ]  │
│  [FLAC] album.flac       │  Album     : [ A Night at the Opera        ]  │
│  [OGG] brano.ogg         │  Anno      : [ 1975 ]                         │
│  [M4A] pezzo.m4a         │  Traccia   : [ 01 ]                           │
│  [OPUS] audio.opus       │  Genere    : [ Rock                        ]  │
│                          │  Commento  : [                             ]  │
│  [2 selezionati]         │  Copertina : image/jpeg (243 KB) [F5=Copertina]│
├──────────────────────────┴───────────────────────────────────────────────┤
│  /home/utente/Musica/traccia01.mp3  F1=Aiuto F2=Salva F5=Blocco F10=Esci│
└──────────────────────────────────────────────────────────────────────────┘
```

### Funzionalità

- **TUI a doppio pannello** — Layout stile Midnight Commander con browser file ed editor tag affiancati
- **Supporto multiformato** — MP3 (ID3v1/v2), FLAC, OGG Vorbis, Opus, M4A/AAC e altri
- **Metadati online** — Ricerca su MusicBrainz per titolo/artista/album e compilazione automatica dei campi
- **Impronta audio** — Identificazione del brano tramite contenuto audio con AcoustID (non servono tag esistenti)
- **Copertine** — Scarica dalla Cover Art Archive o carica da un file JPEG/PNG locale
- **Modifica in blocco** — Seleziona più file e applica i campi scelti a tutti in una sola operazione
- **Interfaccia in italiano** — Tutta l'interfaccia è localizzata in italiano
- **Nessuna dipendenza a runtime** — Tutte le librerie (FTXUI, TagLib, nlohmann/json, cpp-httplib) vengono scaricate e linkate staticamente in fase di build; solo OpenSSL deve essere installata nel sistema

### Formati supportati

| Formato | Estensioni | Standard tag |
|---------|-----------|--------------|
| MP3 | `.mp3` | ID3v1, ID3v2.3, ID3v2.4 |
| FLAC | `.flac` | Vorbis Comments |
| OGG Vorbis | `.ogg`, `.oga` | Vorbis Comments |
| Opus | `.opus` | Vorbis Comments |
| M4A / AAC | `.m4a`, `.aac`, `.mp4` | Atomi MP4 (compatibile iTunes) |

Tramite TagLib sono disponibili ulteriori formati (APE, WavPack, AIFF, WAV, ecc.).

### Requisiti

| Dipendenza | Scopo | Come installare |
|-----------|-------|-----------------|
| g++ ≥ 10 o clang++ ≥ 11 | Compilatore C++17 | `sudo apt install build-essential` |
| CMake ≥ 3.16 | Sistema di build | `sudo apt install cmake` |
| Git | Download dipendenze | `sudo apt install git` |
| libssl-dev | Supporto HTTPS | `sudo apt install libssl-dev` |
| fpcalc (opzionale) | Impronta audio | `sudo apt install chromaprint-tools` |

Tutte le librerie C++ (FTXUI, TagLib, nlohmann/json, cpp-httplib, Catch2) vengono scaricate e compilate automaticamente da CMake — non è necessaria alcuna installazione manuale.

### Compilazione

```bash
# Clona il repository
git clone https://github.com/<tuo-username>/MusicTagCLI.git
cd MusicTagCLI

# Configurazione (build Release consigliata)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compilazione (usa tutti i core disponibili)
cmake --build build -j$(nproc)

# Facoltativo: esegui i test
cmake -B build -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

Il binario risultante si trova in `build/musictagcli`.

### Installazione

```bash
# Installa in /usr/local/bin (richiede root)
sudo cmake --install build

# Oppure copia manualmente il binario
cp build/musictagcli ~/.local/bin/
```

### Utilizzo

```bash
# Apri la directory corrente
musictagcli

# Apri una directory specifica
musictagcli ~/Musica

# Apri la directory di un file specifico
musictagcli ~/Musica/album/
```

Al primo avvio, il file di configurazione viene creato automaticamente in `~/.config/musictagcli/config.ini`.

### Tasti rapidi

#### Browser file (pannello sinistro)

| Tasto | Azione |
|-------|--------|
| ↑ / ↓ o j / k | Naviga tra i file |
| Invio o → | Entra nella directory |
| ← o Backspace | Directory superiore |
| Spazio | Seleziona / deseleziona per la modifica in blocco |
| a | Seleziona tutti i file audio nella directory corrente |
| A | Deseleziona tutto |
| Tab | Sposta il focus all'Editor tag |
| F5 | Apri l'editor in blocco per i file selezionati |
| F6 | Cerca metadati online |
| F10 | Esci |

#### Editor tag (pannello destro)

| Tasto | Azione |
|-------|--------|
| ↑ / ↓ o Tab | Naviga tra i campi |
| F2 | Salva i tag nel file |
| F5 | Apri il dialogo copertina |
| F6 | Cerca metadati online |
| ESC | Scarta le modifiche non salvate |
| Tab | Sposta il focus al Browser file |

### Ricerca metadati online

Premi **F6** su qualsiasi file audio per aprire il dialogo di ricerca online. Sono disponibili due modalità:

1. **Cerca per tag** — Interroga MusicBrainz usando i campi titolo, artista e album correnti. Non richiede API key.
2. **Cerca per impronta** — Analizza il contenuto audio tramite AcoustID per identificare il brano anche in assenza o in caso di errore nei tag. Richiede `fpcalc` installato e una API key AcoustID (vedi [Configurazione](#configurazione)).

I risultati vengono mostrati in una lista scorrevole con punteggio di corrispondenza. Seleziona un risultato e premi **Usa questo** per precompilare i campi; poi premi **F2** per salvare.

### Copertine

- **Scarica** — Seleziona un risultato nel dialogo online e premi **Scarica copertina** per recuperare la copertina frontale dalla [Cover Art Archive](https://coverartarchive.org/).
- **Carica da file** — Premi **F5** nell'Editor tag per aprire il dialogo copertina e inserire il percorso di un file JPEG o PNG locale.
- **Rimuovi** — Apri il dialogo copertina e premi **Rimuovi**.

### Modifica in blocco

1. Naviga fino ai file che vuoi modificare nel Browser file.
2. Premi **Spazio** per selezionare i file singolarmente, oppure **a** per selezionarli tutti.
3. Premi **F5** per aprire il dialogo Modifica in blocco.
4. Per ogni campo, usa la casella di spunta per scegliere se applicarlo.
5. Premi **Applica** per scrivere i campi selezionati su tutti i file scelti.

### Configurazione

<a name="configurazione"></a>

Il file di configurazione viene creato automaticamente in `~/.config/musictagcli/config.ini` al primo avvio:

```ini
# MusicTagCLI configuration
# Register a free AcoustID API key at https://acoustid.org/api-key
# and paste it here to enable fingerprint-based metadata lookup.
acoustid_api_key=YOUR_KEY_HERE
```

Sostituisci `YOUR_KEY_HERE` con la tua API key AcoustID per abilitare la ricerca per impronta. La registrazione è gratuita su [acoustid.org](https://acoustid.org/api-key).

### Struttura del progetto

```
MusicTagCLI/
├── CMakeLists.txt
├── cmake/
│   └── Dependencies.cmake      # Dichiarazioni FetchContent per tutte le librerie
├── src/
│   ├── main.cpp
│   ├── app/
│   │   ├── App.hpp             # Stato condiviso dell'applicazione
│   │   ├── i18n.hpp            # Stringhe di traduzione italiana
│   │   └── KeyBindings.hpp     # Documentazione tasti rapidi
│   ├── core/
│   │   ├── FileEntry.hpp       # Tipo entry file/directory
│   │   ├── FormatDetector.hpp  # Mappatura estensione → AudioFormat
│   │   ├── DirectoryScanner.*  # Listato directory (prima dir, poi audio)
│   │   ├── TagData.*           # Value object tag + proprietà audio
│   │   ├── TagReader.*         # TagLib → TagData
│   │   └── TagWriter.*         # TagData → TagLib → file
│   ├── online/
│   │   ├── SearchResult.hpp    # Struttura risultato MusicBrainz/AcoustID
│   │   ├── HttpClient.*        # Wrapper cpp-httplib (HTTPS, retry, rate limit)
│   │   ├── MusicBrainzClient.* # Ricerca e recupero metadati brani
│   │   ├── AcoustIdClient.*    # Lookup per impronta tramite fpcalc
│   │   └── CoverArtClient.*    # Downloader Cover Art Archive
│   └── ui/
│       ├── MainLayout.*        # Layout principale + sovrapposizione modali
│       ├── FileBrowser.*       # Pannello sinistro
│       ├── TagEditor.*         # Pannello destro
│       ├── StatusBar.*         # Barra di stato inferiore
│       ├── widgets/TagField.*  # Riga di input con etichetta
│       └── dialogs/            # ConfirmDialog, BatchEditDialog,
│                               # CoverArtDialog, OnlineLookupDialog
└── tests/
    ├── CMakeLists.txt
    ├── test_tag_reader.cpp
    ├── test_tag_writer.cpp
    └── test_directory_scanner.cpp
```

### Librerie utilizzate

| Libreria | Versione | Licenza | Ruolo |
|---------|---------|---------|-------|
| [FTXUI](https://github.com/ArthurSonzogni/FTXUI) | 6.1.9 | MIT | Interfaccia TUI |
| [TagLib](https://github.com/taglib/taglib) | 2.0.2 | LGPL 2.1 / MPL 1.1 | Lettura/scrittura tag audio |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.11.3 | MIT | Parsing JSON |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | 0.40.0 | MIT | Client HTTP/HTTPS |
| [Catch2](https://github.com/catchorg/Catch2) | 3.5.2 | BSL-1.0 | Test unitari |
| OpenSSL | sistema | Apache 2.0 | TLS per HTTPS |
