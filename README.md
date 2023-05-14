# gesq
nesq's game of life

![gesq-example](https://user-images.githubusercontent.com/66211581/232922366-08d6457e-9174-4d87-bd87-82951ff856fa.gif)

## How to use
- `Click to Resume/Click to Pause`: you can use to pause and unpause the simulation.
- `Advance State`: when the simulation is paused it will advance to the next state of the board.
- `Clear`: it will fill the board with dead cells.
- Buttons under the `Patterns` tab: they work as changing the "brush", so you can add cool patterns to the simulation.
- When the game is paused you can click in a cell to toggle its state between alive and dead.

## Build and run
You will need [SDL2](https://www.libsdl.org/) and [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf) to build this code.

```console
$ ./build.sh run
```
