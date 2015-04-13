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
	// Given the control points of a Bezier curve and a parametric value,
	// return the curve point and derivative.
	// This is a helper method that is used in 'evaulateDifferentialGeometry'.
	//
	// NOTE: This method is given in the last slide of CS184 Spring 2015 Lecture 14 (O'Brien)
	//***************************************************
	CurveLocalGeometry interpretBezierCurve(std::vector<Eigen::Vector3f> curve, float u) {
		// First, split each of the three segments to form two new ones, AB and BC
		// NOTE: 'curve' is a length-4 list of Vector3f's. Each Vector3f represents a control point of the curve.
		Eigen::Vector3f A = (curve[0] * (1.0 - u)) + (curve[1] * u);
		Eigen::Vector3f B = (curve[1] * (1.0 - u)) + (curve[2] * u);
		Eigen::Vector3f C = (curve[2] * (1.0 - u)) + (curve[3] * u);

		// Now, split AB and BC to form a new segment DE
		Eigen::Vector3f D = (A * (1.0 - u)) + (B * u);
		Eigen::Vector3f E = (B * (1.0 - u)) + (C * u);

		// Finally, pick the right point on DE; this is the point on the curve
		Eigen::Vector3f point = (D * (1.0 - u)) + (E * u);

		// Then, compute the derivative
		Eigen::Vector3f derivative = 3.0 * (E - D);

		return CurveLocalGeometry(point, derivative);
	}


	//****************************************************
	// Method that generates a DifferentialGeometry object that represents
	// the result of evaluating 'this' BezierPatch at (u, v)
	//
	// NOTE: This method is given in the last slide of CS184 Spring 2015 Lecture 14 (O'Brien)
	//***************************************************
	DifferentialGeometry evaluateDifferentialGeometry(float u, float v) {
		// listOfCurves[i] returns a list of points that represents one curve

		// Build control points for a Bezier curve in v
		Eigen::Vector3f vCurve0 = interpretBezierCurve(listOfCurves[0], u).point;
		Eigen::Vector3f vCurve1 = interpretBezierCurve(listOfCurves[1], u).point;
		Eigen::Vector3f vCurve2 = interpretBezierCurve(listOfCurves[2], u).point;
		Eigen::Vector3f vCurve3 = interpretBezierCurve(listOfCurves[3], u).point;

		// Build control points for a Bezier curve in v
		Eigen::Vector3f uCurve0 = interpretBezierCurve(listOfCurves[0], v).point;
		Eigen::Vector3f uCurve1 = interpretBezierCurve(listOfCurves[1], v).point;
		Eigen::Vector3f uCurve2 = interpretBezierCurve(listOfCurves[2], v).point;
		Eigen::Vector3f uCurve3 = interpretBezierCurve(listOfCurves[3], v).point;

		// Evaluate surface and derivative for u and v
		std::vector<Eigen::Vector3f> vCurve;
		vCurve.push_back(vCurve0);
		vCurve.push_back(vCurve1);
		vCurve.push_back(vCurve2);
		vCurve.push_back(vCurve3);

		std::vector<Eigen::Vector3f> uCurve;
		uCurve.push_back(uCurve0);
		uCurve.push_back(uCurve1);
		uCurve.push_back(uCurve2);
		uCurve.push_back(uCurve3);

		CurveLocalGeometry finalVCurve = interpretBezierCurve(vCurve, v);
		CurveLocalGeometry finalUCurve = interpretBezierCurve(uCurve, u);

		// Take cross product of partials to find normal
		Eigen::Vector3f normal = finalVCurve.derivative.cross(finalUCurve.derivative);
		normal.normalize();

		return DifferentialGeometry(finalVCurve.point, normal, Eigen::Vector2f(u, v));
	}


	//****************************************************
	// TODO: Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on adaptive subdivision
	//***************************************************
	void performAdaptiveSubdivision(float error) {

	}


	//****************************************************
	// Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on uniform subdivision
	//***************************************************
	void performUniformSubdivision(float stepSize) {
		float epsilon = 0.001;
		int numberOfSteps = (1.0 + epsilon) / stepSize;
		for (int u = 0; u < numberOfSteps; u++) {
			for (int v = 0; v < numberOfSteps; v++) {
				// Evaluate the differential geometry at (u * stepSize, v * stepSize(
				// For instance, if stepSize = 0.1, then we would evaluate at (0, 0), (0, 0.1), (0, 0.2), etc
				listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(u * stepSize, v * stepSize));
			}
		}

		// NOTE: Code confirmed as working (tested)
		// Populate the list of Triangles, based on the list of points in listOfDifferentialGeometries
		// By ordering above, the DifferentialGeometries are ordered with the following numbering:
		//
		// 1 6  11 16 21
		// 2 7  12 17 22
		// 3 8  13 18 23
		// 4 9  14 19 24
		// 5 10 15 20 25
		//
		// where numberOfSteps = 5
		//
		// NOTE: By construction, our grid will always be n x n (i.e. a square)
		//
		// Basic Algorithm: for every 4 points in a rectangle, like below,
		//
		// 1 2
		// 3 4
		//
		// pair the points up in two triangles like so:
		//
		// 1--2
		// |/ |
		// 3--4
		//
		// so that 1-2-3 form a triangle (tri-1) and 2-3-4 form a triangle (tri-2)

		// Iterate through all of our differential geometries, but do NOT touch the right-most column and the bottom-most row
		for (int u = 0; u < (numberOfSteps - 1); u++) {
			for (int v = 0; v < (numberOfSteps - 1); v++) {
				// (u, v) represents the index in the above grid that we're triangulating
				// This index represents the TOP LEFT corner of the 4-point rectangle that is described above

				// Index of listOfDifferentialGeometries that corresponds with position (u, v)
				int differentialGeometrixIndex = (u * numberOfSteps) + v;

				// Construct tri-1
				listOfTriangles.push_back(Triangle(
						listOfDifferentialGeometries[differentialGeometrixIndex], // top left
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps], // move one unit right
						listOfDifferentialGeometries[differentialGeometrixIndex + 1])); // move one unit down from top left

				// Construct tri-2
				listOfTriangles.push_back(Triangle(
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps], // top right
						listOfDifferentialGeometries[differentialGeometrixIndex + 1], // bottom left
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps + 1])); // bottom right
			}
		}

		// We should have (numberOfSteps - 1) * (numberOfSteps - 1) * 2 triangles
	}
};

#endif /* BEZIERPATCH_H_ */
