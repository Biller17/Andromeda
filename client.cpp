
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>



#define SERVICE_PORT 8642
#define BUFFER_SIZE 1024


void usage(char * program);
void connectToServer(char * address, char * port);
void communicationLoop(int connection_fd);

//global variables
float spaceshipPosY = 600;
float spaceshipPosX = 100;
float enemySpaceshipPosX = 100;
float asteroids [10];


int main(int argc, char * argv[])
{
    float spaceshipVel = 2;
    
   // connectToServer("255.255.255.255", "80");
    
    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1200, 900), "Andromeda");

    // Set the Icon
    /*sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "spaceship.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

     */
    
    sf::Texture spaceship;
    if (!spaceship.loadFromFile(resourcePath() + "spaceship.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite spaceshipSprite(spaceship);
    spaceshipSprite.setScale(.5, .5);
    spaceshipSprite.setPosition(spaceshipPosX, spaceshipPosY);
    
    sf::Texture enemySpaceship;
    if (!enemySpaceship.loadFromFile(resourcePath() + "enemySpaceship.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite enemySpaceshipSprite(enemySpaceship);
    enemySpaceshipSprite.setScale(.5, .5);
    enemySpaceshipSprite.setPosition(spaceshipPosX, -0);
    
    sf::Texture asteroid;
    if (!asteroid.loadFromFile(resourcePath() + "asteroid.png")) {
        return EXIT_FAILURE;
    }
    sf::Sprite asteroid1(asteroid);
    asteroid1.setScale(.2, .2);
    asteroid1.setPosition(450, 450);
    

    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "space.jpg")) {
        return EXIT_FAILURE;
    }
    
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    //set enemy score
    sf::Text enemyScore("Score: ", font, 25);
    enemyScore.setFillColor(sf::Color::Red);
    
    
    //set user score
    sf::Text userScore("Score: ", font, 25);
    userScore.setPosition(0, 725);
    userScore.setFillColor(sf::Color::White);


    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Clear screen
        window.clear();

        // Draw the sprite
        window.draw(sprite);
        
        window.draw(enemySpaceshipSprite);
        window.draw(spaceshipSprite);
        window.draw(asteroid1);
        
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            spaceshipSprite.move(spaceshipVel, 0);
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            spaceshipSprite.move(-spaceshipVel, 0);
        }
        
        
        
        // Draw the string
        window.draw(userScore);
        window.draw(enemyScore);
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}




// Show the user how to run this program
void usage(char * program)
{
    printf("Usage:\n%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}

// Establish a connection with the server indicated by the parameters
void connectToServer(char * address, char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info;
    int connection_fd;
    
    // Prepare the information to determine the local address
    // Clear all fields
    bzero(&hints, sizeof hints);
    // Use internet sockets with IPv4
    hints.ai_family = AF_INET;
    // Use connected sockets
    hints.ai_socktype = SOCK_STREAM;
    // Determine the ip of this machine automatically
    hints.ai_flags = AI_PASSIVE;
    
    ///// GETADDRINFO
    // Get the actual address of this computer
    if ( getaddrinfo(address, port, &hints, &server_info) == -1 )
    {
        perror("ERROR: getaddrinfo");
        exit(EXIT_FAILURE);
    }
    
    ///// SOCKET
    // Use the information obtained by getaddrinfo
    connection_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if ( connection_fd == -1 )
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    
    ///// CONNECT
    // Connect with the desired port
    if ( connect(connection_fd, server_info->ai_addr, server_info->ai_addrlen) == -1 )
    {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }
    
    // Release the memory of the list of addrinfo
    freeaddrinfo(server_info);
    
    // Establish the communication
    communicationLoop(connection_fd);
    
    // Close the socket
    close(connection_fd);
}


void communicationLoop(int connection_fd)
{
    
    while (1)
    {
        if(read(connection_fd, &spaceshipPosX , sizeof(float)) == -1)
        {
            perror("ERROR: recv");
            exit(EXIT_FAILURE);
        }
        
        if(write(connection_fd, &enemySpaceshipPosX, sizeof(float)) == -1)
        {
            perror("ERROR: write");
            exit(EXIT_FAILURE);
        }
        
        if(write(connection_fd, &asteroids, sizeof(float)) == -1)
        {
            perror("ERROR: write");
            exit(EXIT_FAILURE);
        }
        
        
        
        
        
    }
}

