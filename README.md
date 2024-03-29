## Quick start examples for integrating [Banuba SDK on Desktop](https://docs.banuba.com/face-ar-sdk/core/effect_player/) in C++ apps

## Getting Started

1. Get the latest Banuba SDK archive for macOS/Windows and the client token. Please fill out our form at [form at banuba.com](https://www.banuba.com/face-filters-sdk) website, or contact us via [info@banuba.com](mailto:info@banuba.com).
2. Extract Banuba SDK **C API** archive into the `OEP-desktop-c-api/bnb_sdk_c_api` dir:
    `bnb_sdk_c_api/` => `OEP-desktop-c-api/bnb_sdk_c_api`
3. Windows Only: copy `bnb_viewer_standalone/resources/` files into the `OEP-desktop-c-api/resources` dir:
    `bnb_viewer_standalone/resources/` => `OEP-desktop-c-api/resources`
4. Copy and Paste your client token into the appropriate section of `OEP-desktop-c-api/main.cpp`
5. Generate project files by executing the following commands:

    ##### Windows x86 build:

    ```bat
        cd %path_to_repository%
        git submodule update --init
        md build
        cd build
        cmake -A Win32 ..
    ```

    ##### Windows x64 build:

    ```bat
        cd %path_to_repository%
        git submodule update --init
        md build
        cd build
        cmake -A x64 ..
    ```

    ##### macOS build:

    ```sh
        cd $path_to_repository
        git submodule update --init
        mkdir build
        cd build
        cmake -G Xcode ..
    ```

7. The previous step will generate a Xcode project for macOS and a Visual Studio project for Windows. Open the viewer_standalone project in an appropriate IDE on your platform.
8. Select target `example`.
9. Run build.

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Sample structure

- **oep** - is a submodule of the offscreen effect player
- **libraries**
  - **renderer** - used only to demonstrate how to work with offscreen_effect_player. Draws received frames to the specified GLFW window
  - **utils**
    - **glfw_utils** - contains helper classes to work with GLFW
    - **ogl_utils** - contains helper classes to work with Open GL
    - **utils** - contains common helper classes such as thread_pool
- **main.cpp** - contains the main function implementation, demonstrating basic pipeline for frame processing to apply effect offscreen
- **effect_player.cpp, effect_player.hpp** - contains the custom implementation of the effect_player interface with using cpp api
- **render_context.cpp, render_context.hpp** - contains the custom implementation of the render_context interface with using GLFW

## How to change an effect

1. Open `OEP-desktop-c-api/main.cpp`
2. On line 87 find:

   ```c++
   try {
       play_effect("effects/Afro");
   }
   ```

3. Write the effect name that you want to run. For example: ("effects/your_effect_name")

*Note:* The effect must be in `OEP-desktop-c-api-master/resources/effect`.

## Integration note

For integration of the Offscreen Effect player to your application it is necessary to copy the oep folder and implement interfaces for effect_player and render_context, but if your application is based on the GLFW library and using bnb_effect_player cpp api, you can just reuse current implementation.
