# ScamPA - The Scam Preventing Agent
ScamPA is a simple C++ application designed with the purpose of being able to hinder the possibility of potential scams by utilizing ML bots to automate calls with malicious telemarketers.
This application is currently a work-in-progress and only supports Windows - with macOS and Linux support planned. Setup scripts support Visual Studio 2022 by default.

The application framework itself is essentially a heavily modified version of the [Walnut](https://github.com/StudioCherno/Walnut) framework.

![WalnutExample](https://hazelengine.com/images/ForestLauncherScreenshot.jpg)
_<center>Forest Launcher - an application made with Walnut</center>_

## Requirements
- [Visual Studio 2022](https://visualstudio.com) (not strictly required, however included setup scripts only support this)
    - Note: For Visual Studio 2026, you can simply retarget the project files after running initial build scripts.
    - To do this in Visual Studio 2026, simply go: Project > Retarget Solution, then hit "Retarget all" and "Apply"
    - We aim to update the premake5 binary included within this repo once it has been updated to support Visual Studio 2026.

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (version 1.3.283.0)
    - Will be installed for you upon running `Setup.bat`

## Getting Started
Once you've cloned, run `Scripts/Setup.bat` to generate Visual Studio 2022 solution/project files. Once you've opened the solution, you can run the ScamPA-App project to how it works (code in `ScamPAApp.cpp`). 

### 3rd party libaries
- [Dear ImGui](https://github.com/ocornut/imgui) - Bloat-free, immediate-mode graphical user interface library 
- [GLFW](https://github.com/glfw/glfw) - Cross-platform library for OpenGL, OpenGL ES & Vulkan application development
- [stb_image](https://github.com/nothings/stb) - Image-loading library
- [GLM](https://github.com/g-truc/glm) - Math library
- [spdlog](https://github.com/gabime/spdlog) - Logging library
(included for convenience)

#### Future libraries to be added
- [LlamaLib](https://github.com/undreamai/LlamaLib) - A high-level wrapper for [llama.cpp](https://github.com/ggml-org/llama.cpp), which is an inferencing engine for LLMs
- [whisper.cpp](https://github.com/ggml-org/whisper.cpp) - Audio transcription library

## Additional
- ScamPA uses the [Roboto](https://fonts.google.com/specimen/Roboto) font ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))