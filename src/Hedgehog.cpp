#include "Hedgehog.h"
#include "Projectile.h"
#include <iostream>
#include <cmath>

Hedgehog::Hedgehog(sf::Vector2f position, std::string& texturePath)
    : texture(),
    sprite(texture),
    isPressed(false),
    running(false),
    jumping(false),
    onGround(false),
    jump_force(200.f),
    gravity(1200.f),
    speed(0.f,0.f),
    speedRun(400.f),
    state(0),
    shootInterval(0.5f)
{
    if (!texture.loadFromFile(texturePath))
    {
        std::cout << "Texture Hedgehog not found\n";
    }

    image.loadFromFile(texturePath);

    createAlphaMask();

    int frameWidth = texture.getSize().x;
    int frameHeight = texture.getSize().y / 2;

    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setTextureRect(sf::IntRect({0,0},{frameWidth,frameHeight}));
    sprite.setScale({0.5f, 0.5f});
    sprite.setOrigin({sprite.getLocalBounds().getCenter().x,0.f});
}

void Hedgehog::createAlphaMask()
{
    auto size = image.getSize();

    alphaMask.resize(size.y,
                     std::vector<bool>(size.x,false));

    for(unsigned y=0;y<size.y;y++)
    {
        for(unsigned x=0;x<size.x;x++)
        {
            alphaMask[y][x] =
                image.getPixel({x,y}).a > 0;
        }
    }
}

sf::Sprite& Hedgehog::getSprite()
{
    return sprite;
}

const sf::Sprite& Hedgehog::getSprite() const
{
    return sprite;
}

void Hedgehog::handleEvent(const sf::Event& event,
                           const sf::RenderWindow& window)
{
    if (const auto* mousePressed =
        event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos =
                window.mapPixelToCoords(
                    {mousePressed->position.x,
                     mousePressed->position.y});

            if (sprite.getGlobalBounds().contains(mousePos))
            {
                isPressed = true;
                std::cout << "Button is pressed!\n";
            }
        }
    }
}

void Hedgehog::set_position(const sf::Vector2f& pos)
{
    sprite.setPosition(pos);
}

void Hedgehog::run()
{
    running = true;

    int frameWidth = texture.getSize().x;
    int frameHeight = texture.getSize().y / 2;

    int frameY = frameHeight;

    state = (state + 1) % 2;
    if (state == 0)
        sprite.setTextureRect(sf::IntRect({0,frameY},{frameWidth,frameHeight}));
    else
        sprite.setTextureRect(sf::IntRect({0,0},{frameWidth,frameHeight}));
}

void Hedgehog::jump(Direction direction)
{
    if (!jumping)
    {
        speed.y = -jump_force;

        if (direction == Direction::LEFT)
            speed.x = -speedRun;
        else if (direction == Direction::RIGHT)
            speed.x = speedRun;

        jumping = true;
    }
}

void Hedgehog::shoot(std::vector<Projectile>& projectiles)
{
    float startX = sprite.getPosition().x;
    float startY = sprite.getPosition().y;

    float direction =
        (sprite.getScale().x > 0) ? -1.f : 1.f;

    Projectile p({startX,startY},direction);

    projectiles.push_back(p);
}

bool Hedgehog::isCollision(sf::Sprite& otherSprite) const
{
    return sprite.getGlobalBounds()
    .findIntersection(otherSprite.getGlobalBounds())
        .has_value();
}

bool Hedgehog::pixelPerfectCollision(
    sf::Sprite& otherSprite,
    const std::vector<std::vector<bool>>& otherMask)
{
    auto intersection =
        sprite.getGlobalBounds().findIntersection(
            otherSprite.getGlobalBounds());

    if(!intersection)
        return false;

    auto overlap = *intersection;

    for(int x=overlap.position.x;
         x<overlap.position.x+overlap.size.x;
         x++)
    {
        for(int y=overlap.position.y;
             y<overlap.position.y+overlap.size.y;
             y++)
        {
            sf::Vector2f p1 =
                sprite.getInverseTransform()
                    .transformPoint({(float)x,(float)y});

            sf::Vector2f p2 =
                otherSprite.getInverseTransform()
                    .transformPoint({(float)x,(float)y});

            int ix1 = (int)p1.x;
            int iy1 = (int)p1.y;

            int ix2 = (int)p2.x;
            int iy2 = (int)p2.y;

            if(ix1 < 0 || iy1 < 0 ||
                ix2 < 0 || iy2 < 0)
                continue;

            if(iy1 >= alphaMask.size() ||
                ix1 >= alphaMask[0].size())
                continue;

            if(iy2 >= otherMask.size() ||
                ix2 >= otherMask[0].size())
                continue;

            if(alphaMask[iy1][ix1] &&
                otherMask[iy2][ix2])
                return true;
        }
    }

    return false;
}

