#include "Button.hpp"

bool Button::isInside(int mouseX, int mouseY) {
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

void Button::handleClick() {
	if (buttonAction) {
		buttonAction(*this); // Invoke the action handler
	}
}

void Button::draw() {
	// Calculate the position to center the button
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the button background using the calculated center position
	glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the button
	glBegin(GL_QUADS);
	glVertex3f(centerX, centerY, 0);
	glVertex3f(centerX + width, centerY, 0);
	glVertex3f(centerX + width, centerY + height, 0);
	glVertex3f(centerX, centerY + height, 0);
	glEnd();

	// Adjust horizontal position to center the text horizontally within the button
	float textX = centerX + 0.1;

	// Adjust vertical position to center the text vertically within the button
	float textY = centerY + (height - 0.2) / 2;

	// Draw the button text
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
	glRasterPos3f(textX, textY, 1);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	}

}

void Button::drawAssetButton(GLuint texID[]) {
	// Calculate the position to center the button
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the button background using the calculated center position
	glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the button
	glBegin(GL_QUADS);
	glVertex3f(centerX, centerY, 0);
	glVertex3f(centerX + width, centerY, 0);
	glVertex3f(centerX + width, centerY + height, 0);
	glVertex3f(centerX, centerY + height, 0);
	glEnd();

	float widthAdjust = centerX + width / 10;
	float heightAdjust = centerY + height / 3;

	if (sprite) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glEnable(GL_TEXTURE_2D); // Enable texturing

		glBindTexture(GL_TEXTURE_2D, texID[textureNum]); // Which texture

		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(widthAdjust, heightAdjust, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust, 0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust + (height / 2), 0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(widthAdjust, heightAdjust + (height / 2), 0);
		glEnd();

		glDisable(GL_TEXTURE_2D); // Turn texturing off
	}

	else {
		glColor3f(1, 0, 1); // Purple color for the sprite in the button
		glBegin(GL_QUADS);
		glVertex3f(widthAdjust, heightAdjust, 0);
		glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust, 0);
		glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust + (height / 2), 0);
		glVertex3f(widthAdjust, heightAdjust + (height / 2), 0);
		glEnd();
	}

	// Adjust horizontal position to center the text horizontally within the button
	float textX = centerX + 0.1;

	// Adjust vertical position to center the text vertically within the button
	float textY = centerY + (height / 10);

	// Draw the button text
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
	glRasterPos3f(textX, textY, 1);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	}
}

// Method to handle button click
void Toggle::handleClick() {
	toggled = !toggled;
	if (toggle) {
		toggle(*this); // Invoke the toggle action handler
	}
}

// Method to draw the button
void Toggle::draw() {
	// Calculate the position to center the button
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the button background using the calculated center position
	glColor3f(0.7f, 0.7f, 0.7f); // Light gray color for the button
	glBegin(GL_QUADS);
	glVertex3f(centerX, centerY, 0);
	glVertex3f(centerX + width, centerY, 0);
	glVertex3f(centerX + width, centerY + height, 0);
	glVertex3f(centerX, centerY + height, 0);
	glEnd();

	// Define separate width and height for the smaller square
	float smallSquareWidth = width * 0.6f;
	float smallSquareHeight = height * 0.6f;

	// Calculate the position to center the smaller square
	float smallSquareX = centerX + (width - smallSquareWidth) / 2;
	float smallSquareY = centerY + (height - smallSquareHeight) / 2;

	// Draw the button background using the calculated center position
	if (toggled) {
		// Draw a smaller filled square inside the button
		glColor3f(0.0f, 1.0f, 0.0f); // White color for the small square
		glBegin(GL_QUADS);
		glVertex3f(smallSquareX, smallSquareY, 0);
		glVertex3f(smallSquareX + smallSquareWidth, smallSquareY, 0);
		glVertex3f(smallSquareX + smallSquareWidth, smallSquareY + smallSquareHeight, 0);
		glVertex3f(smallSquareX, smallSquareY + smallSquareHeight, 0);
		glEnd();
	}
	else {
		// Draw a smaller filled square inside the button
		glColor3f(1.0f, 1.0f, 1.0f); // White color for the small square
		glBegin(GL_QUADS);
		glVertex3f(smallSquareX, smallSquareY, 0);
		glVertex3f(smallSquareX + smallSquareWidth, smallSquareY, 0);
		glVertex3f(smallSquareX + smallSquareWidth, smallSquareY + smallSquareHeight, 0);
		glVertex3f(smallSquareX, smallSquareY + smallSquareHeight, 0);
		glEnd();
	}
	}