# Spoink
Spoink is a 2-dimensional vertical platformer that involves a single player jumping over obstacles. As the player successfully jumps, the game will slowly become faster and the player's score increases. The player dies when coming into contact with an obstacle, ending the game session. The game is filled with audio cues when a player jumps, meets a point threshold, and dies. Additionally, clouds are randomly generated in the game to incorporate some visual flair, as well as a custom window. A current score counter and a high score meter is located at the top right, while a FPS counter is located at the top left. The game is compiled using Makefile, and the SFML libraries are included within the package.

## How to Play Spoink
Press the spacebar to jump. It is important to time your jumps well in order to avoid the obstacles and earn more points. The further you go, the more points you will receive, however, the game will become faster.
The game will stop when you encounter an obstacle, at which you will see an option to restart the game by pressing the dog.
Close the game by exiting from the window.

## Running Spoink
As a prerequisite, install minGW-w64 on your computer. You can use [Homebrew](https://brew.sh/) to install this software development environment in your terminal by entering:

```brew install mingw-w64```

Then, enter the following in your terminal to compile the code.

```mingw32-make -f MakeFile```

Once the code is compiled, run the created executable in the terminal.

```"\.Spoink.exe"```
