#pragma once
// Samuel Markus
// 2/5/2025
// GameObject parent class

#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP
/// Includes
#include <GL/glut.h>
#include <string>
#include <sstream>
#include "GameObject.hpp"
using std::string; using std::stringstream;


class TextInput {
public:
	float x, y; // Position of the text input box in the viewport
	float width, height; // Dimensions of the text input box in the viewport
	float windowX, windowY; // Position of the button in the window
	float windowWidth, windowHeight; // Dimensions of the button in the window
	string text; // Text to be displayed in the text input box
	GameObject* gameObject; // Used to get and set the variables of the game object
	bool active; // Indicates whether the text input box is active (selected for input)
	int mode = 0; // Indicates which value is being changed

	TextInput(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		const string& txt, int mode)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), mode(mode), active(false), gameObject(nullptr) {
	}

	void setActive(bool isActive) {
		active = isActive;
	}

	// Convert text to float
	float textToFloat() {
		float result;
		stringstream ss(text);
		ss >> result;
		return result;
	}

	void handleKeyPress(unsigned char key) {
		if (active) {
			if (key == '\b' && !text.empty()) { // Backspace key
				text.pop_back();
			}
			else if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
				text += key;
			}
			if (gameObject != nullptr)
				// Update the x value whenever a valid character is entered
				switch (mode)
				{
				case(0):
					gameObject->x = textToFloat();
					break;
				case(1):
					gameObject->y = textToFloat();
					break;
				case(2):
					gameObject->sizeX = textToFloat();
					break;
				case(3):
					gameObject->sizeY = textToFloat();
					break;
				}
		}
	}

	bool isInside(int mouseX, int mouseY) {
		// Calculate the boundaries of the button
		float leftBoundary = windowX - windowWidth / 2;
		float rightBoundary = windowX + windowWidth / 2;
		float bottomBoundary = windowY - windowHeight / 2;
		float topBoundary = windowY + windowHeight / 2;

		// Check if mouseX falls within the button's x-boundaries
		bool insideX = (mouseX >= leftBoundary && mouseX <= rightBoundary);

		// Check if mouseY falls within the button's y-boundaries
		bool insideY = (mouseY >= bottomBoundary && mouseY <= topBoundary);

		// Return true only if both mouseX and mouseY fall within the button's boundaries
		return insideX && insideY;
	}

	void draw() {
		// Draw the text input box background
		glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the text input box
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();

		// Draw the text inside the text input box
		glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
		glRasterPos2f(x + 0.05f, y + (height / 2) - 0.05f); // Adjust position for centering
		for (char c : text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
		}
	}
};

#endif