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

		// Used for gluPerspective()
		float zNear, zFar;
		float FIELD_OF_VIEW;

		// Current amount that the camera is zoomed in (default = 1.0)
		float ZOOM_AMOUNT;

		// The delta that the zoom amount gets changed by on each key press
		float ZOOM_DELTA;

		// TODO: Add rotation factor and delta
		float X_ROTATION_AMOUNT;
		float Y_ROTATION_AMOUNT;

		float ROTATION_DELTA;

		// TODO: Add translation factor (should this be in Scene instead?)


	Camera() {
		position = lookAt = up = minimum = maximum = Eigen::Vector3f(0, 0, 0);
		zNear = zFar = 0.0;
		FIELD_OF_VIEW = 90.0f;
		ZOOM_AMOUNT = 1.0f;
		ZOOM_DELTA = 0.1;
		ROTATION_DELTA = 3.0f;
		X_ROTATION_AMOUNT = Y_ROTATION_AMOUNT = 0.0;
	}

	void zoomIn() {
		ZOOM_AMOUNT -= ZOOM_DELTA;
	}

	void zoomOut() {
		ZOOM_AMOUNT += ZOOM_DELTA;
	}

	void resetCamera() {
		FIELD_OF_VIEW = 90.0f;
		ZOOM_AMOUNT = 1.0f;
		ZOOM_DELTA = 0.1;
		X_ROTATION_AMOUNT = 0.0;
		Y_ROTATION_AMOUNT = 0.0;
	}

	void rotateLeft() {
		X_ROTATION_AMOUNT -= ROTATION_DELTA;
	}

	void rotateRight() {
		X_ROTATION_AMOUNT += ROTATION_DELTA;
	}

	void rotateUp() {
		Y_ROTATION_AMOUNT += ROTATION_DELTA;
	}

	void rotateDown() {
		Y_ROTATION_AMOUNT -= ROTATION_DELTA;
	}

};



#endif /* CAMERA_H_ */
