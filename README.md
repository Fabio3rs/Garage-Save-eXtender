# Garage Save eXtender (GSX)

**Garage Save eXtender** is a comprehensive framework for GTA San Andreas that extends the game's vehicle save system in garages, allowing other mods to save and load custom data associated with vehicles. This opens up unlimited possibilities for enhanced gameplay and mod development.

[![Mod demonstration](https://img.youtube.com/vi/TscmWxlQ3Sg/0.jpg)](https://www.youtube.com/watch?v=TscmWxlQ3Sg)

## 🚀 Features

### For Players
- **Extended Vehicle Persistence**: Vehicles saved in garages can now store additional custom data from other mods
- **Cross-Mod Compatibility**: Mods can share and preserve data through GSX's unified interface
- **Enhanced Gameplay**: Experience features like:
  - Fuel levels saved by Gas Station mods
  - Custom paint jobs and modifications persistence
  - License plates automatically saved
  - Vehicle tuning data preservation
  - And much more depending on installed mods

### For Modders
- **Unified Save/Load Interface**: Simple API for both CLEO scripts and ASI mods
- **Flexible Data Management**: Two types of data handling:
  - **Static Data**: Pre-stored data that persists automatically
  - **Dynamic Data**: Runtime data that's captured when vehicles are stored
- **Event Notifications**: Get notified when vehicles are saved or loaded from garages
- **Cross-Platform Support**: Works with CLEO (Sanny Builder/gta3script) and ASI mods

## 📋 Requirements

- GTA San Andreas (US v1.0 recommended)
- ASI Loader
- Microsoft Visual C++ Redistributable (for Windows)

## 🔧 Installation

1. Download the latest release from the [Releases](../../releases) page
2. Extract `gsx.asi` to your GTA San Andreas directory
3. Ensure you have an ASI loader installed
4. Launch the game - GSX will initialize automatically

## 📚 API Documentation

### C++ API

GSX provides a comprehensive C++ API for ASI mod development:

```cpp
#include "API/Cpp/GSXAPI.h"

// Save data for a vehicle
GSX::setDataToSaveLater(vehicle, "fuel_level", fuel_amount, false);

// Load data for a vehicle
float* saved_fuel = GSX::getSavedDataT<float>(vehicle, "fuel_level");

// Check if data exists
if (GSX::dataToLoadExists(vehicle, "fuel_level")) {
    // Handle existing data
}

// Register callback for save/load events
GSX::addNotifyCallback([](const GSX::externalCallbackStructure* data) {
    if (data->status == GSX::SAVE_CAR) {
        // Vehicle is being saved
    } else if (data->status == GSX::LOAD_CAR) {
        // Vehicle is being loaded
    }
});
```

### Key API Functions

#### Data Management
- `setDataToSaveLater()`: Schedule data to be saved when vehicle is stored
- `pushDirectlyToSavedData()`: Immediately save data to GSX memory
- `getSavedData()` / `getSavedDataT<T>()`: Retrieve saved data
- `dataToLoadExists()` / `dataToSaveLaterExists()`: Check data existence
- `removeSavedData()` / `removeToSaveLaterData()`: Remove data entries

#### Event Handling
- `addNotifyCallback()`: Register permanent callback for save/load events
- `addNotifyTempCallback()`: Register temporary callback (cleared on game reload)
- `getNewCarForeach()`: Manual iteration over vehicle events

#### Utility Functions
- `getVersionString()` / `getVersionNum()`: Get GSX version information
- `getAPIVersionString()`: Get API version

### Data Types

GSX works with two types of data:

1. **Static Data**: Data that's already in GSX memory and persists automatically. This data remains unchanged unless explicitly updated by scripts or replaced by dynamic data.

2. **Dynamic Data**: Data that GSX stores as memory addresses and sizes. When a vehicle is stored, GSX reads from these addresses and copies the specified amount of data to its own memory. If data with the same name already exists, it gets updated.

Data is identified in GSX memory by the name assigned by the modder, allowing for organized and conflict-free data management.

## 🛠️ Building from Source

### Prerequisites
- CMake 3.16+
- C++17 compatible compiler
- Wine (for cross-compilation on non-Windows systems)

### Build Steps
```bash
# Clone the repository
git clone https://github.com/Fabio3rs/Garage-Save-eXtender.git
cd Garage-Save-eXtender

# Initialize submodules
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)
```

### Windows Build
Use Visual Studio or MinGW-w64 for native Windows compilation.

## 📖 Usage Examples

### Example 1: Fuel System Mod
```cpp
// Save fuel level when vehicle is stored
float current_fuel = GetVehicleFuelLevel(vehicle);
GSX::setDataToSaveLater(vehicle, "fuel_level", current_fuel, false);

// Restore fuel level when vehicle is spawned
if (GSX::dataToLoadExists(vehicle, "fuel_level")) {
    float* saved_fuel = GSX::getSavedDataT<float>(vehicle, "fuel_level");
    if (saved_fuel) {
        SetVehicleFuelLevel(vehicle, *saved_fuel);
    }
}
```

### Example 2: Custom Paint Data
```cpp
struct PaintData {
    int color1, color2;
    bool metallic;
};

// Save paint configuration
PaintData paint = {255, 128, true};
GSX::setDataToSaveLater(vehicle, "custom_paint", paint, false);

// Load paint configuration
PaintData* saved_paint = GSX::getSavedDataT<PaintData>(vehicle, "custom_paint");
if (saved_paint) {
    ApplyCustomPaint(vehicle, *saved_paint);
}
```

## 🔌 Compatible Mods

GSX works with many popular GTA San Andreas mods:
- [VehFuncs](https://www.mixmods.com.br/2018/01/VehFuncs.html) - Enhanced vehicle functions
- [RGB Paint System](https://www.mixmods.com.br/2018/06/pintar-veiculos-usando-cores-rgb-com-gui.html) - Custom vehicle colors
- [Pulsing Neon](https://www.mixmods.com.br/2019/05/mod-neon-pulsante.html) - Animated vehicle lighting
- [Paintjobs Loader](https://www.mixmods.com.br/2019/07/paintjobs-loader.html) - Custom vehicle textures
- [License Plate Saver](https://www.mixmods.com.br/2018/01/save-license-plate-salvar-placa-carro.html) - Persistent license plates
- And many more!

## 📁 Project Structure

```
Garage-Save-eXtender/
├── API/                    # API headers and documentation
│   └── Cpp/               # C++ API implementation
├── gsx/                   # Core GSX source code
│   └── tests/            # Unit tests
├── plugins/              # Example plugins
│   └── SaveLicensePlate/ # License plate demo plugin
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork the repository** and create a feature branch
2. **Follow the existing code style** (see `.clang-format`)
3. **Add tests** for new functionality when possible
4. **Update documentation** for API changes
5. **Submit a pull request** with a clear description

### Code Style
- Use `.clang-format` for consistent formatting
- Follow C++17 standards
- Add meaningful comments for complex logic
- Use descriptive variable and function names

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) file for details.

## 🔗 Links

- **Forums**: 
  - [MixMods](https://forum.mixmods.com.br/f5-scripts-codigos/t214-gsx-garage-save-extender)
  - [GTAForums](https://gtaforums.com/topic/925563-garage-save-extender/)
- **Demonstration**: [YouTube Video](https://www.youtube.com/watch?v=TscmWxlQ3Sg)
- **Issues**: [GitHub Issues](../../issues)
- **Releases**: [GitHub Releases](../../releases)

## 🙏 Acknowledgments

- Plugin-SDK team for the GTA SA development framework
- Cereal library for serialization
- Google Test for testing framework
- All contributors and mod developers using GSX

---

*GSX - Extending GTA San Andreas vehicle persistence, one garage at a time.*

