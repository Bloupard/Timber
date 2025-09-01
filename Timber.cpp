// Timber.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

//Declaration de fonctions
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
//Sprite branches[NUM_BRANCHES];

enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];

int main()
{
    // Create a video mode object
    VideoMode vm({ 1920, 1080 });
    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", State::Fullscreen);

    // Create a texture to hold a graphic on the GPU
    sf::Texture textureBackground;
    // Load a graphic into the texture
    textureBackground.loadFromFile("graphics/background.png");
    // Create a sprite
    Sprite spriteBackground(textureBackground);
    // Attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);
    // Set the spriteBackground to cover the screen
    spriteBackground.setPosition({ 0.f,0.f });

    // Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Sprite spriteTree(textureTree);
    spriteTree.setPosition({ 810.f, 0.f });

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");
    Sprite spriteBee(textureBee);
    spriteBee.setPosition({ 0.f, 800.f });
    // Is the bee currently moving?
    bool beeActive = false;
    // How fast can the bee fly
    float beeSpeed = 0.0f;

    // make 3 cloud sprites from 1 texture
    Texture textureCloud;
    // Load 1 new texture
    textureCloud.loadFromFile("graphics/cloud.png");
    // 3 New sprites with the same texture
    Sprite spriteCloud1(textureCloud);
    Sprite spriteCloud2(textureCloud);
    Sprite spriteCloud3(textureCloud);
    // Position the clouds on the left of the screen
    // at different heights
    spriteCloud1.setPosition({ 0.f, 0.f });
    spriteCloud2.setPosition({ 0.f, 250.f });
    spriteCloud3.setPosition({ 0.f, 500.f });
    // Are the clouds currently on screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;
    // How fast is each cloud?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    // Variables to control time itself
    Clock clock;

    //Barre de temps
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition({ (1920 / 2.0f) - timeBarStartWidth / 2.0f, 980 });

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    //Pour traquer lorsque le jeu se joue
    bool paused = true;

    //Dessiner du texte
    int score = 0;

    //Ajout des polices
    Font font("fonts/KOMIKAP_.ttf");
    Text messageText(font, "Appuie sur ENTREE pour demarrer");
    Text scoreText(font, "Score = 0");

    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    //Positionner le texte
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.getCenter());
    messageText.setPosition({ 1920 / 2.0f, 1080 / 2.0f });

    scoreText.setPosition({ 20, 20 });

    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");
    Sprite *branches[NUM_BRANCHES];

    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i] = new Sprite(textureBranch);
        branches[i]->setPosition({ -2000, -2000 });
        branches[i]->setOrigin({ 220, 20 });
    }

    updateBranches(1);
    updateBranches(2);
    updateBranches(3);
    updateBranches(4);
    updateBranches(5);

    // Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer(texturePlayer);
    spritePlayer.setPosition({ 580, 720 });
    // The player starts on the left
    side playerSide = side::LEFT;
    // Prepare the gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/rip.png");
    Sprite spriteRIP(textureRIP);
    spriteRIP.setPosition({ 600, 860 });
    // Prepare the axe
    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    Sprite spriteAxe(textureAxe);
    spriteAxe.setPosition({ 700, 830 });
    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;
    // Prepare the flying log
    Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    Sprite spriteLog(textureLog);
    spriteLog.setPosition({ 810, 720 });
    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    //controle des input joueur
    bool acceptInput = false;

    // Prepare the sounds
    // The player chopping sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop(chopBuffer);
    // The player has met his end under a branch
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    Sound death(deathBuffer);
    // Out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime(ootBuffer);

    while (window.isOpen())
    {
        
        //****************************************
        //Handle the players input
        //****************************************
        
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::KeyReleased>() && !paused)
            {
                // Listen for key presses again
                acceptInput = true;
                // hide the axe
                spriteAxe.setPosition({ 2000, spriteAxe.getPosition().y });
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            window.close();
        }

        //Demarre le jeu
        if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
        {
            paused = false;

            //reset time et score
            score = 0;
            timeRemaining = 6;

            for (int i = 1; i < NUM_BRANCHES; i++)
            {
                branchPositions[i] = side::NONE;
            }
            // Make sure the gravestone is hidden
            spriteRIP.setPosition({ 675, 2000 });
            // Move the player into position
            spritePlayer.setPosition({ 580, 720 });
            acceptInput = true;
        }

        // Wrap the player controls to
        // Make sure we are accepting input
        if (acceptInput)
        {
            // First handle pressing the right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Key::Right))
            {
                // Make sure the player is on the right
                playerSide = side::RIGHT;
                score++;
                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;
                spriteAxe.setPosition({ AXE_POSITION_RIGHT,spriteAxe.getPosition().y });
                spritePlayer.setPosition({ 1200, 720 });
                // Update the branches
                updateBranches(score);
                // Set the log flying to the left
                spriteLog.setPosition({ 810, 720 });
                logSpeedX = -5000;
                logActive = true;
                acceptInput = false;

                // Play a chop sound
                chop.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::Left))
            {
                // Make sure the player is on the left
                playerSide = side::LEFT;
                score++;
                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;
                spriteAxe.setPosition({ AXE_POSITION_LEFT,spriteAxe.getPosition().y });
                spritePlayer.setPosition({ 580, 720 });
                // update the branches
                updateBranches(score);
                // set the log flying
                spriteLog.setPosition({ 810, 720 });
                logSpeedX = 5000;
                logActive = true;
                acceptInput = false;

                // Play a chop sound
                chop.play();
            }
        }

        
        //****************************************
        //Update the scene
        //****************************************
        

        if (!paused)
        {

            // Measure time
            Time dt = clock.restart();

            //soustraction pour le temps restant
            timeRemaining -= dt.asSeconds();
            //Resize de la barre
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f)
            {
                // Pause the game
                paused = true;
                // Change the message shown to the player
                messageText.setString("Fin du temps!!");
                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());
                messageText.setPosition({ 1920 / 2.0f, 1080 / 2.0f });

                // Play the out of time sound
                outOfTime.play();
            }

            // Setup the bee
            if (!beeActive)
            {
                // How fast is the bee
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200.0f;
                // How high is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500.0f;
                spriteBee.setPosition({ 2000, height });
                beeActive = true;
            }
            else // Move the bee
            {
                spriteBee.setPosition({ spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y });
                // Has the bee reached the left-hand edge of the screen?
                if (spriteBee.getPosition().x < -100)
                {
                    // Set it up ready to be a whole new bee next frame
                    beeActive = false;
                }
            }

            //Manage the clouds
            //Cloud 1
            if (!cloud1Active)
            {
                // How fast is the cloud
                srand((int)time(0) * 10);
                cloud1Speed = (rand() % 200) + 0.0f;
                // How high is the cloud
                srand((int)time(0) * 10);
                float height = (rand() % 150) + 0.0f;
                spriteCloud1.setPosition({ -200, height });
                cloud1Active = true;
            }
            else // Move the cloud
            {
                spriteCloud1.setPosition({ spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y });
                // Has the cloud reached the left-hand edge of the screen?
                if (spriteCloud1.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new bee next frame
                    cloud1Active = false;
                }
            }

            //Cloud 2
            if (!cloud2Active)
            {
                // How fast is the cloud
                srand((int)time(0) * 20);
                cloud2Speed = (rand() % 200) + 0.0f;
                // How high is the cloud
                srand((int)time(0) * 20);
                float height = (rand() % 300) - 150.0f;
                spriteCloud2.setPosition({ -200, height });
                cloud2Active = true;
            }
            else // Move the cloud
            {
                spriteCloud2.setPosition({ spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y });
                // Has the cloud reached the left-hand edge of the screen?
                if (spriteCloud2.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new bee next frame
                    cloud2Active = false;
                }
            }

            //Cloud 3
            if (!cloud3Active)
            {
                // How fast is the cloud
                srand((int)time(0) * 30);
                cloud3Speed = (rand() % 200) + 0.0f;
                // How high is the cloud
                srand((int)time(0) * 30);
                float height = (rand() % 450) - 150.0f;
                spriteCloud3.setPosition({ -200, height });
                cloud3Active = true;
            }
            else
            {
                spriteCloud3.setPosition({ spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y });
                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud3.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new cloud next frame
                    cloud3Active = false;
                }
            }

            //MAJ du texte de score
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150.f;
                if (branchPositions[i] == side::LEFT)
                {
                    branches[i]->setPosition({ 610, height });
                    branches[i]->setRotation(degrees(180));
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    branches[i]->setPosition({ 1330, height });
                    branches[i]->setRotation(degrees(0));
                }
                else
                {
                    branches[i]->setPosition({ 3000, height });
                }
            }

            // Handle a flying log
            if (logActive)
            {
                spriteLog.setPosition({ spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()) });
                // Has the log reached the right hand edge?
                if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000)
                {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition({ 810, 720 });
                }
            }

            // has the player been squished by a branch?
            if (branchPositions[5] == playerSide)
            {
                // death
                paused = true;
                acceptInput = false;
                // Draw the gravestone
                spriteRIP.setPosition({ 525, 760 });
                // hide the player
                spritePlayer.setPosition({ 2000, 660 });
                // Change the text of the message
                messageText.setString("ECRASE!!");
                // Center it on the screen
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());
                messageText.setPosition({ 1920 / 2.0f, 1080 / 2.0f });

                // Play the death sound
                death.play();
            }
        } //End if(!paused)

        
        //****************************************
        //Draw the scene
        //****************************************
        
        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);
        // Draw the clouds
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            window.draw(*branches[i]);
        }
        // Draw the tree
        window.draw(spriteTree);

        // Draw the player
        window.draw(spritePlayer);
        // Draw the axe
        window.draw(spriteAxe);
        // Draw the flying log
        window.draw(spriteLog);
        // Draw the gravestone
        window.draw(spriteRIP);
        // Draw the insect
        window.draw(spriteBee);

        window.draw(scoreText);

        window.draw(timeBar);

        if (paused)
        {
            window.draw(messageText);
        }

        // Show everything we just drew
        window.display();
    }
    return 0;
}

void updateBranches(int seed)
{
    for (int j = NUM_BRANCHES - 1; j > 0; j--)
    {
        branchPositions[j] = branchPositions[j - 1];
    }

    srand((int)time(0) + seed);
    int r = (rand() % 5);
    switch (r) {
    case 0:
        branchPositions[0] = side::LEFT;
        break;
    case 1:
        branchPositions[0] = side::RIGHT;
        break;
    default:
        branchPositions[0] = side::NONE;
        break;
    }
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
