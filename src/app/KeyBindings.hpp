#pragma once

// Centralised documentation of key bindings (used by the help overlay).
// The actual event handling is distributed across UI components, but these
// constants ensure consistency in labels shown to the user.

struct KeyBinding {
    const char* key;
    const char* action;
};

inline constexpr KeyBinding BROWSER_KEYS[] = {
    {"↑/↓ j/k",   "Navigate files"},
    {"Enter/→",   "Enter directory"},
    {"←/Bksp",    "Parent directory"},
    {"Space",      "Select/deselect for batch"},
    {"a",          "Select all audio files"},
    {"A",          "Deselect all"},
    {"Tab",        "Switch to Tag Editor"},
    {"F5",         "Batch edit selected files"},
    {"F6",         "Search metadata online"},
    {"F10",        "Quit"},
    {nullptr, nullptr},
};

inline constexpr KeyBinding EDITOR_KEYS[] = {
    {"↑/↓ Tab",   "Navigate fields"},
    {"F2",         "Save tags to file"},
    {"F6",         "Search metadata online"},
    {"ESC",        "Cancel / discard changes"},
    {"Tab",        "Switch to File Browser"},
    {nullptr, nullptr},
};
