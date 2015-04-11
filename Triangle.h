/*
 * Triangle.h
 *
 *  Created on: Apr 11, 2015
 *      Author: ryanyu
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

class Triangle {
public:
	DifferentialGeometry point1, point2, point3;

	Triangle() {

	}

	Triangle(DifferentialGeometry v1, DifferentialGeometry v2, DifferentialGeometry v3) {
		this->point1 = v1;
		this->point2 = v2;
		this->point3 = v3;
	}


};

#endif /* TRIANGLE_H_ */
