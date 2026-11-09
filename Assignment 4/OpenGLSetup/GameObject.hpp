#pragma once
// Samuel Markus
// 2/5/2025
// GameObject parent class

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP
/// Includes
#include <GL/glut.h>

class GameObject
{
public:
	// object state
	GLfloat   x, y, z, sizeX, sizeY;
	GLfloat   colorR, colorG, colorB;
	GLfloat   mass;
	GLfloat   sX, sY, sZ;
	bool	  canSee;
	bool      isSolid;
	bool      destroyed;
	bool      gravity;
	bool      lt;
	bool      rt;
	bool      onGround;

	// Constructor
	GameObject();

	// Methods
	void DrawGameObject(bool,
		GLuint textID[] = new GLuint[0], int frame = 0, GLfloat texRepeat = 1.0f);
	void DrawPlayer(bool,
		GLuint textID[] = new GLuint[0], int frame = 0);

	// Setters
	void setPosition(GLfloat x1, GLfloat y1, GLfloat z1)
	{
		x = x1; y = y1; z = z1;
	}
	void setSize(GLfloat x, GLfloat y)
	{
		sizeX = x; sizeY = y;
	}
	void setColor(GLfloat r, GLfloat g, GLfloat b)
	{
		colorR = r; colorG = g; colorB = b;
	};
	void ToggleDirections() { lt = !lt; rt = !rt; }

	void setSavePos()
	{
		sX = x;
		sY = y;
		sZ = z;
	}

	void restorePos()
	{
		x = sX;
		y = sY;
		z = sZ;
	}

	// Friend functions
	friend bool CheckCollision(GameObject& one, GameObject& two);
};


#endif