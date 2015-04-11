/*
 * Camera.h
 *
 *  Created on: Apr 11, 2015
 *      Author: ryanyu
 */

#ifndef CAMERA_H_
#define CAMERA_H_

class Camera {
	public:
		Eigen::Vector3f position, lookAt, up;

		// Used to determine bounding box for OpenGL
		Eigen::Vector3f minimum, maximum;

	Camera() {
		position = lookAt = up = minimum = maximum = Eigen::Vector3f(0, 0, 0);
	}

};



#endif /* CAMERA_H_ */
