#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <bitset>
#include <algorithm>

#include "Eigen/Geometry"

inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};



//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;

bool debug;



//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}




//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}



//****************************************************
// Draw a filled circle.  
//****************************************************

// For viewport of [400 x 400], center X = 200, center Y = 200
void circle(float centerX, float centerY, float radius) {

  // Draw inner circle
  glBegin(GL_POINTS);

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));

  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      // if current pixel (i, j) is inside the bounds of the circle
      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);

        // setPixel(i,j, resultant_rgb_sum_of_pixel_r, resultant_rgb_sum_of_pixel_g, resultant_rgb_sum_of_pixel_b);
      }

    }
  }

  glEnd();
}



//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  // Ensure that the diameter of the circle is 90% of min(width, height)
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) * 0.45);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// function that assists with exit of program on space bar press
//***************************************************
void exitOnSpaceBarPress( unsigned char key, int x, int y )
{
    switch ( key ) 
    {
    // Space bar
    case ' ':
        exit(1);
    }
}


//****************************************************
// function that prints all of our command line option variables
//***************************************************
void printCommandLineOptionVariables( )
{
  if (debug)
  {
    
  }
}


//****************************************************
// function that parses command line options,
// given number of command line arguments (argc)
// and the argument array (argv)
//***************************************************
void parseCommandLineOptions(int argc, char *argv[])
{
  string flag;

  int i = 1;
  while (i <= argc - 1) {

    flag = argv[i];

    // Advance to next flag, if one exists
    i++;
  }
}


//****************************************************
// psuedocode for parsing .bez file
//****************************************************

/*

BezierPatch currentBezierPatch
for each collection of 4 rows that correspond to one Bezier patch:
    for i between 1 and 4 (ie. each of the 4 rows for the current patch):
        Vect3 point1 = first row
        Vect3 point2 = second row
        Vect3 point3 = third row
        Vect3 point4 = fourth row
        vector<Vect3> curveI;
        curveI.push_back(point1);
        curveI.push_back(point2);
        curveI.push_back(point3);
        curveI.push_back(point4);
        currentBezierPatch.addCurve(curveI);
    all_bezier_patches.addPatch(currentBezierPatch);

*/



//****************************************************
// psuedocode for... everything
//****************************************************

/*

- Parse command line arguments to get subdivision parameter and subdivision method (adaptive vs. uniform)

- Use parsed command line arguments to initialize a list of all Bezier patches (with psuedocode above)

- Iterate through list of patches and subdivide all of the patches in the list, either adapatively or uniformly

  - for uniform subdivision:
    - take given u | v step size, initialize a list of LocalGeometry objects, starting from (u, v) = (0, 0) and moving
      up by u = v = stepsize each time. NOTE: this list of LocalGeometry objects is specific for the current patch.
      That is, the list of LocalGeometry objects will be given by currentPatch.local_geo_list
    - take list of LocalGeometry objects and generate a list of triangles with it. NOTE: this list of triangles is specific
      for the current patch. That is, it is given by currentPatch.triangle_list

  - for adaptive subdivision:
    - split based on lecture slides
      (http://www.cs.berkeley.edu/~job/Classes/CS184/Spring-2015-Slides/14-Surfaces.pdf)

- At this point, each of the patches (ie. surfaces) has its own list of LocalGeometry objects and its own list of triangles

- Create an overarching list of LocalGeometry objects and an overarching list of Triangle objects, and aggregate
  all LocalGeometry and Triangle objects, respectively, into these two overarching lists

- Define myDisplay():
  - Set the gluLookAt, and other OpenGL variables (?)
  - for each BezierPatch in the Scene's BezierPatch list:
    - for each Triangle in the current BezierPatch's triangle list (ie. currentPatch.triangle_list):
      - set OpenGL Vertex3f/Normal3f and other things to draw the current triangle
      - NOTE: need to change this slightly when wireframe mode/other modes are active/non-active

- Call glutDisplayFunc(myDisplay)

*/


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

  // Turns debug mode ON or OFF
  debug = true;

  // This initializes glut
  glutInit(&argc, argv);

  // Parse command line options
  parseCommandLineOptions(argc, argv);
  printCommandLineOptionVariables();

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 1000;
  viewport.h = 1000;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized

  // Program exits if space bar is pressed
  glutKeyboardFunc( exitOnSpaceBarPress );

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








