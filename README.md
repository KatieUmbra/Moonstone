# Moonstone Game Engine

Repository continuing my old [OpenGL_Engine](https://github.com/KatieUmbra/OpenGL_Engine) project that turns it into a functioning game engine.
## Building

### Dependencies
To build this project you need to install the following dependencies:

- `vcpkg`
- `git`
- `ninja`
- `cmake`
  
if using linux:
- `unzip`
- `clang` or `gcc`
  
if using windows:
- `MinGW gcc or clang`

### CMake configuration
- follow the vcpkg instructions  for [CMakeUserPresets.json](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell#4---build-and-run-the-project)
- install vcpkg packages
```shell
$ vcpkg install
```
- run cmake:
```shell
$ cmake --preset=default
```

### Build the project
just run
```shell
$ cmake --build Build --config Debug
```
or
```shell
$ cmake --build Build --config Release
```
for release (move the build/game binary to the root of the project)
