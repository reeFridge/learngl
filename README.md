# LearnGL

repo for tracking history while I'm progressing over this awesome [articles](https://learnopengl.com) on `OpenGL` and `Computer Graphics` in general

## Dependencies

* [GLAD](https://github.com/Dav1dde/glad)
* [STB_IMAGE](https://github.com/nothings/stb/blob/master/stb_image.h)
* [GLM](https://github.com/g-truc/glm)
* [ImGUI](https://github.com/ocornut/imgui/)
* [GLFW](https://github.com/glfw/glfw) (as system package)
* [ASSIMP](https://github.com/assimp/assimp) (as system package)

## Build

get all git deps via `git clone` into `lib` directory

for `glm` for example run:

```sh
mkdir lib && cd lib
git clone https://github.com/g-truc/glm
```

get system deps via package manager

arch pacman:

```sh
sudo paman -S glfw-x11 assimp
```

debian apt:

```sh
sudo apt install libglfw3-dev libassimp-dev
```

and finally use `make` (from root dir) to build executable `build/main`

```sh
make
```
