// Samuel Markus
// 4/29/2025
// Game Engine - Assignment 4

//
//// Includes ////
//
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <IrrKlang/irrKlang.h>
#include "GameObject.hpp"
#include "Button.hpp"
#include "TextInput.hpp"
using namespace irrklang;
using std::string; using std::list; using std::vector;
using std::cout; using std::endl;
using std::ofstream; using std::ios;
using std::cerr; using std::ifstream;

//
//// Global Variables and Definitions ////
//
/// Definitions
// Window size
#define MAIN_PANEL_W 600 // Width of the main panel
#define SIDE_PANEL_W 200 // Width of each side panel
#define MIDDLE_PANEL_H 600 // Height of middle panels
#define BOTTOM_PANEL_H 200 // Height of the bottom panel
#define TOP_PANEL_H 50 // Height of top panel
#define WIN_W 1000 // Adjusted window width
#define WIN_H 850 // Adjusted window height

#define MAX_TIME 500.0f

// Frames
int frame = 0;
const int walkFrames = 7;

// Object Counts
const int groundNum = 3;
const int collectibleNum = 1;

// Player Variables
float speed = 0.15, gravity = 0.2;
float jumpTimer = 10, resetJumpTimer;
float jumpAcceleration = 1.0f; // Adjust this value for jump height
float jumpVelocity = 0.5f; // Adjust this value for initial jump velocity
bool showCollision = false;
bool lt, rt, jump, contact, onGround, gun;
float bulletDelay = 0.5f, resetDelay = bulletDelay;
float bulletSpeed = 0.5f, bulletDistance = 5.0f;

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

// Textures
const int numOfFrames = 13;
GLuint texID[numOfFrames]; // Texture ID's for the four textures.
const char* textureFileNames[numOfFrames] {
   (char*)"sprite/frame0.png",
   (char*)"sprite/frame1.png",
   (char*)"sprite/frame2.png",
   (char*)"sprite/frame3.png",
   (char*)"sprite/frame4.png",
   (char*)"sprite/frame5.png",
   (char*)"sprite/frame6.png",
   (char*)"sprite/jumpPower.png", 
   (char*)"sprite/MarioFlag.png", 
   (char*)"sprite/coin.png",
   (char*)"sprite/ground.png", 
   (char*)"sprite/breakable.png",
   (char*)"sprite/bullet.png"
};	// File names for the files from which texture images are loaded

// World State
bool showTitle = true;
bool gameLoop = true;
bool gameFinish;
bool gameOver;
bool editMode = true;
bool togglePause;
bool toggleAxis = true;
const int AXIS_SIZE = 1000;
float gameTimer = MAX_TIME;
float powerTimer = 10.0f, resetPower;
bool powerUpActive;
int score = 0;
float enemyActDist = 30, enemySpeed = 0.1f;

// Camera
float cameraX = 0, cameraY = 0, scrollY = 0, scrollPos = 0;

// Gameobjects on the screen
GameObject player, bottomCheck, leftCheck, rightCheck, topCheck;
list<GameObject> platforms, collectibles, powerUps, finishes, breakables;
vector<GameObject> bullets;

// Buttons in panels
list<Button*> leftPanelButtons;
list<Button*> rightPanelButtons;
list<Button> bottomPanelButtons;
list<Button> topPanelButtons;
list<Button> fileDropDownButtons, helpDropDownButtons;

// Text input variable
TextInput textInputX(-5.8, 5.4, 4.5, 0.4, 850, 109, 66, 14, "", 0);
TextInput textInputY(1.2, 5.4, 4.5, 0.4, 950, 109, 66, 14, "", 1);
TextInput textInputW(-5.8, 4.4, 4.5, 0.4, 850, 149, 66, 14, "", 2);
TextInput textInputH(1.2, 4.4, 4.5, 0.4, 950, 149, 66, 14, "", 3);
TextInput* textInputs[] = { &textInputX, &textInputY, &textInputW, &textInputH};

bool dropDown; // Toggle the drop down menu
bool fileDropDown, helpDropDown;
bool buttonClicked = false; // Check if drop down button was clicked so button behind doesn't click


//
//// Button Functions ////
//

void toggleGravity(Toggle &button) {
	if (button.gameObject != nullptr)
		button.gameObject->gravity = button.toggled;
}
Toggle gravityToggle(0, 3.6, 1.5, 0.5, 900, 185, 20, 20, toggleGravity, "");

void toggleCollision(Toggle& button)
{
	if (button.gameObject != nullptr)
		button.gameObject->isSolid = button.toggled;
}
Toggle collisionToggle(0, 3.0, 1.5, 0.5, 900, 215, 20, 20, toggleCollision, "");

// Give buttons functions by making functions and 
// assiging them when the button is made.
void hierarchyButton(Button& button) {
	if (button.gameObject != nullptr) {
		// Update textInput with gameObject's x-coordinate
		textInputX.text = std::to_string(button.gameObject->x);
		textInputX.gameObject = button.gameObject;

		// Update textInput with gameObject's y-coordinate
		textInputY.text = std::to_string(button.gameObject->y);
		textInputY.gameObject = button.gameObject;

		// Update textInput with gameObject's width scale
		textInputW.text = std::to_string(button.gameObject->sizeX);
		textInputW.gameObject = button.gameObject;

		// Update textInput with gameObject's height scale
		textInputH.text = std::to_string(button.gameObject->sizeY);
		textInputH.gameObject = button.gameObject;

		gravityToggle.toggled = button.gameObject->gravity;
		gravityToggle.gameObject = button.gameObject;

		collisionToggle.toggled = button.gameObject->isSolid;
		collisionToggle.gameObject = button.gameObject;

	}
	else {
		cout << "Button's gameObject is null" << endl;
	}

	cout << "Completed button action" << endl;
}

