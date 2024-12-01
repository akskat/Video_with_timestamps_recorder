# High FPS Video Capture with System Time Overlay

A C++ script for running a webcam and overlaying system time as timestamps on each video frame. The timestamp is displayed in the bottom-left corner in white text.

**Platform:** Linux (Ubuntu 22)

## Features
- High FPS video capture
- System time overlay on each frame
- Easy-to-quit functionality

---

## Setup

### Prerequisites
To run this script, you need to have OpenCV installed on your system. Install OpenCV with the following command:

```bash
sudo apt install libopencv-dev
```
### Compile
Compile the high_fps_video.cpp file with the following command:
```bash
g++ -o high_fps_video high_fps_video.cpp `pkg-config --cflags --libs opencv4`
```
This will create an executable file named high_fps_video.

### Usage
To start the program, run:

```bash
./high_fps_video
```

## Exiting the Program
To exit, press q at any time during the video playback.

## Example Output
The timestamp will look something like this:
[hh:mm:ss.milliseconds]
