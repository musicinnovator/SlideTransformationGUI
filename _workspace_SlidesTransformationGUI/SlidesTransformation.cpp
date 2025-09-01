// Slides Transformation (C) 2025
// Version with percentage control, multiple choice variant selection, and GUI without dependencies
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <memory>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #include <commdlg.h>
    #include <commctrl.h>
    #pragma comment(lib, "comctl32.lib")
#elif defined(__linux__) || defined(__unix__)
    #define PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysym.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#else
    #error "Unsupported platform"
#endif

// Helper to get note name (from MIDI number)
std::string getNoteName(int noteNumber) {
    static const std::string noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    int octave = noteNumber / 12 - 1;
    int noteIndex = noteNumber % 12;
    return noteNames[noteIndex] + std::to_string(octave);
}

// Helper to get MIDI number from note name
int getNoteNumber(const std::string& noteName) {
    static const std::string noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    // Extract note and octave
    std::string baseNote = noteName.substr(0, noteName.size() - 1);
    int octave = std::stoi(noteName.substr(noteName.size() - 1));

    // Find base note index
    auto it = std::find(std::begin(noteNames), std::end(noteNames), baseNote);
    if (it == std::end(noteNames)) {
        throw std::invalid_argument("Invalid note name: " + noteName);
    }
    int noteIndex = std::distance(std::begin(noteNames), it);

    return (octave + 1) * 12 + noteIndex;
}

// Enum for time meter
enum TimeMeter {
    DUPLE,
    TRIPLE
};

// Helper functions for slide variants - ALL ORIGINAL FUNCTIONS PRESERVED
void handleMeter(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 4;
        EmbRet.push_back({pre1, segment});
        EmbRet.push_back({pre2, segment});
        EmbRet.push_back({pi, segment * 2});
    } else if (meter == TRIPLE) {
        int segment = durPi / 3;
        EmbRet.push_back({pre1, segment});
        EmbRet.push_back({pre2, segment});
        EmbRet.push_back({pi, segment});
    }
}

void handleMeter4(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = (durPi * 3) / 8; // 3/8 of durPi
        int segment2 = durPi / 8;      // 1/8 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pi, durPi / 2});
    } else if (meter == TRIPLE) {
        int segment1 = durPi / 3; // 1/3 of durPi
        int segment2 = durPi / 6; // 1/6 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pi, durPi / 2});
    }
}

void handleMeter5(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = durPi / 4; // 1/4 of durPi
        int segment2 = (durPi * 3) / 8; // 3/8 of durPi
        EmbRet.push_back({pre2, segment1});
        EmbRet.push_back({pre1, segment2});
        EmbRet.push_back({pi, durPi / 2});
    } else if (meter == TRIPLE) {
        int segment1 = durPi / 6; // 1/6 of durPi
        int segment2 = durPi / 3; // 1/3 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pi, durPi / 2});
    }
}

void handleMeter3(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pre3, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment = durPi / 6; // 1/6 of durPi
        EmbRet.push_back({pre1, segment});
        EmbRet.push_back({pre2, segment});
        EmbRet.push_back({pre3, segment});
        EmbRet.push_back({pi, durPi - 3 * segment});  // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 8; // 1/8 of durPi
        EmbRet.push_back({pre1, segment});
        EmbRet.push_back({pre2, segment});
        EmbRet.push_back({pre3, segment});
        EmbRet.push_back({pi, durPi - 3 * segment});  // Remaining duration
    }
}

void handleMeter6(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pre3, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = durPi / 4; // 1/4 of durPi
        int segment2 = durPi / 8; // 1/8 of durPi
        int segment3 = durPi / 8; // 1/8 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pre3, segment3});
        EmbRet.push_back({pi, durPi - (segment1 + segment2 + segment3)}); // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 8; // 1/8 of durPi
        EmbRet.push_back({pre1, segment * 2});
        EmbRet.push_back({pre2, segment});
        EmbRet.push_back({pre3, segment});
        EmbRet.push_back({pi, durPi - 4 * segment});  // Remaining duration
    }
}

void handleMeter7(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pre3, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = durPi / 8; // 1/8 of durPi
        int segment2 = durPi / 4; // 1/4 of durPi
        int segment3 = durPi / 8; // 1/8 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pre3, segment3});
        EmbRet.push_back({pi, durPi - (segment1 + segment2 + segment3)});  // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 8; // 1/8 of durPi
        EmbRet.push_back({pre1, segment});
        EmbRet.push_back({pre2, segment * 2}); // 2/8
        EmbRet.push_back({pre3, segment});
        EmbRet.push_back({pi, durPi - (segment * 4)});  // Remaining duration
    }
}

