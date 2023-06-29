#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <array>

// initialize global game window variables and booleans
const unsigned int windowWidth = 1600;
const unsigned int windowHeight = 900;
const unsigned int groundlevel = windowHeight - 100.f;
int velocity = 8; // speed of the player relative to the framerate
bool Dead = false; // boolean to check if the player is dead
bool DeadRingtone = false; // boolean to check if the audio is playing when the player dies

// create a struct for the player which will count the framerate as a diagnostic tool
// works as a tool to measure player speed relative to the framerate as well
struct FPS_Counter {
    sf::Font font;
    sf::Text text;
    sf::Clock clock; // create a clock to measure the time elapsed
    int Frame;
    int fps;
};

class FPS {
    FPS_Counter fps; // create an instance of the struct FPS_Counter
    public:
        FPS() // constructor for the FPS
        :fps() {
            if (fps.font.loadFromFile("src/rsrc/Fonts/Font.ttf")) {
                fps.text.setFont(fps.font);
            }
            fps.text.setCharacterSize(15);
            fps.text.setPosition(sf::Vector2f(fps.text.getCharacterSize() + 10.f, fps.text.getCharacterSize())); // set the position of the fps counter to the top left of the window
            fps.text.setFillColor(sf::Color(173, 216, 230)); // color the text light blue
        }

        // print the fps counter inside the window
        void update() { // update the fps counter
            if (fps.clock.getElapsedTime().asSeconds() >= 1.f) { // update the fps counter every second
                fps.fps = fps.Frame;
                fps.Frame = 0;
                fps.clock.restart();
            }
            fps.Frame++;
            fps.text.setString("FPS :- " + std::to_string(fps.fps));
           // std::cout  << fps.fps << std::endl; // debugging
        }

        void drawTo(sf::RenderWindow& window) {
            window.draw(fps.text); // draw the fps counter to the window
        }
};

// create a class which will load the sounds for events in the game from the sounds folder
// initializes buffers to store the sound data in arrays, and the sounds
class Sounds {
    public:
        sf::SoundBuffer deathBuffer; // create a buffer to store the sound data
        sf::SoundBuffer pointsBuffer;
        sf::SoundBuffer jumpBuffer;
        sf::Sound deathSound; // create a sound to play the sound data
        sf::Sound pointsSound;
        sf::Sound jumpSound;

        Sounds() // constructor for the sounds
        :deathBuffer(), pointsBuffer(), jumpBuffer(), deathSound(), pointsSound(), jumpSound() {
            deathBuffer.loadFromFile("src/rsrc/Sounds/death.wav"); // load the sound data from the sounds folder
            pointsBuffer.loadFromFile("src/rsrc/Sounds/points.wav");
            jumpBuffer.loadFromFile("src/rsrc/Sounds/jump.wav");

            deathSound.setBuffer(deathBuffer); // set the sound buffer to the sound data
            pointsSound.setBuffer(pointsBuffer);
            jumpSound.setBuffer(jumpBuffer);
        }
};

// create a class for the ground level which will be displayed at the bottom of the window
class Dirt {
    public:
        sf::Sprite dirtSprite; // create a sprite for the dirt
        sf::Texture dirtTexture;
        int level{0}; // initialize the level of the dirt

        Dirt() // constructor for the dirt
        :dirtSprite(), dirtTexture() {
            if (dirtTexture.loadFromFile("src/rsrc/Images/grounds.png")) {
                dirtSprite.setTexture(dirtTexture);
                dirtSprite.setPosition(sf::Vector2f(0.f, windowHeight - dirtTexture.getSize().y + 10)); // set the position of the dirt sprite to the bottom of the window
            }
        }

        void updateDirt() { // update the dirt sprite to move across the window
            if (!Dead) { // player is alive
                if (level > dirtTexture.getSize().x - windowWidth) { 
                    level = 0;
                }
                level += velocity; 
                dirtSprite.setTextureRect(sf::IntRect(level, 0, windowWidth, windowHeight)); // set the texture rectangle to the level of the dirt
            }
            if (Dead) { // player is dead
                dirtSprite.setTextureRect(sf::IntRect(level, 0, windowWidth, windowHeight)); // reset the texture rectangle to the level of the dirt
            }
        }

        void reset() {
            level = 0;
            dirtSprite.setTextureRect(sf::IntRect(0, 0, windowWidth, windowHeight)); // reset the dirt sprite to the beginning of the texture
        }
};