static constexpr int MAX_RESOLVE_STEPS = 200;

void Hedgehog::resolveHorizontalCollision(sf::Sprite& otherSprite,
                                          const std::vector<std::vector<bool>>& otherMask,
                                          float oldX)
{
    if (!pixelPerfectCollision(otherSprite, otherMask))
        return;

    int direction = speed.x > 0 ? 1 : -1;

    bool separated = false;
    for (int step = 0; step < MAX_RESOLVE_STEPS; ++step)
    {
        sprite.move({-direction * 1.f, 0.f});
        if (!pixelPerfectCollision(otherSprite, otherMask))
        {
            separated = true;
            break;
        }
    }

    if (!separated)
    {
        sf::Vector2f pos = sprite.getPosition();
        sprite.setPosition({oldX, pos.y});
    }

    speed.x = 0.f;
}


void Hedgehog::resolveVerticalCollision(sf::Sprite& otherSprite,
                                        const std::vector<std::vector<bool>>& otherMask,
                                        float oldY)
{
    if (!pixelPerfectCollision(otherSprite, otherMask))
        return;

    if (speed.y > 0.f)
    {
        int steps = 0;
        while (pixelPerfectCollision(otherSprite, otherMask) && steps++ < MAX_RESOLVE_STEPS)
            sprite.move({0.f, -1.f});

        speed.y = 0.f;
        jumping = false;
        onGround = true;
    }
    else if (speed.y < 0.f)
    {
        int steps = 0;
        while (pixelPerfectCollision(otherSprite, otherMask) && steps++ < MAX_RESOLVE_STEPS)
            sprite.move({0.f, 1.f});

        speed.y = 0.f;

        if (pixelPerfectCollision(otherSprite, otherMask))
        {
            sf::Vector2f pos = sprite.getPosition();
            sprite.setPosition({pos.x, oldY});
        }
    }
    else
    {
        sf::Vector2f pos = sprite.getPosition();
        sprite.setPosition({pos.x, oldY});
    }
}

void Hedgehog::update(sf::Time& deltaTime,
                      std::vector<Projectile>& projectiles,
                      sf::Sprite& otherSprite,
                      const std::vector<std::vector<bool>>& otherMask)
{
    float dt = deltaTime.asSeconds();

    sf::Vector2f oldPos = sprite.getPosition();
    sprite.move({0, 1});
    bool groundCollision = isCollision(otherSprite) && pixelPerfectCollision(otherSprite, otherMask);
    sprite.setPosition(oldPos);
    onGround = groundCollision;
    jumping = !onGround;

    movement = {0,0};
    running = false;

    // Горизонтальное движение
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        sprite.setScale({0.5,0.5});
        movement.x -= 1;
        run();
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        sprite.setScale({-0.5,0.5});
        movement.x += 1;
        run();
    }

    // Горизонтальная скорость
    speed.x = movement.x * speedRun;

    // Прыжок
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && onGround)
    {
        speed.y = -jump_force;
        jumping = true;
        onGround = false;
    }

    // Гравитация всегда действует
    if (!onGround)
        speed.y += gravity * dt;

    // Выстрелы
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F) &&
        shootCooldown.getElapsedTime().asSeconds() > shootInterval)
    {
        shoot(projectiles);
        shootCooldown.restart();
    }

    // Перемещение по X и разрешение горизонтальной коллизии
    float oldX = sprite.getPosition().x;
    sprite.move({speed.x * dt, 0.f});

    if(isCollision(otherSprite) && pixelPerfectCollision(otherSprite, otherMask))
    {
        resolveHorizontalCollision(otherSprite, otherMask, oldX);
    }

    // Перемещение по Y и разрешение вертикальной коллизии
    float oldY = sprite.getPosition().y;
    sprite.move({0.f, speed.y * dt});

    if(isCollision(otherSprite) && pixelPerfectCollision(otherSprite, otherMask))
    {
        resolveVerticalCollision(otherSprite, otherMask, oldY);
    }
}

void Hedgehog::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

bool Hedgehog::wasClicked() const
{
    return isPressed;
}