// Adds a ground asset to the game
void buttonAddGround(Button& button) {
	// Define actions for buttons
	GameObject ground;
	ground.colorG = 0;
	ground.isSolid = true;
	platforms.emplace_back(ground);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
		100, leftPanelButtons.back()->windowY + 30, 200, 20,
		hierarchyButton, "Ground " + std::to_string(leftPanelButtons.size()), &platforms.back());

	printf("Completed action add ground \n");
}

// Adds a collectible asset to the game
void buttonAddCollect(Button& button) {
	// Define actions for buttons
	GameObject collectible;
	collectible.colorB = 0;
	collectible.sizeX = 0.5;
	collectible.sizeY = 0.5;
	collectible.isSolid = true;
	collectibles.emplace_back(collectible);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
		100, leftPanelButtons.back()->windowY + 30, 200, 20,
		hierarchyButton, "Collectible " + std::to_string(leftPanelButtons.size()), &collectibles.back());

	printf("Completed action add collectible \n");
}

// Adds a power up asset to the game
void buttonAddPower(Button& button) {
	// Define actions for buttons
	GameObject power;
	power.setColor(0, 1, 1);
	power.isSolid = true;
	powerUps.emplace_back(power);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
		100, leftPanelButtons.back()->windowY + 30, 200, 20,
		hierarchyButton, "Power Up " + std::to_string(leftPanelButtons.size()), &powerUps.back());

	printf("Completed action add power up \n");
}

// Adds a finish asset to the game
void buttonAddFinish(Button& button) {
	// Define actions for buttons
	GameObject finish;
	finish.isSolid = true;
	finishes.emplace_back(finish);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
		100, leftPanelButtons.back()->windowY + 30, 200, 20,
		hierarchyButton, "Finish " + std::to_string(leftPanelButtons.size()), &finishes.back());

	printf("Completed action add finish \n");
}

// Adds a finish asset to the game
void buttonAddBreak(Button& button) {
	// Define actions for buttons
	GameObject breakable;
	breakable.isSolid = true;
	breakables.emplace_back(breakable);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
		100, leftPanelButtons.back()->windowY + 30, 200, 20,
		hierarchyButton, "Breakable " + std::to_string(leftPanelButtons.size()), &breakables.back());

	printf("Completed action add finish \n");
}

void fileButtonAction(Button& button) {
	// Define actions for buttons
	dropDown = true;
	fileDropDown = true;
	helpDropDown = false;
	printf("Completed file button action \n");
}

void helpButtonAction(Button& button) {
	// Define actions for buttons
	dropDown = true;
	fileDropDown = false;
	helpDropDown = true;
	printf("Completed help button action \n");
}

void buttonAction4(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	exit(0);
	printf("Completed exiting action \n");
}

void printAbout();
void aboutAction(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printAbout();
}

void printInstructions(); 
void helpAction(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printInstructions();
}

void printContact();
void contactAction(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printContact();
}

// Save game objects that you've added to the main window in a list you've created.
void saveGameObjects(list<GameObject>& gameObjects, const string& filename) {
	string fullName = "save/" + filename + ".txt"; // File path relative to current directory
	ofstream outputFile(fullName, ios::out | ios::trunc); // Open file in write mode, truncate if exists
	if (!outputFile.is_open()) {
		cout << "Error: Unable to open file for writing: " << fullName << endl;
		return;
	}

	// Write the number of game objects
	int numGameObjects = gameObjects.size(); // Assuming platforms store all game objects
	outputFile << numGameObjects << endl;

	// Write game object data
	for (const auto& gameObject : gameObjects) {
		// Write each game object's data to the file
		outputFile << gameObject.x << " " << gameObject.y << " " << gameObject.z << " "
			<< gameObject.sizeX << " " << gameObject.sizeY << " "
			<< gameObject.colorR << " " << gameObject.colorG << " " << gameObject.colorB << " "
			<< gameObject.mass << " " << gameObject.canSee << " " << gameObject.isSolid << " "
			<< gameObject.destroyed << " " << gameObject.gravity << endl;
	}

	outputFile.close();
}

// Load game objects to the main window to a list you've created.
void loadGameObjects(list<GameObject>& gameObjects, const string& filename, string objName) {
	string fullName = "save/" + filename + ".txt"; // File path relative to current directory
	ifstream inputFile(fullName);

	if (!inputFile.is_open()) {
		cerr << "Failed to open file for reading: " << fullName << endl;
		return;
	}

	// Clear the existing game objects list
	gameObjects.clear();


	int numGameObjects;
	inputFile >> numGameObjects;

	// Load each game object from the file
	for (int i = 0; i < numGameObjects; ++i) {
		GameObject gameObject;
		inputFile >> gameObject.x >> gameObject.y >> gameObject.z
			>> gameObject.sizeX >> gameObject.sizeY
			>> gameObject.colorR >> gameObject.colorG >> gameObject.colorB
			>> gameObject.mass >> gameObject.canSee
			>> gameObject.isSolid >> gameObject.destroyed
			>> gameObject.gravity;
		gameObjects.emplace_back(gameObject);

		addToList(leftPanelButtons, 0, leftPanelButtons.back()->y - 0.7, 14, 0.5,
			100, leftPanelButtons.back()->windowY + 30, 200, 20,
			hierarchyButton, objName + " " + std::to_string(leftPanelButtons.size()), &gameObjects.back());
	}

	inputFile.close();
	cout << "Game objects loaded from file: " << fullName << endl;
}