class Barrier { // create class for the barriers on the field, and the bounds of the barriers
    public:
        sf::Sprite barrierSprite; // create a sprite for the barrier
        sf::FloatRect bBounds{0.f, 0.f, 0.f, 0.f}; // create a float rectangle for the bounds of the barrier

        Barrier(sf::Texture& Btexture) // constructor for the barrier
        :barrierSprite(), bBounds() {
            barrierSprite.setTexture(Btexture);
            barrierSprite.setPosition(sf::Vector2f(windowWidth, groundlevel)); // set the position of the barrier to the right of the window
        }
};

class Barriers { // controls the display of barriers and random placement
    public:
        std::vector<Barrier> barriers; // create a vector of barriers
        sf::Time sTimer; // spawn timer
        sf::Texture bTexture1;
        sf::Texture bTexture2;
        sf::Texture bTexture3;
        // sf::Texture bTexture4;
        int random{0}; // initialize random number for barrier placement

        Barriers() // constructor for the barriers
        :sTimer(sf::Time::Zero) { // initialize the spawn timer to zero
            barriers.reserve(5); // reserve 5 barriers in the vector

            if (bTexture1.loadFromFile("src/rsrc/Images/sillycat.png")) { // load the textures for the barriers
                std::cout << "barrier 1 loaded" << std::endl; // debugging
            }
            if (bTexture2.loadFromFile("src/rsrc/Images/Cactus2.png")) {
                std::cout << "barrier 2 loaded" << std::endl;
            }
            if (bTexture3.loadFromFile("src/rsrc/Images/Cactus3.png")) {
                std::cout << "barrier 3 loaded" << std::endl;
            }
        }

        void update(sf::Time& dt) { // update the barriers
            sTimer += dt; // increment the spawn timer
            if (sTimer.asSeconds() > 0.5f + velocity / 8) { // elapsed time of the spawn timer is greater than the threshold
                random = (rand() % 3) + 1; // generate a random integer between 1 and 3 inclusively for the barrier placement

                if (random == 1) {
                    barriers.emplace_back(Barrier(bTexture1)); // add a barrier to the vector
                }
                if (random == 2) {
                    barriers.emplace_back(Barrier(bTexture2));
                }
                if (random == 3) {
                    barriers.emplace_back(Barrier(bTexture3));
                }
               /* if (random == 4) {
                    barriers.emplace_back(Barrier(bTexture4));
                } */
                sTimer = sf::Time::Zero; // reset the spawn timer
            }

            if (!Dead) { // player is alive
                for (int i = 0; i < barriers.size(); i++) { // iterate through the barriers
                    barriers[i].bBounds = barriers[i].barrierSprite.getGlobalBounds(); // set the bounds of the barrier
                    barriers[i].bBounds.width -= 5.f; // adjust the width of the bounds
                    barriers[i].barrierSprite.move(-1 * velocity, 0.f); // move the barrier to the left

                    if (barriers[i].barrierSprite.getPosition().x < -150.f) { // barrier is off the screen
                        std::vector<Barrier>::iterator it = barriers.begin() + i; // create an iterator to the barrier
                        barriers.erase(it); // erase the barrier
                    }
                }
            }
            if (Dead) { // player is dead
                for (auto& barriers : barriers) { // iterate through the barriers
                    barriers.barrierSprite.move(0.f, 0.f); // stop the barriers from moving
                }
            }
        }
        
        void drawTo(sf::RenderWindow& window) { // draw the barriers to the window
            for (auto& barriers : barriers) {
                window.draw(barriers.barrierSprite);
            }
        }

        void reset() {
            barriers.erase(barriers.begin(), barriers.end()); // erase all barriers
        }
};

// create the class for the player and the player's actions
class Player {
    public:
        sf::Sprite player;
        sf::Vector2f pPos{0.f, 0.f}; // player position
        sf::Vector2f pVel{0.f, 0.f}; // player velocity
        sf::Texture pTexture;
        sf::FloatRect pBounds; // player bounds
        Sounds sound; // player sounds
        std::array<sf::IntRect, 6> pFrames; // player frames of sprite sheet, needs a new sprite sheet
        sf::Time pTime; // player time
        int pFrameIndex{0}; // player frame index

