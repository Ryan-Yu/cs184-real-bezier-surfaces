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

#include "Camera.h"
#include "DifferentialGeometry.h"
#include "Triangle.h"
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
Camera camera;
Viewport viewport;
string bezFilename;
string subdivisionMethod;
float subdivisionParameter;
int numberOfBezierPatches;
vector<BezierPatch> listOfBezierPatches;

// ***** Display-related global variables ***** //

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
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

	// clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	// make sure transformation is "zero'd"
	glLoadIdentity();

	float aspect_ratio = ((float) viewport.w) / ((float) viewport.h);

	// set OpenGL viewport
	glViewport(0, 0, viewport.w, viewport.h);

	// TODO: Set OpenGL perspective:
	// void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

	gluPerspective(camera.FIELD_OF_VIEW * camera.ZOOM_AMOUNT, aspect_ratio, camera.zNear, camera.zFar);

	// set shading of model to smooth or flat, based on our global variable
	if (SMOOTH_SHADING) {
		glShadeModel(GL_SMOOTH);
	} else {
		glShadeModel(GL_FLAT);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set camera, via the following:
	// void gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY,
	//                GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
	gluLookAt(camera.position.x(), camera.position.y(), camera.position.z(), camera.lookAt.x(), camera.lookAt.y(),
			camera.lookAt.z(), camera.up.x(), camera.up.y(), camera.up.z());

	// TODO: Set translate and rotate
	// glPushMatrix();
	// glTranslatef();
	// glRotatef();
	// glRotatef();

	/*
	Begin drawing all of the triangles
	PSUEDOCODE:

	for each BezierPatch in the scene's list of Bezier patches:
		for each Triangle in the current Bezier patch's list of triangles
			Grab the three vertices of the triangle and render the triangle

	*/

	glPopMatrix();

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// function that assists with exit of program on space bar press
//***************************************************
void keyPressed( unsigned char key, int x, int y )
{
	switch ( key )
	{
	// Space bar: exit program
	case ' ':
		exit(1);

	case 's':
		// Toggle between flat and smooth shading, but only if we aren't in wireframe mode
		if (!WIREFRAME_MODE) {
			SMOOTH_SHADING = !SMOOTH_SHADING;
			if (debug) {
				if (SMOOTH_SHADING) {
					cout << "Turned smooth shading ON.\n";
				} else {
					cout << "Turned smooth shading OFF.\n";
				}
			}
		}
		break;

	case 'w':
		// Toggle between wireframe mode and filled mode
		WIREFRAME_MODE = !WIREFRAME_MODE;
		if (debug) {
			if (WIREFRAME_MODE) {
				cout << "Turned wireframe mode ON.\n";
			} else {
				cout << "Turned wireframe mode OFF.\n";
			}
		}
		break;

	}

	// TODO: arrow keys = object rotated
	// TODO: shift + arrow keys = object translated (need to use special key for this, probably)
	// TODO: +/- keys = zoom in/out

}


//****************************************************
// function that prints all of our command line option variables
//***************************************************
void printCommandLineOptionVariables( )
{
	if (debug)
	{
		cout << "\nBezier file: " << bezFilename << "\n";
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
// TODO: method that populates each BezierPatch's list of DifferentialGeometries
// and list of Triangles, based on what kind of subdivision (i.e. adaptive or uniform)
// we are performing
//***************************************************
void perform_subdivision(bool adaptive_subdivision) {
	// Iterate through each of the Bezier patches...
	for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
		BezierPatch currentBezierPatch = listOfBezierPatches[i];
		if (adaptive_subdivision) {
			currentBezierPatch.performAdaptiveSubdivision(subdivisionParameter);
		} else {
			currentBezierPatch.performUniformSubdivision(subdivisionParameter);
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

	// Perform subdivision of BezierPatches, based on whether we want to adaptively or uniformly subdivide
	if (subdivisionMethod == "ADAPTIVE") {
		perform_subdivision(true);
	} else if (subdivisionMethod == "UNIFORM") {
		perform_subdivision(false);
	} else {
		cout << "Invalid subdivision method, terminating program.";
		exit(1);
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
// Initializes the camera's vector instance variables,
// based on each BezierPatch's DifferentialGeometry objects
//
// NOTE: This method MUST be called AFTER all Bezier objects and their
//       DifferentialGeometry / Triangle lists are finished being initialized
//****************************************************
void initializeCamera() {
	// First, we iterate through all of the objects in our scene and we determine the minimum and maximum x, y, z values over all objects

	float xMin, xMax, yMin, yMax, zMin, zMax;

	xMin = yMin = zMin = numeric_limits<int>::max();
	xMax = yMax = zMax = numeric_limits<int>::min();

	// Iterate through all BezierPatches...
	for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
		BezierPatch currentBezierPatch = listOfBezierPatches[i];

		// Iterate through each BezierPatch's DifferentialGeometries...
		for (std::vector<DifferentialGeometry>::size_type j = 0; j < currentBezierPatch.listOfDifferentialGeometries.size(); j++) {
			Eigen::Vector3f currentDifferentialGeometryPosition = currentBezierPatch.listOfDifferentialGeometries[j].position;

			// Update min's, if applicable
			if (currentDifferentialGeometryPosition.x() < xMin) {
				xMin = currentDifferentialGeometryPosition.x();
			}
			if (currentDifferentialGeometryPosition.y() < yMin) {
				yMin = currentDifferentialGeometryPosition.y();
			}
			if (currentDifferentialGeometryPosition.z() < zMin) {
				zMin = currentDifferentialGeometryPosition.z();
			}

			// Update max's, if applicable
			if (currentDifferentialGeometryPosition.x() > xMax) {
				xMax = currentDifferentialGeometryPosition.x();
			}
			if (currentDifferentialGeometryPosition.y() > yMax) {
				yMax = currentDifferentialGeometryPosition.y();
			}
			if (currentDifferentialGeometryPosition.z() > zMax) {
				zMax = currentDifferentialGeometryPosition.z();
			}

		}
	}

	// At this point, xMin, xMax, yMin, yMax, zMin, zMax are initialized, and form a box that has dimensions
	// (xMax - xMin)  x  (yMax - yMin)  x  (zMax - zMin)
	float xLength = xMax - xMin;
	float yLength = yMax - yMin;
	float zLength = zMax - zMin;

	// First, we find the CENTER of our x/y/z min/max values, and this becomes our camera's lookAt vector
	Eigen::Vector3f center((xMin + xMax) / 2.0f, (yMin + yMax) / 2.0f, (zMin + zMax) / 2.0f);
	camera.lookAt = center;

	// Set the camera's position to (x, y) = (0, 0). The z-coordinate is the length of the largest coordinate
	// between xLength, yLength, zLength as defined above, so that the object will always be visible in the scope of the camera's lens
	camera.position = Eigen::Vector3f(0.0, 0.0, 2.0f * fmax(zLength, fmax(xLength, yLength)));

	// Hardcode camera up vector to (0, 1, 0)
	camera.up = Eigen::Vector3f(0, 1.0f, 0);
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

	// At this point, all subdivision of Bezier Patches has been completed

	// Initialize position, lookAt, and up vectors of camera so that we may feed them into OpenGL rendering system later
	initializeCamera();

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

	// Handles key presses
	glutKeyboardFunc( keyPressed );

	glutMainLoop();							// infinite loop that will keep drawing and resizing
	// and whatever else

	return 0;
}








