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
	Eigen::Vector3f point1, point2, point3;

	Triangle() {

	}

	Triangle(Eigen::Vector3f v1, Eigen::Vector3f v2, Eigen::Vector3f v3) {
		this->point1 = v1;
		this->point2 = v2;
		this->point3 = v3;
	}

	Triangle(float ax, float ay, float az,
			float bx, float by, float bz,
			float cx, float cy, float cz) {
		this->point1 = Eigen::Vector3f(ax, ay, az);
		this->point2 = Eigen::Vector3f(bx, by, bz);
		this->point3 = Eigen::Vector3f(cx, cy, cz);
	}


};

#endif /* TRIANGLE_H_ */
