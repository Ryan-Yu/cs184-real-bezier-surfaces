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

#include "BezierPatch.h"

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
Viewport viewport;
string bezFilename;
string subdivisionMethod;
float subdivisionParameter;
int numberOfBezierPatches;
vector<BezierPatch> listOfBezierPatches;

// Display-related global variables

// if false, then in flat shading mode
bool SMOOTH_SHADING;

// if false, then in filled mode
bool WIREFRAME_MODE;

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
		cout << "Bezier file: " << bezFilename << "\n";
		cout << "Subdivision Parameter: " << subdivisionParameter << "\n";
		cout << "Subdivision Method: " << subdivisionMethod << "\n\n";

		cout << "We currently have " << listOfBezierPatches.size() << " Bezier patches.\n\n";
		// Iterate through Bezier Patches
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			cout << "  Bezier patch " << (i + 1) << ":\n\n";
			std::vector<std::vector <Eigen::Vector3f> > curves = listOfBezierPatches[i].listOfCurves;

			// Iterate through curves in each Bezier patch
			for (std::vector<std::vector <Eigen::Vector3f> >::size_type j = 0; j < curves.size(); j++) {
				std::vector<Eigen::Vector3f> listOfPointsForCurrentCurve = curves[j];

				cout << "    Curve " << (j + 1) << ":\n";

				// Iterate through points in current curve and print them
				for (std::vector<Eigen::Vector3f>::size_type k = 0; k < listOfPointsForCurrentCurve.size(); k++) {
					printf("    (%f, %f, %f)\n", listOfPointsForCurrentCurve[k].x(), listOfPointsForCurrentCurve[k].y(), listOfPointsForCurrentCurve[k].z());
				}
				cout << "\n\n";
			}
		}

		cout << "Display options:\n";
		if (SMOOTH_SHADING) {
			cout << "  Smooth shading is ON.\n";
		} else {
			cout << "  Flat shading is ON. (i.e. smooth shading is OFF)\n";
		}

		if (WIREFRAME_MODE) {
			cout << "  Wireframe mode is ON.\n";
		} else {
			cout << "  Filled mode is ON. (i.e. Wireframe mode is OFF)\n";
		}

	}
}



//****************************************************
// function that parses an input .bez file and initializes
// a list of Bezier patches
//
// psuedocode for parsing .bez file

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
void parseBezierFile(string filename) {

	ifstream file(filename);
	// current line
	string str;

	// current word that we're parsing on a line
	string currentWord;

	// index of the line
	int i = 0;

	// index of a word on each specific line
	int j = 0;

	// coordinates for each set of four points (i.e. one line)
	float p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, p4x, p4y, p4z;

	// number of lines that have already been processed for the current Bezier patch
	int curvesParsedForCurrentPatch = 0;

	BezierPatch currentBezierPatch;

	while (getline(file, str)) {
		// If we encounter a new line, then we know that the next 4 consecutive lines represent
		// 4 curves that will make up a Bezier patch, so we reset our current Bezier patch
		if (str.length() == 0) {
			curvesParsedForCurrentPatch = 0;
			currentBezierPatch = BezierPatch();
			i++;
			continue;

		}
		j = 0;
		if (i == 0) {
			numberOfBezierPatches = stoi(str);
			i++;
			continue;
		}

		cout << str << "\n";

		istringstream iss(str);
		// Iterate through words on a single line...
		while (iss >> currentWord) {
			if (j == 0) { p1x = stof(currentWord); }
			if (j == 1) { p1y = stof(currentWord); }
			if (j == 2) { p1z = stof(currentWord); }
			if (j == 3) { p2x = stof(currentWord); }
			if (j == 4) { p2y = stof(currentWord); }
			if (j == 5) { p2z = stof(currentWord); }
			if (j == 6) { p3x = stof(currentWord); }
			if (j == 7) { p3y = stof(currentWord); }
			if (j == 8) { p3z = stof(currentWord); }
			if (j == 9) { p4x = stof(currentWord); }
			if (j == 10) { p4y = stof(currentWord); }
			if (j == 11) { p4z = stof(currentWord); }
			j++;
		}

		Eigen::Vector3f point1(p1x, p1y, p1z);
		Eigen::Vector3f point2(p2x, p2y, p2z);
		Eigen::Vector3f point3(p3x, p3y, p3z);
		Eigen::Vector3f point4(p4x, p4y, p4z);

		// 1 of the 4 curves for the currentBezierPatch
		vector<Eigen::Vector3f> currentCurve;
		currentCurve.push_back(point1);
		currentCurve.push_back(point2);
		currentCurve.push_back(point3);
		currentCurve.push_back(point4);

		currentBezierPatch.addCurve(currentCurve);
		curvesParsedForCurrentPatch++;

		// We have parsed all four curves for our current patch
		if (curvesParsedForCurrentPatch == 4) {
			listOfBezierPatches.push_back(currentBezierPatch);
		}

		i++;
	}

}



//****************************************************
// function that parses command line options,
// given number of command line arguments (argc)
// and the argument array (argv)
// Format:
// % as3 inputfile.bez 0.1 -a
//***************************************************
void parseCommandLineOptions(int argc, char *argv[])
{

	if (argc != 3 && argc != 4) {
		cout << "Wrong format for command line input, terminating program.";
		exit(1);
	}

	bezFilename = argv[1];
	subdivisionParameter = stof(argv[2]);
	if (argc == 3) {
		subdivisionMethod = "UNIFORM";
	} else {
		// We have a fourth command line parameter
		if (argv[3] == "-a") {
			subdivisionMethod = "ADAPTIVE";
		} else {
			// Fourth command line parameter is invalid
			cout << "Must specific subdivision method, terminating program.";
			exit(1);
		}
	}

	parseBezierFile(bezFilename);
}



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