void saveButton(Button& button) {
	// Define actions for buttons
	saveGameObjects(platforms, "GroundAssets");
	saveGameObjects(collectibles, "CollectAssets");
	saveGameObjects(powerUps, "PowerUpAssets");
	saveGameObjects(finishes, "FinishAssets");
	saveGameObjects(breakables, "BreakAssets");

	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printf("Completed save action \n");
}

void loadButton(Button& button) {
	auto it = leftPanelButtons.begin();
	it = next(it);
	while (it != leftPanelButtons.end())
	{
		it = leftPanelButtons.erase(it);
	}
	// Define actions for buttons
	loadGameObjects(platforms, "GroundAssets", "Ground");
	loadGameObjects(collectibles, "CollectAssets", "Collectible");
	loadGameObjects(powerUps, "PowerUpAssets", "Power Up");
	loadGameObjects(finishes, "FinishAssets", "Finish");
	loadGameObjects(breakables, "BreakAssets", "Breakable");


	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printf("Completed load action \n");
}

//
//// World Draw Functions ////
//
/// Display XYZ Axis on screen.
void DrawXYAxis() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(AXIS_SIZE, 0, 0); // x-axis (RED)
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, AXIS_SIZE, 0); // y-axis (GREEN)
	glEnd();
	glPopMatrix();
}

/// Draws player along with collision checks
void CreatePlayer(bool show) {
	// Draw player sprite and ground check
	glPushMatrix();
	player.setColor(0, 1, 0);
	player.DrawPlayer(true, texID, frame);
	bottomCheck.x = player.x + 0.2;
	bottomCheck.y = player.y;
	bottomCheck.sizeX = 0.6;
	bottomCheck.sizeY = 0.2;

	bottomCheck.canSee = show;

	leftCheck.x = player.x;
	leftCheck.y = player.y + 0.2;
	leftCheck.sizeX = 0.2;
	leftCheck.sizeY = 0.6;

	leftCheck.canSee = show;

	rightCheck.x = player.x + 0.8;
	rightCheck.y = player.y + 0.2;
	rightCheck.sizeX = 0.2;
	rightCheck.sizeY = 0.6;

	rightCheck.canSee = show;

	topCheck.x = player.x + 0.2;
	topCheck.y = player.y + 0.8;
	topCheck.sizeX = 0.6;
	topCheck.sizeY = 0.2;

	topCheck.canSee = show;

	bottomCheck.DrawGameObject(false);
	leftCheck.DrawGameObject(false);
	rightCheck.DrawGameObject(false);
	topCheck.DrawGameObject(false);
	glPopMatrix();
}

// Draws a square and you can determine the size, position, and color.
void drawSquare(float sizeX, float sizeY, float sizeZ, float x, float y, float z, float r, float g, float b) {
	glPushMatrix();

	glScalef(sizeX, sizeY, sizeZ);
	glTranslatef(x, y, z);
	glColor3f(r, g, b); // Light gray background
	glBegin(GL_POLYGON);
	glVertex3f(-0.5, -0.5, 0);
	glVertex3f(0.5, -0.5, 0);
	glVertex3f(0.5, 0.5, 0);
	glVertex3f(-0.5, 0.5, 0);
	glEnd();

	glPopMatrix();
}

// Functions to draw text
void drawText(string text, float x, float y, float z, float lineSpace) { //im doing this becasue "\n" doesnt work for some reason?

	glRasterPos3f(x, y, z);

	// loop through all chars
	for (int i = 0; text[i] != '\0'; i++) {
		if (text[i] == '\n') {
			// move down to the next line when "\n" is detected
			y -= lineSpace; //line spacing
			glRasterPos3f(x, y, z);
		}
		else {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
			/*
			There is only a set number of options for text types with this approach.
			This is the simplest approach to adding text to OpenGL.

			These are all the font options you have:
			GLUT_BITMAP_8_BY_13
			GLUT_BITMAP_9_BY_15
			GLUT_BITMAP_TIMES_ROMAN_10
			GLUT_BITMAP_TIMES_ROMAN_24
			GLUT_BITMAP_HELVETICA_10
			GLUT_BITMAP_HELVETICA_12
			GLUT_BITMAP_HELVETICA_18

			I've included and linked the files for the freetype library
			and added fonts as well. You should only need to add the
			necessary includes for it to use it but it is on the more
			adanced side, we will only focus on the basics in class.
			*/
		}
	}
}

// Create bullet object
void makeBullet()
{
	GameObject bullet;
	bullet.isSolid = true;
	bullet.setSize(0.5, 0.5);
	if (player.rt)
	{
		bullet.setPosition(player.x + player.sizeX, player.y + (player.sizeY / 3), 0);
	}
	if (player.lt)
	{
		bullet.setPosition(player.x, player.y + (player.sizeY / 3), 0);
	}
	bullet.rt = player.rt;
	bullet.lt = player.lt;
	bullets.push_back(bullet);
}

