
#include <SFML/Graphics.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <optional>

#include "portable-file-dialogs.h"

int main(int argc, char* argv[]) {
    std::string filename;

    if (argc >= 2) {
        filename = argv[1];
    } else {
        auto dialog = pfd::open_file(
            "Select DS Script",
            "",
            { "DS Script", "*.ds" }
        );

        auto result = dialog.result();

        if (result.empty()) {
            return 1;
        }

        filename = result[0];
    }

    std::ifstream file(filename);

    if (!file.is_open()) {
        return 1;
    }

    std::vector<std::pair<std::string, std::string>> dialogues;
    std::string line;

    while (std::getline(file, line)) {
        size_t space = line.find(' ');

        if (space != std::string::npos &&
            space + 1 < line.size() &&
            line[space + 1] == '"') {

            std::string name = line.substr(0, space);
            std::string text = line.substr(space + 2);

            if (!text.empty() && text.back() == '"') {
                text.pop_back();
            }

            dialogues.push_back({name, text});
        }
    }

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "d2s1",
        sf::Style::Close
    );

    sf::Font font;

    if (!font.openFromFile("arial.ttf")) {
        if (!font.openFromFile(
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
        )) {
            return 1;
        }
    }

    sf::Text nameText(font);
    sf::Text dialogueText(font);
    sf::Text continueText(font);

    nameText.setCharacterSize(24);
    dialogueText.setCharacterSize(20);
    continueText.setCharacterSize(16);

    nameText.setFillColor(sf::Color::Yellow);
    dialogueText.setFillColor(sf::Color::White);
    continueText.setFillColor(sf::Color::Green);

    nameText.setPosition({20.f, 500.f});
    dialogueText.setPosition({20.f, 540.f});
    continueText.setPosition({600.f, 570.f});

    continueText.setString("[Enter]");

    size_t currentIndex = 0;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed =
                event->getIf<sf::Event::KeyPressed>()) {

                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    currentIndex++;

                    if (currentIndex >= dialogues.size()) {
                        window.close();
                    }
                }
            }
        }

        if (currentIndex < dialogues.size()) {
            nameText.setString(dialogues[currentIndex].first);
            dialogueText.setString(dialogues[currentIndex].second);
        }

        window.clear(sf::Color(30, 30, 60));

        window.draw(nameText);
        window.draw(dialogueText);
        window.draw(continueText);

        window.display();
    }

    return 0;
}

