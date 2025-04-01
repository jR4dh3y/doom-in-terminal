# Terminal Doom

Terminal Doom is a simple terminal-based game inspired by classic first-person shooters. It uses the `ncurses` library for rendering and provides a basic game loop with player movement and rendering.

## Features
- First-person perspective rendering in the terminal.
- Player movement and rotation.
- Basic map rendering.

## Prerequisites
- A C++17 compatible compiler.
- CMake version 3.10 or higher.
- `ncurses` library installed on your system.

## Building the Project
1. Clone the repository or copy the project files to your local machine.
2. Navigate to the project directory:
3. Run the following commands:
   ```bash
   cmake -B build -S .
   
   cmake --build build
   ```

## Running the Game
After building the project, you can run the game using the following command(in the build dir):
```bash
./doom_terminal
```

## Controls
- `W`: Move forward.
- `S`: Move backward.
- `A`: Rotate left.
- `D`: Rotate right.
- `Q`: Quit the game.

## License
This project is for educational purposes and does not have a specific license. Feel free to modify and use it as needed.
