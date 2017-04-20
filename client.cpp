
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

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
void cardGame(int connection_fd);


int main(int argc, char * argv[])
{
    
    
    if (argc != 3)
        usage(argv[0]);
    
    connectToServer(argv[1], argv[2]);
    
    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

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
    sf::Text text("Hello SFML", font, 50);
    text.setFillColor(sf::Color::Black);


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

        // Draw the string
        window.draw(text);

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
    cardGame(connection_fd);
    
    // Close the socket
    close(connection_fd);
}


void cardGame(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    int hand[BUFFER_SIZE];
    int extraCard = 0;
    int totalPoints;
    
    while (1)
    {
        printf("Welcome to Black Jack Online \n do you wish to play? (yes = y/no = enter)");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // Finish the connection with a string containing only the '\n'
        if (strlen(buffer) == 1)
        {
            printf("Finishing the connection\n");
            break;
        }
        
        ///// SEND
        // Send a reply to the client
        if ( send(connection_fd, buffer, strlen(buffer)+1, 0) == -1 )
        {
            perror("ERROR: send");
            exit(EXIT_FAILURE);
        }
        
        // Clear the buffer
        bzero(buffer, BUFFER_SIZE);
        
        ///// RECV
        // Read the request from the client
        if(read(connection_fd,hand, BUFFER_SIZE) == -1 ){
            perror("ERROR: recv");
            exit(EXIT_FAILURE);
        }
        printf("These are your cards\nCard 1: %d \nCard 2: %d\n", hand[0], hand[1]);
        
        
        //asking to the dealer for another card
        printf("Do you wish to get another card? (yes = y / no = enter)\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        if ( send(connection_fd, buffer, strlen(buffer)+1, 0) == -1 ){
            perror("ERROR: send");
            exit(EXIT_FAILURE);
        }
        
        
        //receiving extra card
        if(strlen(buffer) > 1){
            if(read(connection_fd, &extraCard, BUFFER_SIZE) == -1){
                perror("ERROR: read");
                exit(EXIT_FAILURE);
            }
            printf("Dealer has dealt you a %d\n", extraCard);
        }
        
        //receiving total points
        if(read(connection_fd, &totalPoints, 4) == -1 ){
            perror("ERROR: read");
            exit(EXIT_FAILURE);
        }
        printf("Your total points are %d\n", totalPoints);
        
        
        //Receiving who's the winner
        if(read(connection_fd, &buffer, BUFFER_SIZE) == -1)
        {
            perror("ERROR: recv");
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n",buffer);
        printf("Do you wish to play again? (y = yes/ enter = no)");
        
        
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // Finish the connection with a string containing only the '\n'
        if (strlen(buffer) == 1)
        {
            printf("Finishing the connection\n");
            break;
        }
        
        
        
    }
}

