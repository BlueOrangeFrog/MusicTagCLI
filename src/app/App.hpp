#pragma once
#include <filesystem>
#include <functional>
#include <string>
#include <set>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include "core/FileEntry.hpp"
#include "core/TagData.hpp"
#include "online/SearchResult.hpp"

enum class AppFocus { Browser, Editor, Dialog };

struct App {
    // ── File browser state ────────────────────────────────────────────────
    std::filesystem::path  current_dir;
    std::vector<FileEntry> entries;
    int                    browser_cursor = 0;
    std::set<int>          selected_indices;  // for batch operations

    // ── Tag editor state ──────────────────────────────────────────────────
    std::filesystem::path  current_file;   // currently loaded audio file
    TagData                loaded_tag;     // snapshot from disk
    TagData                edited_tag;     // mutable copy (UI works on this)
    bool                   dirty = false;  // edited_tag != loaded_tag

    // ── Focus ─────────────────────────────────────────────────────────────
    AppFocus focus     = AppFocus::Browser;
    int      focus_tab = 0;  // mirrors focus for Container::Tab (0=Browser, 1=Editor)

    // ── Dialog flags ──────────────────────────────────────────────────────
    bool show_confirm_dialog = false;
    bool show_batch_dialog   = false;
    bool show_cover_dialog   = false;
    bool show_online_dialog  = false;
    bool show_help_dialog    = false;

    // Confirm dialog content
    std::string            confirm_message;
    std::function<void()>  confirm_yes;
    std::function<void()>  confirm_no;

    // ── Online search state (written from background thread) ──────────────
    std::mutex                  online_mutex;
    std::atomic<bool>           online_searching{false};
    std::vector<SearchResult>   online_results;   // guarded by online_mutex
    std::string                 online_error;      // guarded by online_mutex

    // ── Status bar message ────────────────────────────────────────────────
    std::string status_msg;
    std::chrono::steady_clock::time_point status_expiry;

    void set_status(const std::string& msg, int ms = 3000);
    std::string active_status() const;  // returns "" when expired

    // ── Helpers ───────────────────────────────────────────────────────────
    void load_directory(const std::filesystem::path& dir);
    // Returns paths of all selected audio entries (or current if none selected).
    std::vector<std::filesystem::path> batch_paths() const;
};
