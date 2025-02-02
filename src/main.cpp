#include <iostream>
#include <SFML/Graphics.hpp>

class body_part {
public:
    body_part* next = NULL;
    sf::CircleShape shape;
    sf::Vector2f position = { 0.0f, 0.0f };
    sf::Vector2f velocity = { 0.0f, 0.0f };
    float speed = 1000.0f;

    body_part() {
    }

    void update(float& delta_time, sf::Vector2f& target_position) {
        if (next != NULL) {
            velocity = next->position - position;
        }
        else {
            velocity = target_position - position;
        }

        float magnitude = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (magnitude != 0) {
            velocity = velocity / magnitude;
        }

        if (magnitude > shape.getRadius()) {
            position += velocity * delta_time * speed;
        }
    }

    void draw(sf::RenderWindow& window) {
        shape.setPosition(position);
        window.draw(shape);
    }
};

class body {
    float max_radius = 100.0f;
    body_part* head = NULL;
    int length = 0;

public:

    body() {
    }

    void updateAppearance() {
        int count = 0;
        body_part* temp = head;
        while (temp) {
            count += 1;
            float radius = max_radius * count / length;
            temp->shape.setRadius(radius);
            temp->shape.setOrigin(sf::Vector2f({ radius,radius }));
            uint8_t red = 255; //* count / length;
            uint8_t green = 255 - 255 * count / length;
            uint8_t blue = 0;
            uint8_t alpha = 255;
            temp->shape.setFillColor(sf::Color({ red,green,blue,alpha }));
            temp->shape.setOutlineColor(sf::Color({ red,green,blue,alpha }));
            temp->shape.setOutlineThickness(10.0f);
            temp = temp->next;
        }
    }

    void addBodyPart() {
        float radius = 100.0f + static_cast<float>(length) * 10;
        sf::Color color = { 255,255,255,0 };
        uint8_t scale = 255 - length*50;
        sf::Color border_color = { scale,255,255,255 };
        float border_thickness = 10.0f - static_cast<float>(length);
        body_part* new_body_part = new body_part();
        if (!head) {
            head = new_body_part;
        }
        else {
            body_part* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = new_body_part;
            new_body_part->position = temp->position;
        }
        length += 1;
        this->updateAppearance();
    }

    void removeBodyPart() {
        if (!head) return;
        if (!head->next) {
            delete head;
            head = NULL;
        }
        else {
            body_part* temp = head;
            while (temp->next->next) {
                temp = temp->next;
            }
            delete temp->next;
            temp->next = NULL;
        }
        length -= 1;
        this->updateAppearance();
    }

    void update(float& delta_time, sf::Vector2f& target_position) {
        body_part* temp = head;
        while (temp) {
            temp->update(delta_time, target_position);
            temp = temp->next;
        }
    }

    void draw(sf::RenderWindow& window) {
        body_part* temp = head;
        while (temp) {
            temp->draw(window);
            temp = temp->next;
        }
    }
};

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    sf::Clock clock;

    body snake;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    snake.addBodyPart();
                }
                if (mouseButtonPressed->button == sf::Mouse::Button::Right)
                {
                    snake.removeBodyPart();
                }
            }
        }

        float delta_time = clock.restart().asSeconds();
        sf::Vector2f mouse_position = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

        snake.update(delta_time, mouse_position);

        window.clear();
        snake.draw(window);
        window.display();
    }

    return 0;
}