        Player() // constructor for the player
        :player(), pTexture(), sound(), pTime() {
            if (pTexture.loadFromFile("src/rsrc/Images/PlayerSpriteSheet2.png")) { // load the player sprite
                player.setTexture(pTexture);
                for (int i = 0; i < pFrames.size(); i++) {
                    pFrames[i] = sf::IntRect(i * 0, 0, 80, 144); // set the frames of the player sprite sheet
                }
                player.setTextureRect(pFrames[0]); // set the player sprite to the first frame
                player.setPosition(sf::Vector2f(100.f, windowHeight - 250)); // set the player position to the left of the window
                pPos = player.getPosition();
            } else { // when player sprite cannot load
                std::cout << "player sprite cannot load" << std::endl; // debugging
            }
        }

        void update(sf::Time& dt, std::vector<Barrier>& barriers) { // update the player and the player's actions
            pPos = player.getPosition();
            pBounds = player.getGlobalBounds(); // set the bounds of the player
            pBounds.height -= 1.f; // adjust the height of the bounds
            pBounds.width -= 5.f; // adjust the width of the bounds
            pTime += dt; // increment the player time

            for (auto& barriers : barriers) { // iterate through the barriers
                if (pBounds.intersects(barriers.bBounds)) { // player intersects with a barrier
                    Dead = true; // player is dead
                }
            }

            if (!Dead) { // player is alive
                move(); // move the player on the screen

                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) == true && pPos.y >= windowHeight - 150.f) { // player presses space and is on the ground
                    pFrameIndex = 0; // set the player frame index to 0
                    pVel.y = -20.f; // update the player velocity
                    player.setTextureRect(pFrames[1]); // set the player sprite to the second frame
                    sound.jumpSound.play(); // play the jump sound effect
                }
            
                if (pPos.y < windowHeight - 150.f) { // player is in the air
                    pVel.y += 1.f;
                    player.setTextureRect(pFrames[1]); // set the player sprite to the second frame
                }

                if (pPos.y > windowHeight - 150.f) { // player is on the ground
                    player.setPosition(sf::Vector2f(player.getPosition().x, windowHeight - 150.f)); // jump height is limited
                    pVel.y = 0.f;
                }

                player.move(pVel); // move the player
            }

            if (Dead) { // player is dead
                pVel.y = 0.f; // stop the player from moving
                player.setTextureRect(pFrames[3]); // set the player sprite to the fourth frame
                
                if (pTime.asMilliseconds() > 170.f) {  
                    sound.deathSound.stop(); // stop the death sound effect
                    sound.deathSound.setLoop(false);
                    pTime = sf::Time::Zero; // reset the player time
                } else {
                    sound.deathSound.setVolume(30);
                    sound.deathSound.play(); // play the death sound effect
                }
            }
        }

        void move() { // move the player on the screen
            for (int i = 0; i <pFrames.size() - 3; i++) { // iterate through the player frames
                if (pFrameIndex == (i + 1) * 3) { // set the player sprite to the next frame
                    player.setTextureRect(pFrames[i]);
                }
                if (pFrameIndex >= (pFrames.size() - 2) * 3) { // reset the player frame index
                    pFrameIndex = 0;
                }
                pFrameIndex++; // increment the player frame index
            }
        }

        void reset() { // reset the player
            pVel.y = 0.f;
            player.setPosition(sf::Vector2f(player.getPosition().x, windowHeight - 400)); // animate player dropping from the sky
            player.setTextureRect(pFrames[0]); // set the player sprite to the first frame
        }
};

// create the class for the scores that the player earns throughout the game
class Scores {
    public:
        sf::Text lastScoreText; // text for the last score
        sf::Text highScoreText; // text for the highest score achieved in the game session for the current window
        sf::Text scoreText; // text for the current score
        sf::Font scorefont;
        Sounds sound; 
        short score{0}; // current score
        short lastScore{0}; // last score
        short ScoreIndex{0}; // index for the score
        short ScoreGap{0}; // gap between the scores
        short firstScore; 

        Scores() // constructor for the scores
        :scorefont(), scoreText(), lastScoreText(), firstScore(), sound() {
            if (scorefont.loadFromFile( "src/rsrc/Fonts/Font.ttf")) {
                scoreText.setFont(scorefont);
                scoreText.setCharacterSize(15);
                scoreText.setPosition(sf::Vector2f(windowWidth / 2 + windowWidth / 4 + 200.f, scoreText.getCharacterSize() + 10.f)); // set the position of the score text
                scoreText.setFillColor(sf::Color(173, 216, 230)); // text color is light blue

                lastScoreText.setFont(scorefont);
                lastScoreText.setCharacterSize(15);
                lastScoreText.setPosition(sf::Vector2f(scoreText.getPosition().x - 250.f, scoreText.getPosition().y)); // set the position of the last score text
                lastScoreText.setFillColor(sf::Color(173, 216, 230)); // light blue color

                highScoreText.setFont(scorefont);
                highScoreText.setCharacterSize(15);
                highScoreText.setPosition(sf::Vector2f(lastScoreText.getPosition().x + 50.f, scoreText.getPosition().y)); // set the position of the highest score text
                highScoreText.setFillColor(sf::Color(173, 216, 230)); // light blue color
            }
            highScoreText.setString("HIGH SCORE "); // set the text for the highest score
            firstScore = 0; // initialize the first score
        }

