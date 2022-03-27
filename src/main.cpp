#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "Board.h"

int main(int argc, char **argv) {
    try {  
        TCLAP::CmdLine cmd("Report bugs to nickolasgd@gmail.com", ' ', "0.1");

        TCLAP::ValueArg<unsigned> widthArg("", "width", "Width of the window. Defaults to 1980", false, 1980, "number", cmd);
        TCLAP::ValueArg<unsigned> heightArg("", "height", "Height of the window. Defaults to 1020", false, 1020, "number", cmd);
        TCLAP::ValueArg<unsigned> scaleArg("", "scale", "The size of the window relative to the board. Larger numbers result in smaller board sizes. Defaults to 1", false, 1, "number", cmd);
        TCLAP::ValueArg<unsigned> drawArg("", "draw-limit", "This limits the draw rate of the program. 0 uncaps the draw rate. Frames are skipped if this is lower than update-limit. Defaults to 0", false, 0, "number", cmd);
        TCLAP::ValueArg<unsigned> updateArg("", "update-limit", "This limits the update rate of the program. 0 uncaps the update rate. Defaults to 0", false, 0, "number", cmd);
        TCLAP::ValueArg<unsigned> threadArg("", "threads", "This sets the number of threads used to calculate next step. Defaults to 1", false, 1, "number", cmd);
        TCLAP::ValueArg<unsigned> seedArg("", "seed", "This sets the seed for random board generation. Uses system time if 0. Defaults to 0", false, 0, "number", cmd);
        TCLAP::ValueArg<bool> pngArg("", "png", "1 outputs each frame to a png in a subdirectory. Defaults to 0", false, 0, "1 or 0", cmd);

        cmd.parse( argc, argv );

        std::cout << "Width: " << widthArg.getValue() << '\n';
        std::cout << "Height: " << heightArg.getValue() << '\n';
        std::cout << "Scale: " << scaleArg.getValue() << '\n';
        std::cout << "Draw limit: " << drawArg.getValue() << '\n';
        std::cout << "Update limit: " << updateArg.getValue() << '\n';
        std::cout << "Update threads: " << threadArg.getValue() << '\n';
        std::cout << "Output PNG: " << pngArg.getValue() << '\n';

        Board game(widthArg.getValue(), heightArg.getValue(), scaleArg.getValue(), drawArg.getValue(), updateArg.getValue(), threadArg.getValue(), seedArg.getValue(), pngArg.getValue());

    } catch (TCLAP::ArgException &e) { 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    
    return 0;
}