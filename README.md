# Standalone OSI Trace File Player

[![Build](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/build.yml/badge.svg)](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/build.yml)
[![Linter](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/linter.yml/badge.svg)](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/linter.yml)
[![SPDX](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/spdx.yml/badge.svg)](https://github.com/Persival-GmbH/standalone-osi-trace-file-player/actions/workflows/spdx.yml)

This mini application can read a binary ASAM OSI trace file (SensorData or SensorView) and send it step by step via TCP using ZeroMQ.

## Usage

```bash
./trace-file-player <path/to/trace/file.osi> <ip.address:port>
```

The player will read in the trace file step by step and send it via TCP to the given IP adress and port (e.g. 127.0.0.1:3456).

## Build Instructions

### Build Model in Ubuntu 20.04 / 22.04

1. Clone this repository **with submodules**:

    ```bash
    git clone https://github.com/Persival-GmbH/standalone-osi-trace-file-player.git --recurse-submodules
    ```

2. Install dependencies:

   ```bash
   sudo apt install libzmq3-dev
   ```

3. Build the model by executing in the extracted project root directory:

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```
