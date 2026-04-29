# gesq

nesq's game of life

> [!NOTE]  
> gesq development has been moved to [codeberg repo](https://codeberg.org/c_nicnic/gesq).

![gesq-example](https://user-images.githubusercontent.com/66211581/232922366-08d6457e-9174-4d87-bd87-82951ff856fa.gif)

## How to use

- `Click to Resume/Click to Pause`: you can use to pause and unpause the simulation.
- `Advance State`: when the simulation is paused it will advance to the next state of the board.
- `Clear`: it will fill the board with dead cells.
- Buttons under the `Patterns` tab: they work as changing the "brush", so you can add cool patterns to the simulation.
- When the game is paused you can click in a cell to toggle its state between alive and dead.

## TODO

- Add an option to save/load custom patterns.

## How to build

This project uses the [nob](https://github.com/tsoding/nob.h) build system.

To boostrap the build system:

```console
$ cc -o nob nob.c
```

After that any subsequent build may be done by running:

```consonle
$ ./nob
```

Nob is smart and will recompile itself when you modify its source code.

Any transitional files used during build will be placed in the `.build` directory.
The `gesq` executable will be placed in the root directory of the repository.
