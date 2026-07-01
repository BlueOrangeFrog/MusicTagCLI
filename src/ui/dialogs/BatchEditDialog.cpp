#include "BatchEditDialog.hpp"
#include "core/TagWriter.hpp"
#include "ui/widgets/TagField.hpp"
#include "app/i18n.hpp"
#include <ftxui/dom/elements.hpp>
#include <array>

using namespace ftxui;

ftxui::Component make_batch_dialog(App* app) {
    struct State {
        TagData draft;
        std::array<bool, 8> checked = {true,true,true,true,true,true,true,false};
        // order: title, artist, album, year, track, genre, comment, cover
    };
    auto st = std::make_shared<State>();

    static const std::array<const char*, 8> FIELD_NAMES =
        {"title","artist","album","year","track","genre","comment","cover"};

    // Checkboxes with NO label — the label is shown by the adjacent input field.
    std::vector<Component> checkboxes;
    for (int i = 0; i < 8; ++i)
        checkboxes.push_back(Checkbox("", &st->checked[i]));

    // Input fields (year/track are digits-only)
    auto year_str  = std::make_shared<std::string>();
    auto track_str = std::make_shared<std::string>();
    std::vector<Component> inputs;
    inputs.push_back(make_tag_field(t("Title"),   &st->draft.title));
    inputs.push_back(make_tag_field(t("Artist"),  &st->draft.artist));
    inputs.push_back(make_tag_field(t("Album"),   &st->draft.album));
    inputs.push_back(make_tag_field(t("Year"),    year_str.get(),  true));
    inputs.push_back(make_tag_field(t("Track"),   track_str.get(), true));
    inputs.push_back(make_tag_field(t("Genre"),   &st->draft.genre));
    inputs.push_back(make_tag_field(t("Comment"), &st->draft.comment));

    auto btn_apply = Button(t("Apply"), [app, st, year_str, track_str]() {
        try { st->draft.year  = static_cast<uint32_t>(std::stoul(*year_str));  } catch (...) {}
        try { st->draft.track = static_cast<uint32_t>(std::stoul(*track_str)); } catch (...) {}

        std::set<std::string> fields;
        for (int i = 0; i < 8; ++i)
            if (st->checked[i]) fields.insert(FIELD_NAMES[i]);

        auto paths = app->batch_paths();
        if (!paths.empty()) {
            auto res = TagWriter::write_batch(paths, st->draft, fields);
            app->set_status("Batch: " + std::to_string(res.ok) + " ok, "
                           + std::to_string(res.error) + t(" errors"));
        }
        app->show_batch_dialog = false;
    });

    auto btn_cancel = Button(t("Cancel"), [app]() {
        app->show_batch_dialog = false;
    });

    // Focus tree: inputs FIRST so the dialog opens with cursor in the first
    // text field. Checkboxes come after (Tab past all inputs reaches them).
    // The render lambda shows them interleaved visually: [cb][label : input].
    auto inputs_col     = Container::Vertical(std::vector<Component>(inputs.begin(), inputs.end()));
    auto checkboxes_col = Container::Vertical(std::vector<Component>(checkboxes.begin(), checkboxes.end()));
    auto btn_row        = Container::Horizontal({btn_apply, btn_cancel});
    auto all            = Container::Vertical({inputs_col, checkboxes_col, btn_row});

    return Renderer(all, [app, st, checkboxes, inputs, btn_apply, btn_cancel]() {
        int n = static_cast<int>(app->selected_indices.size());
        std::string title_str = t("Batch Edit \xe2\x80\x94 ")
            + (n > 0 ? std::to_string(n) + t(" files") : t("current file"));

        // Each row: [checkbox indicator] [Label : input value]
        Elements field_rows;
        for (int i = 0; i < static_cast<int>(inputs.size()); ++i) {
            field_rows.push_back(hbox(
                checkboxes[i]->Render(),
                inputs[i]->Render() | flex
            ));
        }
        // Cover: checkbox only (cover bytes are managed via F5 in the tag editor)
        field_rows.push_back(hbox(
            checkboxes[7]->Render(),
            text(t("Cover")) | color(Color::GrayLight)
        ));

        Elements rows;
        rows.push_back(text(title_str) | bold | center);
        rows.push_back(separator());
        rows.push_back(vbox(field_rows));
        rows.push_back(separator());
        rows.push_back(hbox(btn_apply->Render(), text("  "), btn_cancel->Render()) | center);

        return vbox(rows) | border | size(WIDTH, GREATER_THAN, 55);
    });
}
