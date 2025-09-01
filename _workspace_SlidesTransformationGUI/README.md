# Slides Transformation Tool

## Overview
The Slides Transformation Tool is a specialized application for transforming musical notes with slide patterns. It allows you to apply various slide transformations to eligible notes in your music files, with control over transformation percentage and variant selection.

## Features
- Transform eligible notes with slide patterns
- Control transformation percentage (0-100%)
- Choose from multiple slide variants or use random selection
- Generate MIDI output from transformed files
- Cross-platform support (Windows and Linux)
- User-friendly GUI interface

## Eligible Note Labels
The tool transforms notes with the following labels:
- SAN, RLN, SMP, Mmd7, I8, U2R, HT, MmAug6
- RDN, RN, MmAug4, Mmm3, LAD, DNW, LNSN
- DBC, DDN, LNR, LNSAS, LNSAL, DI, SPCM
- SPDM, SSN, SVN, ANS, ANL, FTB, CDB

## Slide Variants
The tool includes numerous slide variants, including:
- Basic STT variants (2-note patterns)
- Dotted DSTT variants (2-note patterns)
- Inverted ISTT variants (2-note patterns)
- Dotted Inverted DISTT variants (2-note patterns)
- Three-tone TTS variants (3-note patterns)
- First position dotted TTSd1 variants (3-note patterns)
- Second position dotted TTSd2 variants (3-note patterns)
- Third position dotted TTSd3 variants (3-note patterns)
- Inverted TTIT variants (3-note patterns)
- Inverted above ITTIT variants (3-note patterns)
- Inverted three-tone above ITTS variants (3-note patterns)

## Building from Source
1. Ensure you have CMake (3.10+) and a C++17 compatible compiler installed
2. Clone or download this repository
3. Create a build directory: `mkdir build && cd build`
4. Run CMake: `cmake ..`
5. Build the project: `cmake --build .`

## Usage
### GUI Mode
1. Launch the application
2. Select input and output files
3. Choose transformation percentage using the slider
4. Select a slide variant or use random selection
5. Click "Process File" to transform the notes
6. Optionally, select a MIDI output file and click "Generate MIDI"

### Command Line Mode
```
SlidesTransformation <input_file> <output_file> [midi_output_file] [transformation_percentage] [variant]
```

Example:
```
SlidesTransformation input.txt output.txt output.mid 50 RANDOM
```

## Input File Format
The input file should be a text file with the following format:
```
Track Note Duration Label
1 C4 480 SAN
1 E4 240 RLN
...
```

## Output
The tool generates a text file with transformed notes and optionally a MIDI file.

## Dependencies
- Windows: comctl32 library
- Linux: X11 libraries

## License
Copyright (C) 2025