        void update() { // update the scores
            if (!Dead) { // player is alive
                ScoreIndex++;
                // add score to the player as long as they are alive
                if (ScoreIndex >= 5) { // update the score every 5 frames
                    ScoreIndex = 0;
                    score++; // update the player score and reset the score index back to 0
                }

                ScoreGap = score - firstScore; // calculate the score gap

                if (ScoreGap > 100) { // increase the player speed and award points
                    firstScore += 100;
                    velocity += 1;
                    sound.pointsSound.play(); // play a sound to alert player speed has increased and points awarded
                }
                
                scoreText.setString(std::to_string(score)); // set the score text to the current score
                lastScoreText.setString(std::to_string(lastScore)); // set the last score text to the last score
            }
        }

        void reset() {
            // reset the scores when player dies and show the previous score
            if (score > lastScore) { // update the last score if the current score is greater than the last score
                lastScore = score;
            }
            if (score < lastScore) { // keep the last score if the current score is less than the last score
                lastScore = lastScore;
            }   
            lastScoreText.setString(std::to_string(lastScore)); // set the last score text to the last score
            score = 0; // reset the score
        }
};


// create the class for restarting the game after the player dies
class TryAgain {
    public:
        sf::Sprite tryAgainSprite; // sprite for the try again button
        sf::FloatRect tryAgainBounds;
        sf::Texture tryAgainTexture;
        sf::Vector2f mousePos; // mouse position
        bool checkPressed{false}; // check if the button is pressed

        TryAgain() // constructor for the try again button
        :tryAgainSprite(), tryAgainTexture(), mousePos(0.f, 0.f), tryAgainBounds() {
            if (tryAgainTexture.loadFromFile("src/rsrc/Images/dog.png")) { // load the try again button texture
                tryAgainSprite.setTexture(tryAgainTexture);
                tryAgainSprite.setPosition(sf::Vector2f(windowWidth / 2 - tryAgainTexture.getSize().x / 2, windowHeight / 2)); // set the position of the try again button
                tryAgainBounds = tryAgainSprite.getGlobalBounds(); // get the bounds of the try again button
            }
        }
};

// create the class for the background of the game window, displaying the sky and clouds
class Sky {
    public:
        std::vector<sf::Sprite> sky; // vector of sprites for the sky
        sf::Time currentTime; // current time
        sf::Texture skyTexture;
        std::random_device randDevice; // random device for generating random numbers
        std::mt19937 rng{randDevice()}; // random number generator

        Sky() // constructor for the sky
        :sky(), skyTexture(), currentTime(), randDevice() {
            if (skyTexture.loadFromFile("src/rsrc/Images/Cloudz.png")) { // load the sky texture
                std::cout << "loaded sky texture" << std::endl; // print to console if the sky texture is loaded (debugging purposes)
            }
            sky.reserve(4); // reserve 4 sprites for the sky
            sky.emplace_back(sf::Sprite(skyTexture));
            sky.back().setPosition(sf::Vector2f(windowWidth, windowHeight / 2 - 140.f)); // set the position of the first sky sprite
        }

        void updatesky(sf::Time& dt) { // update the sky
            currentTime += dt; // update the current time
            if (currentTime.asSeconds() > 8.f) { // add a new sky sprite every 8 seconds
                sky.emplace_back(sf::Sprite(skyTexture));
                std::uniform_int_distribution<std::mt19937::result_type> dist6(windowHeight / 2 - 200, windowHeight / 2 - 50); // generate uniformly distributed random integers within a specified range
                sky.back().setPosition(sf::Vector2f(windowWidth, dist6(rng))); // set the position of the new sky sprite
                currentTime = sf::Time::Zero; // reset the current time
            }

            for (int i = 0; i < sky.size(); i++) { // move the sky sprites
                if (!Dead) { // player is alive
                    sky[i].move(sf::Vector2f(-1.f, 0.f)); // move the sky sprites to the left
                }
                if (Dead) { // player is dead
                    sky[i].move(sf::Vector2f(-0.5f, 0.f)); // move the sky sprites to the left, but slower
                }
                if (sky[i].getPosition().x < 0.f - skyTexture.getSize().x) { // delete the sky sprites when they go off the screen
                    std::vector<sf::Sprite>::iterator skyIt = sky.begin() + i; // iterator for the sky sprites
                    sky.erase(skyIt); // erase the sky sprites
                }
                
            }
        }

