/*
 * BezierPatch.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ryanyu
 */

#ifndef BEZIERPATCH_H_
#define BEZIERPATCH_H_

class BezierPatch {
	public:
		std::vector<std::vector <Eigen::Vector3f> > listOfCurves;

		// final list of subdivided triangles, ready to feed to OpenGL display system
		std::vector<Triangle> listOfTriangles;

		// list of differential geometries (i.e. points) that we are evaluating the given patch at
		std::vector<DifferentialGeometry> listOfDifferentialGeometries;

	BezierPatch() {

	}

	// Adds a curve to the list of curves.
	// NOTE: A curve, at initialization from the command line, is represented by a length-4 list of Vector3f's.
	//       That is, a curve is represented by a list of four points.
	void addCurve(std::vector<Eigen::Vector3f> curve) {
		listOfCurves.push_back(curve);
	}

	void addTriangle(DifferentialGeometry vertex1, DifferentialGeometry vertex2, DifferentialGeometry vertex3) {
		listOfTriangles.push_back(Triangle(vertex1, vertex2, vertex3));
	}

	void addDifferentialGeometry(Eigen::Vector3f position, Eigen::Vector3f normal, Eigen::Vector2f uvValues) {
		listOfDifferentialGeometries.push_back(DifferentialGeometry(position, normal, uvValues));
	}


	//****************************************************
	// TODO: Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on adaptive subdivision
	//***************************************************
	void performAdaptiveSubdivision(float error) {

	}


	//****************************************************
	// TODO: Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on uniform subdivision
	//***************************************************
	void performUniformSubdivision(float stepSize) {

	}
};

#endif /* BEZIERPATCH_H_ */
