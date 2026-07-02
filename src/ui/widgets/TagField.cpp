#include "TagField.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Component make_tag_field(const std::string& label,
                                 std::string* value,
                                 bool digits_only,
                                 int  label_width,
                                 std::function<void()> on_change)
{
    InputOption opt;
    if (digits_only) {
        opt.on_change = [value, on_change]() {
            std::string filtered;
            for (char c : *value)
                if (std::isdigit(static_cast<unsigned char>(c)))
                    filtered += c;
            *value = std::move(filtered);
            if (on_change) on_change();
        };
    } else if (on_change) {
        opt.on_change = on_change;
    }

    auto placeholder = std::make_shared<std::string>();
    auto input = Input(value, *placeholder, opt);

    // Capture placeholder so it stays alive as long as the component does.
    return Renderer(input, [label, label_width, input, placeholder]() {
        return hbox({
            text(label) | size(WIDTH, EQUAL, label_width),
            text(" : "),
            input->Render() | flex,
        });
    });
}
