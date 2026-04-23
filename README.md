# GlueFiles

## What it is

GlueFiles is a small desktop utility that concatenates multiple files—in a specific order—into a single output file. Browse folders, drag-and-drop files, reorder them, and optionally include each file’s absolute path as a header before its contents. You can also paste a JSON list of paths.

## Build from source

### Linux / Windows

Requires **Qt 6 (Widgets)** and **CMake ≥ 3.16**.

```bash
# Clone
git clone https://github.com/willmanstoolbox/gluefiles
cd gluefiles

# Configure + build
cmake -S . -B build
cmake --build build -j

# Run
./build/gluefiles

```

## Support & Contact
If you find any bugs, have any ideas to improve this or just want to chat about C in general feel free to email me.

**Email:** ticuette@gmail.com

**More Tools:** You can support my work or check out other tools at [willmanstoolbox.com](https://willmanstoolbox.com)