void drawTextWithBG(string text, float x, float y, float width, float height, bool showBG) {
	// Calculate the position to center the text with background
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the text background using the calculated center position
	if (showBG) { // Turn background on or off
		glColor3f(0.75f, 0.75f, 0.75f); // Light gray color for the background
		// You can add more to change the color
		glBegin(GL_QUADS);
		glVertex3f(centerX, centerY, 0);
		glVertex3f(centerX + width, centerY, 0);
		glVertex3f(centerX + width, centerY + height, 0);
		glVertex3f(centerX, centerY + height, 0);
		glEnd();
	}

	// Adjust horizontal position to center the text horizontally within the backgound
	float textX = centerX + 0.1;

	// Adjust vertical position to center the text vertically within the background
	float textY = centerY + (height - 0.2) / 2;

	// Draw the button text
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
	glRasterPos3f(textX, textY, 1);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	}
}

// Check Gravity
void gravityCheck() {
	onGround = false;

	for (auto& ground : platforms) {
		if (CheckCollision(bottomCheck, ground) && player.isSolid) {
			onGround = true;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(leftCheck, ground) && player.isSolid) {
			player.x += speed;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(rightCheck, ground) && player.isSolid) {
			player.x -= speed;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(topCheck, ground) && jump && player.isSolid) {
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	for (auto& breakable : breakables)
	{
		if (CheckCollision(bottomCheck, breakable) 
				&& player.isSolid && !breakable.destroyed) {
			onGround = true;
			break;
		}
	}

	for (auto& breakable : breakables) {
		if (CheckCollision(leftCheck, breakable) && 
				player.isSolid && !breakable.destroyed) {
			player.x += speed;
			break;
		}
	}

	for (auto& breakable : breakables) {
		if (CheckCollision(rightCheck, breakable) 
				&& player.isSolid && !breakable.destroyed) {
			player.x -= speed;
			break;
		}
	}

	for (auto& breakable : breakables) {
		if (CheckCollision(topCheck, breakable) && jump 
				&& player.isSolid && !breakable.destroyed) {
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	if (!onGround && player.gravity && !togglePause) {
		// Apply gravity
		player.y -= gravity;
	}
}

// Restart game to original state
void restartGame()
{
	player.setPosition(0, 0, 0);
	player.destroyed = false;
	for (auto c = collectibles.begin(); c != collectibles.end(); c++)
	{
		c->destroyed = false;
	}
	for (auto b = breakables.begin(); b != breakables.end(); b++)
	{
		b->destroyed = false;
	}

	if (editMode)
	{
		player.isSolid = false;
		player.gravity = false;
		gameLoop = true;
	}
	else
	{
		player.isSolid = true;
		player.gravity = true;
		gameLoop = false;
		if (!gameOver || !gameFinish) showTitle = true;
	}
	cameraX = 0;
	cameraY = 0;

	powerUpActive = false;
	jumpVelocity = 0.5f;
	jump = false;

	SoundEngine->stopAllSounds();
	SoundEngine->play2D("audio/anchovyjazz.mp3", true);
}

//
//// Display Function ////
//

// Initialization function
void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the window screen and change the background color
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, -7.0, 7.0, -10.0, 10.0);

	resetJumpTimer = jumpTimer;

	player.rt = true;
	bottomCheck.colorR = 0;
	bottomCheck.isSolid = true;
	leftCheck.colorR = 0;
	leftCheck.isSolid = true;
	rightCheck.colorR = 0;
	rightCheck.isSolid = true;
	topCheck.colorR = 0;
	topCheck.isSolid = true;

	SoundEngine->play2D("audio/anchovyjazz.mp3", true);

	/// BUTTON FUNCTIONS
	// Create buttons and add them to the list
	// Add hierarchy
	Button* button1 = new Button (0, 6, 14, 0.5, 100, 92, 200, 20, hierarchyButton, "Player", &player);
	leftPanelButtons.push_back(button1);

	// Add toggles
	Button* grav = &gravityToggle;
	Button* collision = &collisionToggle;

	rightPanelButtons.push_back(grav);
	rightPanelButtons.push_back(collision);

	// Bottom panel buttons
	Button groundButton(-6, -1, 1, 8, 70, 763, 70, 114, buttonAddGround, "  Ground"); 
	groundButton.sprite = true;
	groundButton.textureNum = numOfFrames - 3;
	bottomPanelButtons.push_back(groundButton);

	Button coinButton(-4.5, -1, 1, 8, 190, 763, 70, 114, buttonAddCollect, "  Coin");
	coinButton.sprite = true;
	coinButton.textureNum = numOfFrames - 4;
	bottomPanelButtons.push_back(coinButton);

	Button powerButton(-3.0, -1, 1, 8, 270, 763, 70, 114, buttonAddPower, "  Power");
	powerButton.sprite = true;
	powerButton.textureNum = numOfFrames - 6;
	bottomPanelButtons.push_back(powerButton);

	Button finishButton(-1.5, -1, 1, 8, 380, 763, 70, 114, buttonAddFinish, "  Finish");
	finishButton.sprite = true;
	finishButton.textureNum = numOfFrames - 5;
	bottomPanelButtons.push_back(finishButton);

	Button enemyButton(0, -1, 1, 8, 500, 763, 70, 114, buttonAddBreak, "  Breakable");
	enemyButton.sprite = true;
	enemyButton.textureNum = numOfFrames - 2;
	bottomPanelButtons.push_back(enemyButton);

	// Top panel buttons
	Button button3(-6.5, 0, 1, 20, 33, 25, 66, 50, fileButtonAction, "    File");
	topPanelButtons.push_back(button3);

	Button button4(-5.5, 0, 1, 20, 107, 25, 66, 50, helpButtonAction, "    Help");
	topPanelButtons.push_back(button4);

	// Save Button
	Button fileButton1(-5.6, 5.95, 2, 0.5, 100, 64, 142, 26, saveButton, "             Save");
	fileDropDownButtons.push_back(fileButton1);

	// Load Button
	Button fileButton2(-5.6, fileButton1.y - 0.5, 2, 0.5, fileButton1.windowX, fileButton1.windowY + 30, 142, 26, loadButton, "             Load");
	fileDropDownButtons.push_back(fileButton2);

	Button fileButton3(-5.6, fileButton2.y - 0.5, 2, 0.5, fileButton2.windowX, fileButton2.windowY + 30, 142, 26, buttonAction4, "             Exit");
	fileDropDownButtons.push_back(fileButton3);

	Button helpButton1(-4.6, 5.95, 2, 0.5, 173, 64, 142, 26, aboutAction, "           About");
	helpDropDownButtons.push_back(helpButton1);

	Button helpButton2(-4.6, helpButton1.y - 0.5, 2, 0.5, helpButton1.windowX, helpButton1.windowY + 30, 142, 26, helpAction, "           Controls");
	helpDropDownButtons.push_back(helpButton2);

	Button helpButton3(-4.6, helpButton2.y - 0.5, 2, 0.5, helpButton2.windowX, helpButton2.windowY + 30, 142, 26, contactAction, "           Contact");
	helpDropDownButtons.push_back(helpButton3);
}

void drawLeftPanel() {

	glPushMatrix();
	glViewport(0, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Make sure to set up the camera after the viewport but within the push and pop
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set up the camera position and orientation
	gluLookAt(0, scrollY, 0.1, // Camera position
		0, scrollY, 0.0f,    // Look-at point (looking at the center of the scene)
		0.0f, 1.0f, 0.0f);          // Up vector

	// Dark Grey background
	drawSquare(15, 14 + leftPanelButtons.size(), 1, 0, 0, 0, 0.5, 0.5, 0.5);

	drawTextWithBG("                 Hierarchy",
		0, 6.6, 14, 0.8, true);

	// Draw the buttons
	for (auto& button : leftPanelButtons) {
		button->draw();
	}

	glPopMatrix();
}

void drawRightPanel() {
	glPushMatrix();
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Green background
	drawSquare(15, 14, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	// Draw text box to label panel
	drawTextWithBG("                 Inspector",
		0, 6.6, 14, 0.8, true);

	// Draw text box for x coordinate label
	drawTextWithBG("X:",
		-4, 5.6, 6, 0.5, true);

	// Draw text box for y coordinate label
	drawTextWithBG("Y:",
		3, 5.6, 6, 0.5, true);

	// Draw text box for width coordinate label
	drawTextWithBG("W:",
		-4, 4.6, 6, 0.5, true);

	// Draw text box for height coordinate label
	drawTextWithBG("H:",
		3, 4.6, 6, 0.5, true);

	// Draw the text box
	for (TextInput* t : textInputs)
		t->draw();

	// Draw text box for show player colliders toggle
	drawTextWithBG("Toggle Gravity:",
		-4, 3.6, 6, 0.5, true);

	// Draw text box for show player colliders toggle
	drawTextWithBG("Collision:",
		-4, 3, 6, 0.5, true);

	// Draw the buttons
	for (auto& button : rightPanelButtons)
		button->draw();

	glPopMatrix();
}

void drawTopPanel() {
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

	//glScalef(1, 20, 1);

	// Blue background 
	drawSquare(20, 20, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	// Draw the buttons
	for (auto& button : topPanelButtons) {
		button.draw();
	}

	glPopMatrix();
}

void drawBottomPanel() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);

	//glScalef(1, 5, 1);

	// Dark Grey background
	drawSquare(14, 15, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	drawTextWithBG("                  Assets",
		0, 5.7, 14, 2.6, true);

	// Draw the buttons
	for (auto& button : bottomPanelButtons) {
		button.drawAssetButton(texID);
	}

	glPopMatrix();
}

void drawMainPanel() {

	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Make sure to set up the camera after the viewport but within the push and pop
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set up the camera position and orientation
	gluLookAt(cameraX, cameraY, 0.1, // Camera position
		cameraX, cameraY, 0.0f,    // Look-at point (looking at the center of the scene)
		0.0f, 1.0f, 0.0f);          // Up vector

	if (showTitle && !editMode)
	{
		glColor3f(0, 0, 1);
		drawText("Penguin Game", -1.5f, 0, 0, 0.05f);
		drawText("Press 'g' to start", -1.5f, -0.5f, 0, 0.05f);
	}

	// Game Over condition: If player falls off screen
	if (player.y <= -9 || gameFinish || gameTimer <= 0 || player.destroyed)
	{
		gameLoop = false;
		glColor3f(0, 0, 1);
		if (gameFinish) drawText("You Win!", -1, 0, 0, 0.05);
		else drawText("Game Over", -1, 0, 0, 0.05);
		drawText("Press 'g' to restart", -1.0f, -0.5f, 0, 0.05f);

		// Update camera to origin
		cameraX = 0; cameraY = 0;

		if (!gameOver)
		{
			SoundEngine->stopAllSounds();
			if (gameFinish) SoundEngine->play2D("audio/vuvuzela.wav", false);
			else SoundEngine->play2D("audio/fart.mp3", false);
		}
		gameOver = true;
	}

	if (gameLoop)
	{
		// Draw axis
		if (toggleAxis) DrawXYAxis();

		// Makes the player
		CreatePlayer(showCollision);

		// Makes the ground
		for (auto p = platforms.begin(); p != platforms.end(); p++)
		{
			p->DrawGameObject(true, texID, numOfFrames - 3, 1.0f);
			if (p->gravity && !editMode) p->y -= gravity;
		}

		// Makes collectibles, use of iterators to point directly to gameobjects
		for (auto c = collectibles.begin(); c != collectibles.end(); c++)
		{
			c->DrawGameObject(true, texID, numOfFrames - 4);

			// Removes the collectible
			if (CheckCollision(player, *(c)) && !c->destroyed)
			{
				c->destroyed = true;
				SoundEngine->play2D("audio/coin.mp3", false);
				score++;
			}
			if (c->gravity && !editMode) c->y -= gravity;
		}

		// Makes power ups, use of iterators to point directly to gameobjects
		for (auto p = powerUps.begin(); p != powerUps.end(); p++)
		{
			p->DrawGameObject(true, texID, numOfFrames - 6);

			// Removes the collectible
			if (CheckCollision(player, *(p)) && !p->destroyed)
			{
				p->destroyed = true;
				jumpVelocity = 0.8f;
				powerUpActive = true;
				resetPower = powerTimer;
			}

			if (!powerUpActive)
				p->destroyed = false;
			if (p->gravity && !editMode) p->y -= gravity;
		}
		
		// Draw finish
		for (auto f = finishes.begin(); f != finishes.end(); f++)
		{
			f->DrawGameObject(true, texID, numOfFrames - 5);
			if (f->gravity && !editMode) f->y -= gravity;
			if (CheckCollision(player, *(f)))
			{
				gameFinish = true;
			}
		};

		// Makes breakables
		for (auto b = breakables.begin(); b != breakables.end(); b++)
		{
			b->DrawGameObject(true, texID, numOfFrames - 2);
		}

		// Bullet travel
		for (auto b = bullets.begin(); b != bullets.end(); b++)
		{
			if (!b->destroyed) b->DrawGameObject(true, texID, numOfFrames - 1);
			if (b->rt)
			{
				b->x += bulletSpeed;
				if ((b->x - player.x) > bulletDistance)
				{
					b->destroyed = true;
				}
			}
			if (b->lt)
			{
				b->x -= bulletSpeed;
				if (b->lt && ((b->x - player.x) < -bulletDistance))
				{
					b->destroyed = true;
				}
			}

			// Destroy when hitting a solid platform
			for (auto p = platforms.begin(); p != platforms.end(); p++)
			{
				if (CheckCollision(*b, *p))
				{
					b->destroyed = true;
					break;
				}
			}

			for (auto br = breakables.begin(); br != breakables.end(); br++)
			{
				if (CheckCollision(*b, *br))
				{
					b->destroyed = true;
					br->destroyed = true;
					SoundEngine->play2D("audio/brickbreak.wav", false);
					break;
				}
			}
		}

		// Delete destroyed bullets
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i].destroyed)
			{
				bullets.erase(bullets.begin() + i);
			}
		}

		gravityCheck();

		//Movement
		if (!togglePause)
		{
			if (lt)
			{
				player.x -= speed;
				cameraX = player.x;
				cameraY = 0;
				player.lt = lt;
				player.rt = false;
			}
			else if (rt)
			{
				player.x += speed;
				cameraX = player.x;
				cameraY = 0;
				player.rt = rt;
				player.lt = false;
			}
		}
		else // if game is paused
		{
			glColor3f(1, 1, 1);
			drawText("Paused", (cameraX - 0.5f), 0, 0, 0.05);
		}

		// Text
		if (!editMode)
		{
			// Game Timer
			glColor3f(1, 1, 1);
			string timerText = "Time: " + std::to_string(int(gameTimer));
			drawText(timerText, (cameraX - 6.5f), (cameraY + 6), 0, 0.05f);

			// Score Counter
			glColor3f(1, 1, 1);
			string scoreText = "Score: " + std::to_string(score);
			drawText(scoreText, (cameraX + 5), (cameraY + 6), 0, 0.05f);

			// Power Counter
			if (powerUpActive)
			{
				glColor3f(1, 1, 1);
				string powerText = "Power: " + std::to_string(int(resetPower));
				drawText(powerText, (cameraX - 6.5f), (cameraY + 5), 0, 0.05f);
			}
		}
		showTitle = false;
	}
	glPopMatrix();
}

// Draws a viewport over the entire window that we can draw our menu on
void drawDropDownViewport() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, WIN_H);

	if (dropDown)
	{
		// Draw the buttons for file drop down menu
		if (fileDropDown)
			for (auto& button : fileDropDownButtons) {
				button.draw();
			}

		// Draw the buttons for help drop down menu
		if (helpDropDown)
			for (auto& button : helpDropDownButtons) {
				button.draw();
			}
	}

	glPopMatrix();
}

