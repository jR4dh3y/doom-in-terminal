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
   ```bash
   cd /path/to/project
   ```
3. Create a build directory and navigate to it:
   ```bash
   mkdir build && cd build
   ```
4. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
5. Build the project:
   ```bash
   make
   ```

## Running the Game
After building the project, you can run the game using the following command:
```bash
./doom_terminal
```

## Controls
- `W`: Move forward.
- `S`: Move backward.
- `A`: Rotate left.
- `D`: Rotate right.
- `Q`: Quit the game.

## Project Structure
- `src/`: Contains the source code for the game.
- `include/`: Contains the header files.
- `build/`: Directory for build files (created during the build process).
- `CMakeLists.txt`: CMake configuration file.

## License
This project is for educational purposes and does not have a specific license. Feel free to modify and use it as needed.