void handleMeter8(std::vector<std::pair<int, int>>& EmbRet, int pre1, int pre2, int pre3, int pi, int durPi, TimeMeter meter) {
    if (meter == DUPLE) {
        int segment1 = durPi / 8; // 1/8 of durPi
        int segment2 = durPi / 8; // 1/8 of durPi
        int segment3 = durPi / 4; // 1/4 of durPi
        EmbRet.push_back({pre1, segment1});
        EmbRet.push_back({pre2, segment2});
        EmbRet.push_back({pre3, segment3});
        EmbRet.push_back({pi, durPi - (segment1 + segment2 + segment3)});  // Remaining duration
    } else if (meter == TRIPLE) {
        int segment = durPi / 12; // 1/12 of durPi
        EmbRet.push_back({pre1, segment * 2});
        EmbRet.push_back({pre2, segment * 2});
        EmbRet.push_back({pre3, segment * 4});
        EmbRet.push_back({pi, durPi - (segment * 8)});  // Remaining duration
    }
}

// Main function to apply slide variants - ORIGINAL FUNCTION NAME AND ALL VARIANTS PRESERVED
std::vector<std::pair<int, int>> applySlideVariants(int pi, int durPi, TimeMeter meter, const std::string& variant) {
    if (durPi <= 0) {
        throw std::invalid_argument("Duration (durPi) must be greater than 0");
    }
    if (meter != DUPLE && meter != TRIPLE) {
        throw std::invalid_argument("Invalid TimeMeter");
    }

    std::vector<std::pair<int, int>> EmbRet;

    // Basic STT slide variants (2-note patterns)
    if (variant == "STTM2m") {  // Slide note start m3 below principal note, then up a M2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "STTm2M") {  // Slide note start m3 below principal note, then up a m2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "STTm3m") {  // Slide note start M3 below principal note, then up a m3, and then resolve
        int pre1 = pi - 4;  // M3 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "STTM2M") {  // Slide note start M3 below principal note, then up a M2, and then resolve
        int pre1 = pi - 4;  // M3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);

    // Dotted DSTT slide variants (2-note patterns)
    } else if (variant == "DSTTM2m") {  // Slide note start dotted m3 below principal note, then up a M2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DSTTm2M") {  // Slide note start dotted m3 below principal note, then up a m2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DSTTm3m") {  // Slide note start dotted M3 below principal note, then up a m3, and then resolve
        int pre1 = pi - 4;  // M3 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DSTTM2M") {  // Slide note start dotted M3 below principal note, then up a M2, and then resolve
        int pre1 = pi - 4;  // M3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);

    // Inverted ISTT slide variants (2-note patterns)
    } else if (variant == "ISTTM2m") {  // Slide note starts M3 above principal note, then down a M2, and then resolve
        int pre1 = pi + 4;  // M3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "ISTTm2M") {  // Slide note starts m3 above principal note, then down a m2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "ISTTM3m") {  // Slide note starts M3 above principal notes, then down a m3, and then resolve
        int pre1 = pi + 4;  // M3 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "ISTTM2M") {  // Slide note starts m3 above principal note, then down a M2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        handleMeter(EmbRet, pre1, pre2, pi, durPi, meter);

    // Dotted Inverted DISTT slide variants (2-note patterns)
    } else if (variant == "DISTTM2m") {  // Slide note starts dotted M3 above principal note, then down a M2, and then resolve
        int pre1 = pi + 4;  // M3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DISTTm2M") {  // Slide note starts dotted m3 above principal note, then down a m2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DISTTm3m") {  // Slide note starts dotted M3 above principal notes, then down a m3, and then resolve
        int pre1 = pi + 4;  // M3 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);
    } else if (variant == "DISTTM2M") {  // Slide note starts dotted m3 above principal note, then down a M2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        handleMeter4(EmbRet, pre1, pre2, pi, durPi, meter);

    // Three-tone TTS slide variants (3-note patterns)
    } else if (variant == "TTSM2m2M") {  // Three tone Slide note starts P4 below principal note, then up a M2, then up a m2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSm3M2M") {  // Three tone Slide note starts A4 below principal note, then up a m3, then up a m2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSm2M2M") {  // Three tone Slide note starts P4 below principal note, then up a m2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSM2M2m") {  // Three tone Slide note starts P4 below principal note, then up M2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSM2M2M") {  // Three tone Slide note starts A4 below principal note, then up M2, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSm2m3m") {  // Three tone Slide note starts P4 below principal note, then up a m2, then up a m3, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSm3M2m") {  // Three tone Slide note starts A4 below principal note, then up a m3, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSM2m3m") {  // Three tone Slide note starts A4 below principal note, then up M2, then up a m3, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // First position dotted TTSd1 variants (3-note patterns)
    } else if (variant == "TTSd1M2m2M") {  // Three tone Slide note starts dotted P4 below principal note, then up a M2, then up a m2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1m3M2M") {  // Three tone Slide note starts dotted A4 below principal note, then up a m3, then up a m2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1m2M2M") {  // Three tone Slide note starts dotted P4 below principal note, then up a m2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1M2M2m") {  // Three tone Slide note starts dotted P4 below principal note, then up M2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1M2M2M") {  // Three tone Slide note starts dotted A4 below principal note, then up M2, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1m2m3m") {  // Three tone Slide note starts dotted P4 below principal note, then up a m2, then up a m3, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1m3M2m") {  // Three tone Slide note starts dotted A4 below principal note, then up a m3, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd1M2m3m") {  // Three tone Slide note starts dotted A4 below principal note, then up M2, then up a m3, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter6(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // Second position dotted TTSd2 variants (3-note patterns)
    } else if (variant == "TTSd2M2m2M") {  // Three tone Slide note starts P4 below principal note, then up a dotted M2, then up a m2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2m3M2M") {  // Three tone Slide note starts A4 below principal note, then up a dotted m3, then up a m2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2m2M2M") {  // Three tone Slide note starts P4 below principal note, then up a dotted m2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2M2M2m") {  // Three tone Slide note starts P4 below principal note, then up a dotted M2, then up a M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2M2M2M") {  // Three tone Slide note starts A4 below principal note, then up a dotted M2, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2m2m3m") {  // Three tone Slide note starts P4 below principal note, then up a dotted m2, then up a m3, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2m3M2m") {  // Three tone Slide note starts A4 below principal note, then up a dotted m3, then up a M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd2M2m3m") {  // Three tone Slide note starts A4 below principal note, then up a dotted M2, then up a m3, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter7(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // Third position dotted TTSd3 variants (3-note patterns)
    } else if (variant == "TTSd3M2m2M") {  // Three tone Slide note starts P4 below principal note, then up a M2, then up a dotted m2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3m3M2M") {  // Three tone Slide note starts A4 below principal note, then up a m3, then up a dotted m2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3m2M2M") {  // Three tone Slide note starts P4 below principal note, then up a m2, then up a dotted M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3M2M2m") {  // Three tone Slide note starts P4 below principal note, then up a M2, then up a dotted M2, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3M2M2M") {  // Three tone Slide note starts A4 below principal note, then up a M2, then up a dotted M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3m2m3m") {  // Three tone Slide note starts P4 below principal note, then up a m2, then up a dotted m3, and then resolve
        int pre1 = pi - 5;  // P4 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3m3M2m") {  // Three tone Slide note starts A4 below principal note, then up a m3, then up a dotted M2, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTSd3M2m3m") {  // Three tone Slide note starts A4 below principal note, then up a M2, then up a dotted m3, and then resolve
        int pre1 = pi - 6;  // A4 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter8(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // Inverted TTIT variants (3-note patterns)
    } else if (variant == "TTITM2M2M") {  // Three tone Slide Inverted note starts M2 below principal note, then up a M2, then up a M2, and then resolve
        int pre1 = pi - 2;  // M2 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITM2M2m") {  // Three tone Slide Inverted note starts m3 below principal note, then up a M2, then up a M2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITM2m3M") {  // Three tone Slide Inverted note start m3 below principal note, then up a M2, then up a m3, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 3;  // m3 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITm2M2m") {  // Three tone Slide Inverted note starts M2 below principal note, then up a m2, then up a M2, and then resolve
        int pre1 = pi - 2;  // M2 below principal
        int pre2 = pre1 + 1;  // m2 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITm3M2M") {  // Three tone Slide Inverted note starts m3 below principal note, then up a m3, then up a M2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 2;  // M2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITm3m2m") {  // Three tone Slide Inverted note starts m3 below principal note, then up a m3, then up a m2, and then resolve
        int pre1 = pi - 3;  // m3 below principal
        int pre2 = pre1 + 3;  // m3 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "TTITM2m2m") {  // Three tone Slide Inverted note starts M2 below principal note, then up a M2, then up a m2, and then resolve
        int pre1 = pi - 2;  // M2 below principal
        int pre2 = pre1 + 2;  // M2 up from pre1
        int pre3 = pre2 + 1;  // m2 up from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // Inverted above ITTIT variants (3-note patterns)
    } else if (variant == "ITTITM2M2M") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a M2, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm2M3m") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m2, then down a M3, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        int pre3 = pre2 - 4;  // M3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm3m2M") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m3, then down a m2, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm3m2m") {  // Inverted Three tone Slide Inverted note starts m3 above principal note, then down a m3, then down a m2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITM2M3m") {  // Inverted Three tone Slide Inverted note starts m3 above principal note, then down a M2, then down a M3, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 4;  // M3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITM2m2M") {  // Inverted Three tone Slide Inverted note starts m2 above principal note, then down a M2, then down a m2, and then resolve
        int pre1 = pi + 1;  // m2 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITM2m2m") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a m2, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITM2m3m") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a m3, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 3;  // m3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm2M2M") {  // Inverted Three tone Slide Inverted note starts m2 above principal note, then down a m2, then down a M2, and then resolve
        int pre1 = pi + 1;  // m2 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm2m3M") {  // Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m2, then down a m3, and then resolve
        int pre1 = pi + 2;  // M2 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        int pre3 = pre2 - 3;  // m3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTITm3M2M") {  // Inverted Three tone Slide Inverted note starts m3 above principal note, then down a m3, then down a M2, and then resolve
        int pre1 = pi + 3;  // m3 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);

    // Inverted three-tone above ITTS variants (3-note patterns)
    } else if (variant == "ITTSM2M2m") {  // Inverted Three tone Above Slide note starts P4 above principal note, then down a M2, then down a M2, and then resolve
        int pre1 = pi + 5;  // P4 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSm2M2M") {  // Inverted Three tone Above Slide note starts P4 above principal note, then down a m2, then down a M2, and then resolve
        int pre1 = pi + 5;  // P4 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSm2m3m") {  // Inverted Three tone Above Slide note starts P4 above principal note, then down a m2, then down a m3, and then resolve
        int pre1 = pi + 5;  // P4 above principal
        int pre2 = pre1 - 1;  // m2 down from pre1
        int pre3 = pre2 - 3;  // m3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSm3M2m") {  // Inverted Three tone Above Slide note starts A4 above principal note, then down a m3, then down a M2, and then resolve
        int pre1 = pi + 6;  // A4 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSm3m2M") {  // Inverted Three tone Above Slide note starts A4 above principal note, then down a m3, then down a m2, and then resolve
        int pre1 = pi + 6;  // A4 above principal
        int pre2 = pre1 - 3;  // m3 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSM2M2M") {  // Inverted Three tone Above Slide note starts A4 above principal note, then down a M2, then down a M2, and then resolve
        int pre1 = pi + 6;  // A4 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 2;  // M2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSM2m2m") {  // Inverted Three tone Above Slide note starts M3 above principal note, then down a M2, then down a m2, and then resolve
        int pre1 = pi + 4;  // M3 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 1;  // m2 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else if (variant == "ITTSM2m3m") {  // Inverted Three tone Above Slide note starts A4 above principal note, then down a M2, then down a m3, and then resolve
        int pre1 = pi + 6;  // A4 above principal
        int pre2 = pre1 - 2;  // M2 down from pre1
        int pre3 = pre2 - 3;  // m3 down from pre2
        handleMeter3(EmbRet, pre1, pre2, pre3, pi, durPi, meter);
    } else {
        // Handle unknown variant
        throw std::invalid_argument("Unknown slide variant: " + variant);
    }

    return EmbRet;
}

