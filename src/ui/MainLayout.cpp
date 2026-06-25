#include "MainLayout.hpp"
#include "FileBrowser.hpp"
#include "TagEditor.hpp"
#include "StatusBar.hpp"
#include "dialogs/ConfirmDialog.hpp"
#include "dialogs/BatchEditDialog.hpp"
#include "dialogs/CoverArtDialog.hpp"
#include "dialogs/OnlineLookupDialog.hpp"
#include "app/KeyBindings.hpp"
#include "core/TagReader.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>

using namespace ftxui;

// Help overlay component (F1)
static Component make_help_overlay(App* app) {
    auto btn = Button("Close [F1]", [app]() { app->show_help_dialog = false; });

    auto build_table = [](const KeyBinding* kb) {
        Elements rows;
        for (; kb->key; ++kb) {
            rows.push_back(hbox({
                text(kb->key)    | size(WIDTH, EQUAL, 18) | color(Color::Cyan),
                text(kb->action),
            }));
        }
        return vbox(rows);
    };

    return Renderer(btn, [btn, build_table]() {
        return vbox({
            text("Keyboard Shortcuts") | bold | center,
            separator(),
            hbox({
                vbox({ text("File Browser") | bold, build_table(BROWSER_KEYS) }) | flex,
                separator(),
                vbox({ text("Tag Editor")   | bold, build_table(EDITOR_KEYS)  }) | flex,
            }),
            separator(),
            btn->Render() | center,
        }) | border | size(WIDTH, GREATER_THAN, 60);
    });
}

ftxui::Component make_main_layout(App* app, ScreenInteractive& screen) {
    auto quit_fn = screen.ExitLoopClosure();

    // Callback: called by FileBrowser when an audio file is focused
    auto on_file = [app](const FileEntry& e) {
        auto tag = TagReader::read(e.path);
        if (tag) {
            app->current_file = e.path;
            app->loaded_tag   = *tag;
            app->edited_tag   = *tag;
            app->dirty        = false;
        } else {
            app->set_status("Cannot read tags: " + e.path.filename().string());
        }
    };

    // Panels
    auto browser   = make_file_browser(app, on_file, quit_fn);
    auto editor    = make_tag_editor(app);
    auto statusbar = make_status_bar(app);

    // Dialogs
    auto confirm_dlg = make_confirm_dialog(
        &app->show_confirm_dialog,
        "",  // message is set dynamically from app->confirm_message
        [app]() { if (app->confirm_yes) app->confirm_yes(); },
        [app]() { if (app->confirm_no)  app->confirm_no();  });

    // Re-create confirm dialog each render to pick up dynamic message — wrap in a lambda renderer
    auto confirm_renderer = Renderer(confirm_dlg, [app, confirm_dlg]() {
        // Patch: rebuild element with the current message each render
        auto dlg = vbox({
            text(app->confirm_message) | bold | center,
            separator(),
            confirm_dlg->Render(),
        }) | border | size(WIDTH, GREATER_THAN, 44);
        return dlg;
    });

    auto batch_dlg   = make_batch_dialog(app);
    auto cover_dlg   = make_cover_dialog(app);
    auto online_dlg  = make_online_dialog(app);
    auto help_dlg    = make_help_overlay(app);

    // Main content: dual-panel layout
    // Focus routing: only the active panel responds to events.
    auto panels = Container::Tab({browser, editor},
                                  reinterpret_cast<int*>(&app->focus));

    auto content = Renderer(panels, [app, browser, editor, statusbar]() {
        return vbox({
            // Title bar
            text(" MusicTagCLI") | bold | color(Color::Cyan)
                | size(HEIGHT, EQUAL, 1),
            // Dual panel
            hbox({
                browser->Render() | size(WIDTH, EQUAL, 35),
                editor->Render()  | flex,
            }) | flex,
            // Status bar
            statusbar->Render(),
        });
    });

    // Layer modals on top
    auto with_confirm = Modal(content,       confirm_renderer, &app->show_confirm_dialog);
    auto with_batch   = Modal(with_confirm,  batch_dlg,        &app->show_batch_dialog);
    auto with_cover   = Modal(with_batch,    cover_dlg,        &app->show_cover_dialog);
    auto with_online  = Modal(with_cover,    online_dlg,       &app->show_online_dialog);
    auto with_help    = Modal(with_online,   help_dlg,         &app->show_help_dialog);

    return with_help;
}
