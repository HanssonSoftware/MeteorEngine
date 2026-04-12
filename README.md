# Meteor Engine

Meteor Engine is a modular C++ game engine designed for high-performance, in-house game development. It prioritizes low-level control, zero-standard-library overhead, and custom memory management.

## ⚠️ Build System Warning
The engine build pipeline has been **completely overhauled**. Legacy Visual Studio build commands (MSBuild) are no longer capable of linking the modular architecture and custom LLVM/Clang toolchain.
**Direct compilation via Visual Studio will fail.**

## 🛠️ Bootstrapping MeteorBuild
To bring the engine to life, you must first initialize the proprietary MeteorBuild system. This process handles module discovery, dependency resolution, and toolchain synchronization. First you need to install Clang/LLVM!
### Step 0: Installing Necessary Components
**Windows, MAKE SURE TO ADD TO THE PATH!**
```bash
Dedicated link currently not available, llvm is available on github.
```
**Debian/Ubuntu (Linux), sudo required!**
```bash
sudo apt-get clang
```

### Step 1: Generate the Build Tool
Run the generation script from the root directory to bootstrap the environment:
**Windows:**
```bash
.\scripts\generatemeteorbuild.bat
```
**Unix/Linux: (2026-04-12: For now there is only batch file!)**
```bash
./scripts/generatemeteorbuild.sh
```
### Step 2: Running the Build
Once initialized, use the generated MeteorBuild binary to compile the engine modules:
```bash
# Example build command
./MeteorBuild Shipping --target All
# You can ask the build system, if you do not know what to do
./MeteorBuild -h/-help

```
## 💻 IDE Integration
While a .sln file is maintained for **Intellisense** and code navigation purposes, the build button within Visual Studio is deprecated. All binary assembly is handled externally to ensure strict adherence to the Hansson Software modular specification.
## 📜 License
The source code is available under a custom license requiring written permission from the author for usage, modification, or distribution. See the LICENSE file for details.
### Hansson Software Internal Log
> *Note: Build system migrated to standalone meta-compiler to bypass MSBuild limitations and enforce Clang/LLVM optimization passes.*
> 

## Contact

Developer: Levente Farkas
Email: farkaslevente200608@gmail.com
GitHub: https://github.com/HanssonSoftware/MeteorEngine