        void drawTo(sf::RenderWindow& window) { // draw the sky sprites to the window
            for (auto& sky : sky) {
                window.draw(sky);
            }
        }
};

// create the class for the game, incorporating all the other classes
class Game {
    public:
        FPS fps; // instantiate the fps, player, dirt, barriers, scores, sky and try again classes
        Player player;
        Dirt dirt;
        Barriers barriers;
        Scores score;
        Sky sky;
        TryAgain tryAgain;
        sf::Font Gfont; // font for the game over text
        sf::Text Gtext; // text for the game over text
        sf::Vector2f mousePos{0.f, 0.f}; // mouse position

        Game() // constructor for the game
        :fps(), player(), dirt(), barriers(), score(), sky(), Gfont(), Gtext() {
            Gfont.loadFromFile("src/rsrc/Fonts/Font.ttf");
            Gtext.setFont(Gfont);
            player.player.setPosition(sf::Vector2f((windowWidth / 2) - (windowWidth / 4), windowHeight / 2 )); // set the position of the player
            Gtext.setString("You Died :("); // set the game over text to be displayed when the player dies
            Gtext.setPosition(sf::Vector2f(tryAgain.tryAgainSprite.getPosition().x - Gtext.getCharacterSize(), tryAgain.tryAgainSprite.getPosition().y - 50)); // set the position of the game over text
            Gtext.setFillColor(sf::Color( 173, 216, 230 )); // light blue color for the game over text
        }

        void setMousePos(sf::Vector2i p_mousePos) { // set the mouse position
            mousePos.x = p_mousePos.x;
            mousePos.y = p_mousePos.y;
        }

        void update(sf::Time dt) { // update the game
            tryAgain.checkPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left); // check if the try again button is pressed
            if (Dead && tryAgain.tryAgainBounds.contains(mousePos) && tryAgain.checkPressed) { // if the player is dead and the try again button is pressed
                dirt.reset(); // reset the dirt, barriers, player and score
                barriers.reset();
                player.reset();
                score.reset();
                Dead = false; // set the player to be alive
            } else { // if the player is alive, continue updating the game
                dirt.updateDirt(); // update the dirt, barriers, player, sky and score
                barriers.update(dt);
                player.update(dt, barriers.barriers);
                sky.updatesky(dt);
                score.update();
            }
            fps.update(); // update the fps
        }

        void drawTo(sf::RenderWindow& window) { // draw the game to the window
            if (Dead) { // if the player is dead, draw the game over text and try again button
                sky.drawTo(window);
                window.draw(dirt.dirtSprite);
                barriers.drawTo(window);
                window.draw(score.scoreText);
                window.draw(score.lastScoreText);
                window.draw(score.highScoreText);
                window.draw(player.player);
                window.draw(Gtext);
                window.draw(tryAgain.tryAgainSprite);
                fps.drawTo(window);    
            } else { // if the player is alive, draw the game as normal
                sky.drawTo(window);
                window.draw(dirt.dirtSprite);
                barriers.drawTo(window);
                window.draw(score.scoreText);
                window.draw(score.lastScoreText);
                window.draw(score.highScoreText);
                window.draw(player.player);
                fps.drawTo(window);
            }
        }
};

int main() { // main function 
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Spoink"); // create the window, labeled "Spoink" as the game title
    window.setFramerateLimit(100); // set the framerate limit to 100
    window.setVerticalSyncEnabled(true); // enable vertical sync

    Game game; // instantiate the game class
    sf::Event event;
    sf::Time dt;
    sf::Clock clock;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { // if the window is closed, close the window
                window.close();
            }
            game.setMousePos(sf::Mouse::getPosition(window)); // set the mouse position
        }
        dt = clock.restart();
        game.update(dt); // update the game with the time
        window.clear(sf::Color(191, 161, 207, 0)); // clear the window with a light purple color
        game.drawTo(window); // draw the game to the window
        window.display(); // display the window with the game
    }
}