// Structure to represent a slide variant
struct SlideVariant {
    std::string name;
    std::string description;
};

// NEW FUNCTION: Generate a random pool of slide variants for user selection
std::vector<SlideVariant> generateRandomSlideVariantPool(int poolSize = 10) {
    // Define the complete pool of slide variants
    const std::vector<SlideVariant> allVariants = {
        // Basic STT variants (2-note patterns)
        {"STTM2m", "Slide note start m3 below principal note, then up a M2, and then resolve"},
        {"STTm2M", "Slide note start m3 below principal note, then up a m2, and then resolve"},
        {"STTm3m", "Slide note start M3 below principal note, then up a m3, and then resolve"},
        {"STTM2M", "Slide note start M3 below principal note, then up a M2, and then resolve"},

        // Dotted DSTT variants (2-note patterns)
        {"DSTTM2m", "Slide note start dotted m3 below principal note, then up a M2, and then resolve"},
        {"DSTTm2M", "Slide note start dotted m3 below principal note, then up a m2, and then resolve"},
        {"DSTTm3m", "Slide note start dotted M3 below principal note, then up a m3, and then resolve"},
        {"DSTTM2M", "Slide note start dotted M3 below principal note, then up a M2, and then resolve"},

        // Inverted ISTT variants (2-note patterns)
        {"ISTTM2m", "Slide note starts M3 above principal note, then down a M2, and then resolve"},
        {"ISTTm2M", "Slide note starts m3 above principal note, then down a m2, and then resolve"},
        {"ISTTM3m", "Slide note starts M3 above principal notes, then down a m3, and then resolve"},
        {"ISTTM2M", "Slide note starts m3 above principal note, then down a M2, and then resolve"},

        // Dotted Inverted DISTT variants (2-note patterns)
        {"DISTTM2m", "Slide note starts dotted M3 above principal note, then down a M2, and then resolve"},
        {"DISTTm2M", "Slide note starts dotted m3 above principal note, then down a m2, and then resolve"},
        {"DISTTm3m", "Slide note starts dotted M3 above principal notes, then down a m3, and then resolve"},
        {"DISTTM2M", "Slide note starts dotted m3 above principal note, then down a M2, and then resolve"},

        // Three-tone TTS variants (3-note patterns)
        {"TTSM2m2M", "Three tone Slide note starts P4 below principal note, then up a M2, then up a m2, and then resolve"},
        {"TTSm3M2M", "Three tone Slide note starts A4 below principal note, then up a m3, then up a m2, and then resolve"},
        {"TTSm2M2M", "Three tone Slide note starts P4 below principal note, then up a m2, then up a M2, and then resolve"},
        {"TTSM2M2m", "Three tone Slide note starts P4 below principal note, then up M2, then up a M2, and then resolve"},
        {"TTSM2M2M", "Three tone Slide note starts A4 below principal note, then up M2, then up a M2, and then resolve"},
        {"TTSm2m3m", "Three tone Slide note starts P4 below principal note, then up a m2, then up a m3, and then resolve"},
        {"TTSm3M2m", "Three tone Slide note starts A4 below principal note, then up a m3, then up a M2, and then resolve"},
        {"TTSM2m3m", "Three tone Slide note starts A4 below principal note, then up M2, then up a m3, and then resolve"},

        // First position dotted TTSd1 variants (3-note patterns)
        {"TTSd1M2m2M", "Three tone Slide note starts dotted P4 below principal note, then up a M2, then up a m2, and then resolve"},
        {"TTSd1m3M2M", "Three tone Slide note starts dotted A4 below principal note, then up a m3, then up a m2, and then resolve"},
        {"TTSd1m2M2M", "Three tone Slide note starts dotted P4 below principal note, then up a m2, then up a M2, and then resolve"},
        {"TTSd1M2M2m", "Three tone Slide note starts dotted P4 below principal note, then up M2, then up a M2, and then resolve"},
        {"TTSd1M2M2M", "Three tone Slide note starts dotted A4 below principal note, then up M2, then up a M2, and then resolve"},
        {"TTSd1m2m3m", "Three tone Slide note starts dotted P4 below principal note, then up a m2, then up a m3, and then resolve"},
        {"TTSd1m3M2m", "Three tone Slide note starts dotted A4 below principal note, then up a m3, then up a M2, and then resolve"},
        {"TTSd1M2m3m", "Three tone Slide note starts dotted A4 below principal note, then up M2, then up a m3, and then resolve"},

        // Second position dotted TTSd2 variants (3-note patterns)
        {"TTSd2M2m2M", "Three tone Slide note starts P4 below principal note, then up a dotted M2, then up a m2, and then resolve"},
        {"TTSd2m3M2M", "Three tone Slide note starts A4 below principal note, then up a dotted m3, then up a m2, and then resolve"},
        {"TTSd2m2M2M", "Three tone Slide note starts P4 below principal note, then up a dotted m2, then up a M2, and then resolve"},
        {"TTSd2M2M2m", "Three tone Slide note starts P4 below principal note, then up a dotted M2, then up a M2, and then resolve"},
        {"TTSd2M2M2M", "Three tone Slide note starts A4 below principal note, then up a dotted M2, then up a M2, and then resolve"},
        {"TTSd2m2m3m", "Three tone Slide note starts P4 below principal note, then up a dotted m2, then up a m3, and then resolve"},
        {"TTSd2m3M2m", "Three tone Slide note starts A4 below principal note, then up a dotted m3, then up a M2, and then resolve"},
        {"TTSd2M2m3m", "Three tone Slide note starts A4 below principal note, then up a dotted M2, then up a m3, and then resolve"},

        // Third position dotted TTSd3 variants (3-note patterns)
        {"TTSd3M2m2M", "Three tone Slide note starts P4 below principal note, then up a M2, then up a dotted m2, and then resolve"},
        {"TTSd3m3M2M", "Three tone Slide note starts A4 below principal note, then up a m3, then up a dotted m2, and then resolve"},
        {"TTSd3m2M2M", "Three tone Slide note starts P4 below principal note, then up a m2, then up a dotted M2, and then resolve"},
        {"TTSd3M2M2m", "Three tone Slide note starts P4 below principal note, then up a M2, then up a dotted M2, and then resolve"},
        {"TTSd3M2M2M", "Three tone Slide note starts A4 below principal note, then up a M2, then up a dotted M2, and then resolve"},
        {"TTSd3m2m3m", "Three tone Slide note starts P4 below principal note, then up a m2, then up a dotted m3, and then resolve"},
        {"TTSd3m3M2m", "Three tone Slide note starts A4 below principal note, then up a m3, then up a dotted M2, and then resolve"},
        {"TTSd3M2m3m", "Three tone Slide note starts A4 below principal note, then up a M2, then up a dotted m3, and then resolve"},

        // Inverted TTIT variants (3-note patterns)
        {"TTITM2M2M", "Three tone Slide Inverted note starts M2 below principal note, then up a M2, then up a M2, and then resolve"},
        {"TTITM2M2m", "Three tone Slide Inverted note starts m3 below principal note, then up a M2, then up a M2, and then resolve"},
        {"TTITM2m3M", "Three tone Slide Inverted note start m3 below principal note, then up a M2, then up a m3, and then resolve"},
        {"TTITm2M2m", "Three tone Slide Inverted note starts M2 below principal note, then up a m2, then up a M2, and then resolve"},
        {"TTITm3M2M", "Three tone Slide Inverted note starts m3 below principal note, then up a m3, then up a M2, and then resolve"},
        {"TTITm3m2m", "Three tone Slide Inverted note starts m3 below principal note, then up a m3, then up a m2, and then resolve"},
        {"TTITM2m2m", "Three tone Slide Inverted note starts M2 below principal note, then up a M2, then up a m2, and then resolve"},

        // Inverted above ITTIT variants (3-note patterns)
        {"ITTITM2M2M", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a M2, and then resolve"},
        {"ITTITm2M3m", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m2, then down a M3, and then resolve"},
        {"ITTITm3m2M", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m3, then down a m2, and then resolve"},
        {"ITTITm3m2m", "Inverted Three tone Slide Inverted note starts m3 above principal note, then down a m3, then down a m2, and then resolve"},
        {"ITTITM2M3m", "Inverted Three tone Slide Inverted note starts m3 above principal note, then down a M2, then down a M3, and then resolve"},
        {"ITTITM2m2M", "Inverted Three tone Slide Inverted note starts m2 above principal note, then down a M2, then down a m2, and then resolve"},
        {"ITTITM2m2m", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a m2, and then resolve"},
        {"ITTITM2m3m", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a M2, then down a m3, and then resolve"},
        {"ITTITm2M2M", "Inverted Three tone Slide Inverted note starts m2 above principal note, then down a m2, then down a M2, and then resolve"},
        {"ITTITm2m3M", "Inverted Three tone Slide Inverted note starts M2 above principal note, then down a m2, then down a m3, and then resolve"},
        {"ITTITm3M2M", "Inverted Three tone Slide Inverted note starts m3 above principal note, then down a m3, then down a M2, and then resolve"},

        // Inverted three-tone above ITTS variants (3-note patterns)
        {"ITTSM2M2m", "Inverted Three tone Above Slide note starts P4 above principal note, then down a M2, then down a M2, and then resolve"},
        {"ITTSm2M2M", "Inverted Three tone Above Slide note starts P4 above principal note, then down a m2, then down a M2, and then resolve"},
        {"ITTSm2m3m", "Inverted Three tone Above Slide note starts P4 above principal note, then down a m2, then down a m3, and then resolve"},
        {"ITTSm3M2m", "Inverted Three tone Above Slide note starts A4 above principal note, then down a m3, then down a M2, and then resolve"},
        {"ITTSm3m2M", "Inverted Three tone Above Slide note starts A4 above principal note, then down a m3, then down a m2, and then resolve"},
        {"ITTSM2M2M", "Inverted Three tone Above Slide note starts A4 above principal note, then down a M2, then down a M2, and then resolve"},
        {"ITTSM2m2m", "Inverted Three tone Above Slide note starts M3 above principal note, then down a M2, then down a m2, and then resolve"},
        {"ITTSM2m3m", "Inverted Three tone Above Slide note starts A4 above principal note, then down a M2, then down a m3, and then resolve"}
    };

    // Create a copy of all variants and shuffle it
    std::vector<SlideVariant> shuffledVariants = allVariants;

    // Use modern random number generation
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledVariants.begin(), shuffledVariants.end(), g);

    // Return the first poolSize variants
    std::vector<SlideVariant> pool;
    for (int i = 0; i < std::min(poolSize, static_cast<int>(shuffledVariants.size())); ++i) {
        pool.push_back(shuffledVariants[i]);
    }

    return pool;
}

// NEW FUNCTION: Parse user input for multiple choice selection
std::vector<int> parseUserChoices(const std::string& input, int maxChoice) {
    std::vector<int> choices;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        try {
            int choice = std::stoi(token);
            if (choice >= 1 && choice <= maxChoice) {
                // Avoid duplicates
                if (std::find(choices.begin(), choices.end(), choice) == choices.end()) {
                    choices.push_back(choice);
                }
            }
        } catch (const std::exception&) {
            // Ignore invalid tokens
        }
    }

    return choices;
}

// NEW FUNCTION: Check if a label should be transformed based on percentage
bool shouldTransformLabel(double transformationPercentage) {
    // Generate random number between 0 and 100
    double randomValue = (static_cast<double>(rand()) / RAND_MAX) * 100.0;
    return randomValue < transformationPercentage;
}

// Structure to represent a MIDI note event
struct MidiEvent {
    int track;
    int noteNumber;
    int startTime;
    int duration;
    bool isNoteOn;
};

// Application state
struct AppState {
    std::string inputFile;
    std::string outputFile;
    std::string midiOutputFile;
    double transformationPercentage = 50.0;
    std::vector<std::string> selectedVariants;
    bool processingComplete = false;
    std::string statusMessage;
    std::string resultSummary;
    int totalEligibleNotes = 0;
    int transformedNotes = 0;
    std::map<std::string, int> variantUsageCount;
};

// Function to process file with GUI integration
void processFile(const std::string& inputFile, const std::string& outputFile, AppState& state) {
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);

    if (!input.is_open() || !output.is_open()) {
        state.statusMessage = "Error opening files.";
        return;
    }

    // Write header to the output file
    output << std::left << std::setw(11) << "Track"
           << std::setw(11) << "Note"
           << std::setw(20) << "Duration"
           << std::setw(20) << "Label"
           << std::setw(25) << "Slide_Variant"
           << "\n";
    output << "---------------------------------------------------------------------------------\n";

    // Reset statistics
    state.totalEligibleNotes = 0;
    state.transformedNotes = 0;
    state.variantUsageCount.clear();

    std::string line;
    while (std::getline(input, line)) {
        std::istringstream ss(line);

        int track, duration;
        std::string noteName, label;

        // Parse line with Note in string format (e.g., "C4")
        if (!(ss >> track >> noteName >> duration)) {
            output << line << "\n";  // Handle malformed lines
            continue;
        }

        std::getline(ss, label);
        label.erase(0, label.find_first_not_of(" \t"));  // Trim leading whitespace
        // Remove trailing carriage return and whitespace (Windows line endings)
        label.erase(label.find_last_not_of(" \t\r\n") + 1);

        // Check if this label is eligible for transformation
        if (label == "SAN" || label == "RLN" || label == "SMP" || label == "Mmd7" ||
            label == "I8" || label == "U2R" || label == "HT" || label == "MmAug6" ||
            label == "RDN" || label == "RN" || label == "MmAug4" || label == "Mmm3" || 
            label == "LAD" || label == "DNW" || label == "LNSN" || label == "DBC" || 
            label == "DDN" || label == "LNR" || label == "LNSAS" || label == "LNSAL" || 
            label == "DI" || label == "SPCM" || label == "SPDM" || label == "SSN" || 
            label == "SVN" || label == "ANS" || label == "ANL" || label == "FTB" || 
            label == "CDB") {

            state.totalEligibleNotes++;

            // Check if this note should be transformed based on percentage
            if (shouldTransformLabel(state.transformationPercentage)) {
                state.transformedNotes++;

                try {
                    // Convert note name to MIDI number
                    int noteIndex = getNoteNumber(noteName);

                    // Randomly select a variant from the user's choices
                    std::string selectedVariant;
                    if (state.selectedVariants.empty() || (state.selectedVariants.size() == 1 && state.selectedVariants[0] == "RANDOM")) {
                        // Use a random variant from the complete list
                        std::vector<SlideVariant> allVariants = generateRandomSlideVariantPool(100); // Get a large pool
                        selectedVariant = allVariants[rand() % allVariants.size()].name;
                    } else {
                        // Use one of the user's selected variants randomly
                        selectedVariant = state.selectedVariants[rand() % state.selectedVariants.size()];
                    }

                    // Apply slide transformation
                    auto transformed = applySlideVariants(noteIndex, duration, DUPLE, selectedVariant);

                    // Track variant usage
                    state.variantUsageCount[selectedVariant]++;

                    // Output the transformed notes
                    for (const auto& [transformedNote, transformedDuration] : transformed) {
                        std::string transNote = getNoteName(transformedNote); // Convert MIDI to readable name
                        output << std::left
                               << std::setw(11) << track
                               << std::setw(11) << transNote
                               << std::setw(20) << transformedDuration
                               << std::setw(20) << label
                               << std::setw(25) << selectedVariant
                               << "\n";
                    }
                } catch (const std::exception& e) {
                    // Handle cases where getNoteNumber produces an error
                    state.statusMessage += "Error processing note '" + noteName + "': " + e.what() + "\n";
                }
            } else {
                // Output original data for notes not selected for transformation
                output << std::left
                       << std::setw(11) << track
                       << std::setw(11) << noteName
                       << std::setw(20) << duration
                       << std::setw(20) << label
                       << std::setw(25) << "ORIGINAL" // Mark as original
                       << "\n";
            }
        } else {
            // Output original data for non-eligible labels
            output << std::left
                   << std::setw(11) << track
                   << std::setw(11) << noteName
                   << std::setw(20) << duration
                   << std::setw(20) << label
                   << std::setw(25) << "" // Empty variant column
                   << "\n";
        }
    }

    input.close();
    output.close();

    // Calculate actual percentage
    double actualPercentage = state.totalEligibleNotes > 0 ?
        (static_cast<double>(state.transformedNotes) / state.totalEligibleNotes) * 100.0 : 0.0;

    // Update result summary
    std::stringstream summary;
    summary << "Transformation Statistics:\n"
            << "Total eligible notes found: " << state.totalEligibleNotes << "\n"
            << "Notes transformed: " << state.transformedNotes << "\n"
            << "Actual transformation percentage: " << std::fixed << std::setprecision(1)
            << actualPercentage << "%\n\n";

    if (state.selectedVariants.size() == 1 && state.selectedVariants[0] != "RANDOM") {
        summary << "Variant used: " << state.selectedVariants[0] << "\n";
    } else if (state.selectedVariants.size() > 1) {
        summary << "Variants used (" << state.selectedVariants.size() << " total):\n";
        for (const auto& [variant, count] : state.variantUsageCount) {
            summary << "  " << variant << ": " << count << " times\n";
        }
    } else {
        summary << "Variant selection: Random\n";
    }

    summary << "Processing complete. Transformed results written to " << outputFile << "\n";
    state.resultSummary = summary.str();
    state.statusMessage = "Processing complete!";
    state.processingComplete = true;
}

// Function to convert processed data to MIDI file with MIDI sync fix
void convertToMidi(const std::string& inputFile, const std::string& outputFile, AppState& state) {
    std::ifstream input(inputFile);
    if (!input.is_open()) {
        state.statusMessage += "Error opening input file: " + inputFile + "\n";
        return;
    }

    // Skip header lines
    std::string line;
    std::getline(input, line); // Skip column headers
    std::getline(input, line); // Skip separator line

    // Parse the file and collect note events
    std::map<int, std::vector<MidiEvent>> trackEvents;
    std::map<int, int> trackPositions; // FIXED: Track positions for sequential notes within each track

    while (std::getline(input, line)) {
        std::istringstream ss(line);
        int track;
        std::string noteName;
        int duration;
        std::string label, variant;

        // Skip lines that don't contain note data
        if (line.empty() || line[0] == '-' || line.find("MIDI File Analyzed") != std::string::npos) {
            continue;
        }

        // Parse the line
        if (!(ss >> track >> noteName >> duration)) {
            continue; // Skip malformed lines
        }

        // Skip header or non-note lines
        if (noteName == "Note" || noteName == "Track") {
            continue;
        }

        try {
            int noteNumber = getNoteNumber(noteName);

            // FIXED: Use track-specific positioning for sequential notes within each track
            int& trackPosition = trackPositions[track];

            // Create note-on event at the track's current position
            MidiEvent noteOn{track, noteNumber, trackPosition, duration, true};
            trackEvents[track].push_back(noteOn);

            // Create note-off event
            MidiEvent noteOff{track, noteNumber, trackPosition + duration, 0, false};
            trackEvents[track].push_back(noteOff);

            // Update the position for this track (notes within a track are sequential)
            trackPosition += duration;

        } catch (const std::exception& e) {
            state.statusMessage += "Error processing note '" + noteName + "': " + std::string(e.what()) + "\n";
        }
    }

    input.close();

    // Write MIDI file
    std::ofstream midiFile(outputFile, std::ios::binary);
    if (!midiFile.is_open()) {
        state.statusMessage += "Error opening output MIDI file: " + outputFile + "\n";
        return;
    }

    // Write MIDI header
    // Format: MThd + <length> + <format> + <tracks> + <division>
    midiFile.write("MThd", 4); // Chunk type

    // Header length (always 6 bytes)
    char headerLength[4] = {0, 0, 0, 6};
    midiFile.write(headerLength, 4);

    // Format (0 = single track, 1 = multiple tracks, same timebase)
    char format[2] = {0, 1};
    midiFile.write(format, 2);

    // Number of tracks
    int numTracks = trackEvents.size();
    char tracksCount[2] = {static_cast<char>((numTracks >> 8) & 0xFF),
                          static_cast<char>(numTracks & 0xFF)};
    midiFile.write(tracksCount, 2);

    // Division (ticks per quarter note = 1024)
    char division[2] = {0x04, 0x00}; // 1024 in big-endian
    midiFile.write(division, 2);

    // Write each track
    for (const auto& [trackNum, events] : trackEvents) {
        // Sort events by time
        std::vector<MidiEvent> sortedEvents = events;
        std::sort(sortedEvents.begin(), sortedEvents.end(),
                 [](const MidiEvent& a, const MidiEvent& b) {
                     return a.startTime < b.startTime ||
                            (a.startTime == b.startTime && !a.isNoteOn && b.isNoteOn);
                 });

        // Write track header
        midiFile.write("MTrk", 4);

        // Placeholder for track length (will be filled in later)
        long trackLengthPos = midiFile.tellp();
        midiFile.write("\0\0\0\0", 4);

        // Track start position
        long trackStartPos = midiFile.tellp();

        // Write track events
        int lastTime = 0;

        // Set instrument (program change) - using piano (0) as default
        char programChange[3] = {0x00, static_cast<char>(0xC0), 0x00}; // Delta time, command, program number
        midiFile.write(programChange, 3);

        for (const auto& event : sortedEvents) {
            // Write delta time (variable length)
            int deltaTime = event.startTime - lastTime;
            lastTime = event.startTime;

            // Convert delta time to variable length quantity
            std::vector<char> vlq;
            if (deltaTime == 0) {
                vlq.push_back(0);
            } else {
                while (deltaTime > 0) {
                    char byte = deltaTime & 0x7F;
                    deltaTime >>= 7;
                    if (!vlq.empty()) {
                        byte |= 0x80;
                    }
                    vlq.push_back(byte);
                }
                std::reverse(vlq.begin(), vlq.end());
            }

            for (char byte : vlq) {
                midiFile.put(byte);
            }

            // Write note event
            if (event.isNoteOn) {
                // Note on: 0x90 | channel, note, velocity
                midiFile.put(0x90);
                midiFile.put(static_cast<char>(event.noteNumber));
                midiFile.put(0x64); // Velocity (100)
            } else {
                // Note off: 0x80 | channel, note, velocity
                midiFile.put(0x80);
                midiFile.put(static_cast<char>(event.noteNumber));
                midiFile.put(0x00); // Velocity (0)
            }
        }

        // Write end of track
        midiFile.put(0x00); // Delta time
        midiFile.put(0xFF); // Meta event
        midiFile.put(0x2F); // End of track
        midiFile.put(0x00); // Length

        // Calculate and write track length
        long trackEndPos = midiFile.tellp();
        long trackLength = trackEndPos - trackStartPos;

        midiFile.seekp(trackLengthPos);
        char trackLengthBytes[4] = {
            static_cast<char>((trackLength >> 24) & 0xFF),
            static_cast<char>((trackLength >> 16) & 0xFF),
            static_cast<char>((trackLength >> 8) & 0xFF),
            static_cast<char>(trackLength & 0xFF)
        };
        midiFile.write(trackLengthBytes, 4);
        midiFile.seekp(trackEndPos);
    }

    midiFile.close();
    state.statusMessage += "MIDI file created successfully: " + outputFile + "\n";
}