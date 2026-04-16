# Meteor Engine

Meteor Engine is a modular C++ game engine designed for high-performance, in-house game development. It prioritizes low-level control, zero-standard-library overhead, and custom memory management.

## ⚠️ Build System Warning
The engine build pipeline has been **completely overhauled**. Legacy Visual Studio build commands (MSBuild) are no longer capable of linking the modular architecture and custom LLVM/Clang toolchain.
**Direct compilation via Visual Studio will successfully run, but not recommended.**

## 🛠️ Bootstrapping MeteorBuild
To bring the engine to life, you must first initialize the proprietary MeteorBuild system. This process handles module discovery, dependency resolution, and toolchain synchronization. First you need to install Clang/LLVM!
### Step 0: Installing Necessary Components
**Windows:** Download [here](https://releases.llvm.org).

> [!NOTE]
> Make sure option "Add to System PATH" is selected! 
> Otherwise you need to enter the full path to the compiler all time!


**Debian/Ubuntu (Linux), sudo required!**
```bash
sudo apt-get clang
```

### Step 1: Generate the Build Tool
Run the generation script from the root directory to bootstrap the environment:
**Windows:**
```bash
./scripts/generatemeteorbuild.bat
```

~~**Debian/Ubuntu:**~~
```bash
./scripts/generatemeteorbuild.sh
```
> [!WARNING]
> 2026-04-12: For now there is only batch file!

### Step 2: Running the Build
Once initialized, use the generated MeteorBuild binary to compile the engine modules:
```bash
# Example build command
./MeteorBuild -build -source ../engine -int ../intermediate -vs2026

# You can ask the build system, if you do not know what to do
./MeteorBuild -h/-help
```
## 💻 IDE Integration
While a .sln file is maintained for **Intellisense** and code navigation purposes, the build button within Visual Studio will be redirected to the MeteorBuild ecosystem through NMake commands.
## 📜 License
The source code is available under a custom license requiring written permission from the author for usage, modification, or distribution. See the LICENSE file for details.

> [!NOTE] 
> Build system migrated to standalone meta-compiler to bypass MSBuild limitations and enforce Clang/LLVM optimization passes.

## Contact

Developer: Levente Farkas
Email: farkaslevente200608@gmail.com
GitHub: https://github.com/HanssonSoftware/MeteorEngine
