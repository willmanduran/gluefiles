# GlueFiles

GlueFiles is a minimalist desktop utility designed to concatenate multiple files into a single output with precise order control.

## Features
* **Multi-format support:** Concatenate any text-based files.
* **Drag-and-Drop:** Intuitive interface to add files from your system.
* **Smart Headers:** Optionally include absolute file paths as headers.
* **Only Headers:** You can also choose to output only headers!.
* **JSON Import:** Paste a JSON array of paths for batch processing.
* **Bilingual:** Native support for English and Spanish.

## Build from Source

### Prerequisites
* **Qt 6.x** (Widgets & LinguistTools)
* **CMake ≥ 3.16**
* **C++17 Compiler** (I used GCC 11+ and MinGW 13+)

### Linux
```bash
sudo apt install qt6-base-dev qt6-l10n-tools qt6-tools-dev

git clone https://github.com/willmanstoolbox/gluefiles
cd gluefiles
mkdir build && cd build
cmake ..
make -j$(nproc)

cpack -G DEB # or TGZ
```

### Windows

```powershell
# 1. Setup Environment
$env:Path += ";C:\Qt\6.11.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin"

# 2. Configure & Build
mkdir build; cd build
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64" ..
cmake --build . --parallel 4

# 3. Deploy
windeployqt --qmldir .. gluefiles.exe
```

---

## Support & Contact
If you find any bugs, have ideas to improve this, or want to discuss low-level systems and C-style architecture, feel free to reach out.

**Email:** ticuette@gmail.com  
**Portfolio:** [willmanstoolbox.com](https://willmanstoolbox.com)

---