# OpenGL 3D Golf Simulator

## Overview
The **OpenGL 3D Golf Simulator** is a 3D simulation game developed using OpenGL and C++. The player must shoot golf balls from four different positions and time their shots accurately to land the balls in the goal (flag). The game features five 3D models, dynamic wall colors, and various controls for player movement and interaction.

## Game Mechanics

In **Golf Simulator**, the player controls a golfer who must shoot golf balls from different positions within a time limit. The game includes several features:
- **Golf Balls**: The player must time their shots using the moving cursor at the top of the screen.
- **Goal (Flag)**: The goal is to get the golf balls into the flag before time runs out.
- **Movement**: The player can move their character and adjust their orientation to aim for the target.
- **Time Limit**: The game has a countdown timer. If not all balls are in the goal before time runs out, the game ends in failure.

### 3D Models
The scene contains five 3D models:
1. **Tiger**: The player character, representing the golfer.
2. **Pond**: A decorative obstacle in the scene.
3. **Golf Car**: A part of the scenery that adds to the environment's realism.
4. **Golf Bag**: Used to represent the player's equipment.
5. **Flag (Goal)**: The target area where the golf balls must land to win the game.

### Dynamic Walls
- The walls of the scene periodically change colors, adding a dynamic element to the environment.

## Controls

- **SPACE**: Shoot the golf ball. Press this button while the cursor is aligned with the target to time the shot correctly.
- **+**: Move the player forward.
- **-**: Move the player backward.
- **O**: Change the player's orientation to adjust aim.
- **P**: Activate animations, adding movement and effects to the scene.
- **ESC**: Exit the game.

## Scene and Gameplay Flow

- The game starts with the player positioned at one of the four shooting locations.
- The player must time their shots accurately by pressing **SPACE** when the moving cursor aligns with the target.
- The scene contains a flag (goal) where the player must aim and get all the balls in before time runs out.
- If the player manages to get all balls in the goal within the time limit, they win the game.
- If not all balls are in the goal before the time ends, the game will end in a loss.
- When the game ends, the scene is replaced with an appropriate message (e.g., "Game Over" or "You Win").

## Technical Details

- **Rendering**: The game utilizes OpenGL 3D rendering to display the golf course and player interactions.
- **3D Models**: The game includes five 3D models — Tiger (player), pond, golf car, golf bag, and flag (goal).
- **Dynamic Walls**: The walls of the scene continuously change colors to create a more immersive experience.
- **Animation**: Player movements and the game environment are enhanced using animations triggered by the **P** key.
  
## Setup and Installation

1. **Clone or download the project** to your local machine.
2. **Install OpenGL** (OpenGL 3.x) and ensure you have the necessary libraries for C++.
3. **Compile the project** using your preferred C++ IDE or build system.
4. **Run the game** by executing the compiled file.

## Usage

- **Start Game**: Press **SPACE** to shoot a golf ball.
- **Move Player**: Use **+** to move the player forward and **-** to move the player backward.
- **Change Orientation**: Press **O** to adjust the player's orientation.
- **Activate Animations**: Press **P** to enable animations and effects.
- **Exit Game**: Press **ESC** to exit the game.

## Future Improvements

- Add more shooting positions and dynamic obstacles to increase difficulty.
- Implement multiplayer mode where players can compete against each other.
- Add a more complex scoring system with levels and bonuses.
- Include more diverse animations and visual effects to enhance the user experience.

## License

This project is open-source and is released under the MIT License.
