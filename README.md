## Bar Music Visualizer
Application for displaying frequency magnitudes as bars. Written in C++ with the use of QT Framework.

## Architecture
- `common/` contains common stuff that are used in many places across the project, such as RingBuffer, Settings Values ("gui/theme" and etc).
- `controllers/` contains core logic of the corresponding widgets.
- `qss/` contains basic themes.
- `svg/` contains several placeholder-kinda SVGs used as QIcon's.
- `widgets/` contains widgets for displaying control widgets (seekbar, pause and so on) and the visualizer itself.


## Dependencies
- spdlog
- fftw
- qt6

## Build
```bash
git clone https://github.com/commmrade/MusicVisualizer/
mkdir build && cd build
cmake ..
sudo cmake --build .
```
