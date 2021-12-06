#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

class Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(class Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();
short height = input->height;
short width = input->width;
output -> width = width;
output -> height = height;
int red;
int blue;
int green;
short div = filter->getDivisor(); //put this in a variable so it wouldn't get called so many times in the loop
int * d = filter->data; //data array to access values directly instead of having to use the filter->get() function
//loop reordering 
#pragma omp parallel for
for(int row = 1; row < height-1 ; row++) {
  for(int col = 1; col < width-1; col++) {
      red=0;
      blue=0;
      green=0;

//unrolled plane
      for (int j = 0; j < 3; j++) { 
        for (int i = 0; i < 3; i++) {
            red+=input->color[COLOR_RED][row + i - 1][col + j - 1] * d[i * 3 + j];
            blue+=input->color[COLOR_BLUE][row + i - 1][col + j - 1] * d[i * 3 + j];
            green+=input->color[COLOR_GREEN][row + i - 1][col + j - 1] * d[i * 3 + j];
        }
      }

	 
	red = red/div;
    blue = blue/div;
    green = green/div;
      
    red = (red<0) ? 0: red; //changed if statement to a conditional ternary operator
    red = (red>255) ? 255: red; //changed if statement to a conditional ternary operator
      
    blue = (blue<0) ? 0: blue; //changed if statement to a conditional ternary operator
    blue = (blue>255) ? 255: blue; //changed if statement to a conditional ternary operator
      
    green = (green<0) ? 0: green; //changed if statement to a conditional ternary operator
    green = (green>255) ? 255: green; //changed if statement to a conditional ternary operator
      
    output->color[COLOR_RED][row][col]=red; //setting back to color variable that was changed during program
    output->color[COLOR_BLUE][row][col]=blue; //setting back to color variable that was changed during program
    output->color[COLOR_GREEN][row][col]=green; //setting back to color variable that was changed during program
      }
    }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (width * height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (width * height));
  return diffPerPixel;
}
