// Support Code written by Michael D. Shah
// Last Updated: 6/11/21
// Please do not redistribute without asking permission.

// Functionality that we created
#include "SDLGraphicsProgram.hpp"
#include <string>
#include <iostream>

int main(int argc, char** argv){

    // TODO: if user did not specify an argument, call the program to generate a random height map, else display the specified file
    // TODO: If no time, just generate a random height map each time
    int octaves_specified = 5;
    if (argc == 2) {
        octaves_specified = atoi(argv[1]);
    }
	// Create an instance of an object for a SDLGraphicsProgram
	SDLGraphicsProgram mySDLGraphicsProgram(1280,720, octaves_specified);
	// Run our program forever
	mySDLGraphicsProgram.Loop();
	// When our program ends, it will exit scope, the
	// destructor will then be called and clean up the program.
	return 0;
}