void MyDisplay() {
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// These do follow the rendering order too. Keep that in mind.
	drawLeftPanel();
	drawMainPanel();
	drawRightPanel();
	drawTopPanel();
	drawBottomPanel();
	drawDropDownViewport();

	glFlush();
	glutSwapBuffers();
}

//
//// Inputs ////
//
/// Special Key Inputs
void specialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		lt = true;
		rt = false;
		break;
	case GLUT_KEY_RIGHT:
		lt = false;
		rt = true;
		break;
	}
	glutPostRedisplay();
}

/// Special Key Release Inputs
void specialKeyboardRelease(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		lt = false;
		break;
	case GLUT_KEY_RIGHT:
		rt = false;
		break;
	}
}

/// Normal Key Inputs 
void Keyboard(unsigned char key, int x, int y)
{
	// Handle key press events for input boxes
	for (TextInput* t : textInputs)
		t->handleKeyPress(key);

	switch (key)
	{
	case 'q':
		toggleAxis = !toggleAxis;
		break;
	case 'w': // Move camera up
		cameraY += 0.1f;
		break;
	case 'a': // Move camera left
		cameraX -= 0.1f;
		break;
	case 's': // Move camera down
		cameraY -= 0.1f; // Adjust the value to control speed
		break;
	case 'd': // Move camera right
		cameraX += 0.1f;
		break;
	case 'g': // Start game
		if (showTitle) // Start game
		{
			showTitle = false;
			gameLoop = true;
		}
		if (gameOver || gameFinish) // Restart game
		{
			restartGame();
			gameTimer = MAX_TIME;
			score = 0;
			SoundEngine->play2D("audio/anchovyjazz.mp3", true);
			gameLoop = true;
			gameOver = false;
			gameFinish = false;
		}
		break;
	case 'p': // Toggle pause
		if (gameLoop)
		{
			togglePause = !togglePause;
			SoundEngine->setAllSoundsPaused(togglePause);
		}
		break;
	case 'z':
		if (gun && !editMode && !togglePause)
		{
			makeBullet();
			SoundEngine->play2D("audio/beam_shot.wav", false);
			gun = false;
		}
		break;
	case 32: // Spacebar
		if (onGround && !jump && !togglePause) {
			SoundEngine->play2D("audio/jump.mp3", false);
			jump = true;
		}
		break;
	case 9: // toggle modes
		editMode = !editMode;
		for (auto it = breakables.begin(); it != breakables.end(); it++)
		{
			if (editMode) it->restorePos();
			else it->setSavePos();
		}
		for (auto it = collectibles.begin(); it != collectibles.end(); it++)
		{
			if (editMode) it->restorePos();
			else it->setSavePos();
		}
		for (auto it = platforms.begin(); it != platforms.end(); it++)
		{
			if (editMode) it->restorePos();
			else it->setSavePos();
		}
		for (auto it = powerUps.begin(); it != powerUps.end(); it++)
		{
			if (editMode) it->restorePos();
			else it->setSavePos();
		}
		restartGame();
		gameTimer = MAX_TIME;
		score = 0;
		gameOver = false;
		gameFinish = false;
		break;

	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

/// Mouse input
void MouseControl(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Initialize world coordinates and clicked panel
		float worldX, worldY;
		string clickedPanel;

		// Determine which viewport was clicked
		if (x >= 0 && x < SIDE_PANEL_W) {
			// Left panel clicked
			clickedPanel = "Left Panel";
		}
		else if (x >= SIDE_PANEL_W && x < SIDE_PANEL_W + MAIN_PANEL_W) {
			// Main panel clicked
			clickedPanel = "Main Panel";
		}
		else if (x >= SIDE_PANEL_W + MAIN_PANEL_W && x < SIDE_PANEL_W + MAIN_PANEL_W * 2) {
			// Right panel clicked
			clickedPanel = "Right Panel";
		}
		else {
			// Main panel clicked
			clickedPanel = "Main Panel";
		}

		if (y >= WIN_H - BOTTOM_PANEL_H) {
			// Bottom panel clicked
			clickedPanel = "Bottom Panel";
		}
		else if (y < TOP_PANEL_H) {
			// Top panel clicked
			clickedPanel = "Top Panel";
		}

		cout << "Clicked in " << clickedPanel << " at window coordinates (" << x << ", " << y << ")" << endl;

		// Check if any button was clicked
		// Checks if drop down menu is up
		if (dropDown)
		{
			buttonClicked = false;

			// Check if any button was clicked
			for (auto& button : fileDropDownButtons) {
				if (button.isInside(x, y) && fileDropDown) {
					buttonClicked = true;
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			for (auto& button : helpDropDownButtons) {
				if (button.isInside(x, y) && helpDropDown) {
					buttonClicked = true;
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			// If no button was clicked, then enables other editor buttons
			if (!buttonClicked) {
				dropDown = false;
				fileDropDown = false;
				helpDropDown = false;
			}
		}

		// Doesn't allow other buttons to be clicked if drop down is up.
		if (!dropDown && !buttonClicked)
		{
			for (auto& button : leftPanelButtons) {
				if (button->isInside(x, y)) {
					button->handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			for (auto& button : rightPanelButtons) {
				if (button->isInside(x, y)) {
					button->handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			for (auto& button : bottomPanelButtons) {
				if (button.isInside(x, y)) {
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}
			// Check if any button was clicked
			for (auto& button : topPanelButtons) {
				if (button.isInside(x, y)) {
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}
		}
	}

	// Allows for none drop down buttons to be clickable again
	if (buttonClicked)
		buttonClicked = false;

	// Check if clicked in or out of text box to set as active or not.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (auto& t : textInputs)
		{
			if (t->isInside(x, y)) {
				t->setActive(true);
			}

			else {
				t->setActive(false);
			}
		}
	}

	float scroll = 1.0f;
	if (button == 3 && state == GLUT_UP)
	{
		scrollPos += scroll;
		if (scrollY < 0)
		{
			scrollY += scroll;
			for (auto& button : leftPanelButtons)
			{
				button->windowY += 45;
			}
		}
	}
	if (button == 4 && state == GLUT_UP)
	{
		scrollPos -= scroll;
		if (scrollY > leftPanelButtons.back()->y + 7)
		{
			scrollY -= scroll;
			for (auto& button : leftPanelButtons)
			{
				button->windowY -= 45;
			}
		}
	}


	glutPostRedisplay();
}

//
//// Textures and Timer Functions ////
//
/// Load textures
void loadTextures() {
	int i;
	glGenTextures(numOfFrames, texID); // Get the texture object IDs.
	for (i = 0; i < numOfFrames; i++) {
		// Load image with FreeImage
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], PNG_DEFAULT); // Load PNG files
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap); // Convert to 32-bit (with alpha channel)
		FreeImage_Unload(bitmap);
		if (bitmap32) {
			BYTE* bits = FreeImage_GetBits(bitmap32);
			int width = FreeImage_GetWidth(bitmap32);
			int height = FreeImage_GetHeight(bitmap32);
			for (int j = 0; j < width * height; j++) {
				BYTE temp = bits[j * 4];
				bits[j * 4] = bits[j * 4 + 2];
				bits[j * 4 + 2] = temp;
			}

			glBindTexture(GL_TEXTURE_2D, texID[i]); // Bind texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			FreeImage_Unload(bitmap32);
		}
	}
	glEnable(GL_BLEND); // Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function
}

/// Timer function
void timer(int v)
{
	if (!togglePause) frame++;

	if (frame >= walkFrames) {
		frame = 0;
	}

	if (gameLoop) gameTimer -= 0.1f;

	if (powerUpActive)
	{
		if (resetPower > 0)
		{
			resetPower -= 0.1f;
		}
		else
		{
			powerUpActive = false;
			jumpVelocity = 0.5f;
		}
	}

	// jump timer
	if (jump && !togglePause) {
		if (jumpTimer > 0) {
			float jumpAccelerationTemp = jumpAcceleration;
			float jumpVelocityTemp = jumpVelocity;
			player.y += jumpVelocityTemp; // Increment player's y-coordinate based on velocity
			jumpVelocityTemp -= jumpAccelerationTemp; // Apply gravity to decrease velocity
			jumpTimer--;
		}
		else {
			jump = false;
			jumpTimer = resetJumpTimer;
		}
	}

	// bullet delay
	if (!gun && !togglePause)
	{
		if (resetDelay > 0)
		{
			resetDelay--;
		}
		else
		{
			gun = true;
			resetDelay = bulletDelay;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Adjust frame delay based on FPS
}


//
//// Console Prints ////
//
void printInstructions()
{
	std::cout << "Program Controls: \n////////////////////// " << std::endl;
	std::cout << "\nBasic Controls:" << std::endl;
	std::cout << "\'Arrow Keys\': Move sprite by direction." << std::endl;
	std::cout << "\'Space Bar\': Jump." << std::endl;
	std::cout << "\'Scroll Wheel\': Move left panel up or down when number of objects goes off view." << std::endl;
	std::cout << "\'z\': Fire weapon. Use it to break bricks." << std::endl;
	std::cout << "\'g\': Start game on title screen." << std::endl;
	std::cout << "\'ESC\': Exit program." << std::endl;
	std::cout << "\nCamera:" << std::endl;
	std::cout << "\'w keys\': Move camera up." << std::endl;
	std::cout << "\'a keys\': Move camera left." << std::endl;
	std::cout << "\'s keys\': Move camera down." << std::endl;
	std::cout << "\'d keys\': Move camera right." << std::endl;
	std::cout << "\nToggles:" << std::endl;
	std::cout << "\'q\': Toggle XY axis display." << std::endl;
	std::cout << "\'p\': Pause game." << std::endl;
	std::cout << "\'Tab\': Toggle edit and playmode." << std::endl;
	std::cout << "\n//////////////////////\n " << std::endl;
}

void printAbout()
{
	std::cout << "//////////////////////\n" << std::endl;
	cout << "Game engine project by Samuel Markus" << endl;
	cout << "Create a platformer game with a penguin" << endl;
	std::cout << "\n//////////////////////\n " << std::endl;
}

void printContact()
{
	std::cout << "//////////////////////\n " << std::endl;
	cout << "For contact support: email smarkus1@kent.edu" << endl;
	cout << "We will reach you in 1-2 business weeks." << endl;
	std::cout << "\n//////////////////////\n " << std::endl;
}

//
//// Main Function ////
//
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutCreateWindow("(Samuel Markus) (#811194109)");

	glutTimerFunc(0, timer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardRelease);
	glutMouseFunc(MouseControl);

	glutMainLoop();
	return 0;
}
