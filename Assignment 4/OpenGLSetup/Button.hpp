#pragma once
// Samuel Markus
// 4/7/2025
// Button parent class

#ifndef BUTTON_HPP
#define BUTTON_HPP
/// Includes
#include <GL/glut.h>
#include <string>
#include <list>
#include <functional>
#include <iostream>
#include "GameObject.hpp"
using std::string;
using std::function;
using std::list;

// Button class
class Button {
public:
	float x, y; // Position of the button in the viewport
	float width, height; // Dimensions of the button in the viewport
	float windowX, windowY; // Position of the button in the window
	float windowWidth, windowHeight; // Dimensions of the button in the window
	string text; // Text to be displayed on the button
	bool clicked; // Indicates whether the button has been clicked
	function<void(Button&)> buttonAction; // Action handler for the button
	// Had to update here to allow for button passing
	GameObject* gameObject; // Used to get and set the variables of the game object
	int textureNum; // Texture number
	bool sprite; // If there is a sprite or not

	// x, y, w, h, windowX, windowY, windowWidth, windowHeight, text, buttonAction, gameObject pointer
	// Updated to allow for the passing of a gameObject reference but is not necessary
	Button(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Button&)> act, const string& txt, GameObject* obj = nullptr)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), buttonAction(act), clicked(false), gameObject(obj), 
		textureNum(0), sprite(false) {
	}

	bool isInside(int mouseX, int mouseY);
	// Method to handle button click
	virtual void handleClick();
	// Method to draw the button
	virtual void draw();
	void drawAssetButton(GLuint texID[]);
	// Static method to create and add a new button to a specified list
	friend static void addToList(list<Button*>& buttonList, float x, float y, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Button&)> action, const string& text, GameObject* obj) {

		// Create a new button instance and add it to the list
		buttonList.push_back(new Button(x, y, w, h, winX, winY, winW, winH, action, text, obj));
	}
};

class Toggle : public Button
{
public:
	bool toggled; // Indicates whether the button has been clicked
	function<void(Toggle&)> toggle; // Action handler for the button

	Toggle(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Toggle&)> act, const string& txt, GameObject* obj = nullptr)
		: Button(posX, posY, w, h, winX, winY, winW, winH, NULL, txt, obj),
		toggle(act), toggled(false){}
	void handleClick() override;
	void draw() override;
};

#endif