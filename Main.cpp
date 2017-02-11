/*
 * Programmer: Jason Carruthers
 * Last modified: 2/9/16
 *
 * This program implements a z-buffer, though it is modified by my own design as a
 * combination of the z-buffer and a-buffer algorithms found in the book, as I
 * have given triangles (and potentially any polygon) the ability to have
 * transparency.
 * NOTE: This program assumes orthographic projection.
 */
#include <GL/glut.h>
#include <math.h>
#include <list>
#include <vector>
#include <ctime>
#include <map>



/*
* Classes
*/
class Color3
{
public:
	enum RGBParameters
	{
		Red,
		Green,
		Blue,
		Num__RGBParameters,
	};
public:
	//Constructors
	Color3(float red = 1.0f, float green = 1.0f, float blue = 1.0f)
	{
		SetR(red);
		SetG(green);
		SetB(blue);
	}

	//Accessors
	float GetR() const
	{
		return mRGB[(int)Red];
	}
	float GetG() const
	{
		return mRGB[(int)Green];
	}
	float GetB() const
	{
		return mRGB[(int)Blue];
	}

	//Mutators
	void Set(const Color3 &color)
	{
		SetR(color.GetR());
		SetG(color.GetG());
		SetB(color.GetB());
	}
	void Set(float newRed, float newGreen, float newBlue)
	{
		SetR(newRed);
		SetG(newGreen);
		SetB(newBlue);
	}
	void SetR(float newRed)
	{
		newRed = (newRed < 0.0f) ? 0.0f : newRed;
		newRed = (newRed > 1.0f) ? 1.0f : newRed;
		mRGB[(int)Red] = newRed;
	}
	void SetG(float newGreen)
	{
		newGreen = (newGreen < 0.0f) ? 0.0f : newGreen;
		newGreen = (newGreen > 1.0f) ? 1.0f : newGreen;
		mRGB[(int)Green] = newGreen;
	}
	void SetB(float newBlue)
	{
		newBlue = (newBlue < 0.0f) ? 0.0f : newBlue;
		newBlue = (newBlue > 1.0f) ? 1.0f : newBlue;
		mRGB[(int)Blue] = newBlue;
	}

	//Overloaded operators
	float &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__RGBParameters)
			return mRGB[(int)Red]; //If index out of bounds, return the red value by default.
		return mRGB[index];
	}
	const float &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__RGBParameters)
			return mRGB[(int)Red]; //If index is out of bounds, return the first element by default.
		return mRGB[index];
	}

private:
	float mRGB[Num__RGBParameters];
};


//Color4 is like Color3 but has an alpha value
class Color4
{
public:
	enum RGBAParameters
	{
		Red,
		Green,
		Blue,
		Alpha,
		Num__RGBAParameters,
	};
public:
	//Constructors
	Color4(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
	{
		SetR(red);
		SetG(green);
		SetB(blue);
		SetA(alpha);
	}

	//Accessors
	float GetR() const
	{
		return mRGBA[(int)Red];
	}
	float GetG() const
	{
		return mRGBA[(int)Green];
	}
	float GetB() const
	{
		return mRGBA[(int)Blue];
	}
	float GetA() const
	{
		return mRGBA[(int)Alpha];
	}
	Color3 GetColor3() const
	{
		return Color3(mRGBA[(int)Red], mRGBA[(int)Green], mRGBA[(int)Blue]);
	}

	//Mutators
	void Set(const Color4 &color)
	{
		SetR(color.GetR());
		SetG(color.GetG());
		SetB(color.GetB());
		SetA(color.GetA());
	}
	void Set(float newRed, float newGreen, float newBlue, float newAlpha)
	{
		SetR(newRed);
		SetG(newGreen);
		SetB(newBlue);
		SetA(newAlpha);
	}
	void SetR(float newRed)
	{
		newRed = (newRed < 0.0f) ? 0.0f : newRed;
		newRed = (newRed > 1.0f) ? 1.0f : newRed;
		mRGBA[(int)Red] = newRed;
	}
	void SetG(float newGreen)
	{
		newGreen = (newGreen < 0.0f) ? 0.0f : newGreen;
		newGreen = (newGreen > 1.0f) ? 1.0f : newGreen;
		mRGBA[(int)Green] = newGreen;
	}
	void SetB(float newBlue)
	{
		newBlue = (newBlue < 0.0f) ? 0.0f : newBlue;
		newBlue = (newBlue > 1.0f) ? 1.0f : newBlue;
		mRGBA[(int)Blue] = newBlue;
	}
	void SetA(float newAlpha)
	{
		newAlpha = (newAlpha < 0.0f) ? 0.0f : newAlpha;
		newAlpha = (newAlpha > 1.0f) ? 1.0f : newAlpha;
		mRGBA[(int)Alpha] = newAlpha;
	}

	//Overloaded operators
	float &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__RGBAParameters)
			return mRGBA[(int)Red]; //If index out of bounds, return the red value by default.
		return mRGBA[index];
	}
	const float &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__RGBAParameters)
			return mRGBA[(int)Red]; //If index is out of bounds, return the first element by default.
		return mRGBA[index];
	}

private:
	float mRGBA[Num__RGBAParameters];
};

class Vector2I
{
public:
	enum VectorElements
	{
		X,
		Y,
		Num__Elements,
	};
public:
	//Constructors
	Vector2I(int x = 0, int y = 0)
	{
		mArr[X] = x;
		mArr[Y] = y;
	}

	//Accessors
	int GetX() const
	{
		return mArr[X];
	}
	int GetY() const
	{
		return mArr[Y];
	}

	//Mutators
	void SetX(int newX)
	{
		mArr[X] = newX;
	}
	void SetY(int newY)
	{
		mArr[Y] = newY;
	}

	//Overloaded operators
	int &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	const int &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	friend bool operator== (Vector2I &pair1, Vector2I &pair2)
	{
		return (pair1.GetX() == pair2.GetX() && pair1.GetY() == pair2.GetY());
	}

private:
	int mArr[Num__Elements];
};


class Vector3I
{
public:
	enum VectorElements
	{
		X,
		Y,
		Z,
		Num__Elements,
	};
public:
	//Constructors
	Vector3I(int x = 0, int y = 0, int z = 0)
	{
		mArr[X] = x;
		mArr[Y] = y;
		mArr[Z] = z;
	}

	//Accessors
	int GetX() const
	{
		return mArr[X];
	}
	int GetY() const
	{
		return mArr[Y];
	}
	int GetZ() const
	{
		return mArr[Z];
	}

	//Mutators
	void SetX(int newX)
	{
		mArr[X] = newX;
	}
	void SetY(int newY)
	{
		mArr[Y] = newY;
	}
	void SetZ(int newZ)
	{
		mArr[Z] = newZ;
	}

	//Overloaded operators
	int &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	const int &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	friend bool operator== (Vector3I &coord1, Vector3I &coord2)
	{
		return (coord1.GetX() == coord2.GetX() && coord1.GetY() == coord2.GetY() && coord1.GetZ() == coord2.GetZ());
	}

private:
	int mArr[Num__Elements];
};


class Vector2F
{
public:
	enum VectorElements
	{
		X,
		Y,
		Num__Elements,
	};
public:
	//Constructors
	Vector2F(float x = 0, float y = 0)
	{
		mArr[X] = x;
		mArr[Y] = y;
	}

	//Accessors
	float GetX() const
	{
		return mArr[X];
	}
	float GetY() const
	{
		return mArr[Y];
	}

	//Mutators
	void SetX(float newX)
	{
		mArr[X] = newX;
	}
	void SetY(float newY)
	{
		mArr[Y] = newY;
	}

	//Overloaded operators
	float &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	const float &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	friend bool operator== (Vector2F &pair1, Vector2F &pair2)
	{
		return (pair1.GetX() == pair2.GetX() && pair1.GetY() == pair2.GetY());
	}

private:
	float mArr[Num__Elements];
};


class Vector3F
{
public:
	enum VectorElements
	{
		X,
		Y,
		Z,
		Num__Elements,
	};
public:
	//Constructors
	Vector3F(float x = 0, float y = 0, float z = 0)
	{
		mArr[X] = x;
		mArr[Y] = y;
		mArr[Z] = z;
	}

	//Accessors
	float GetX() const
	{
		return mArr[X];
	}
	float GetY() const
	{
		return mArr[Y];
	}
	float GetZ() const
	{
		return mArr[Z];
	}

	//Mutators
	void SetX(float newX)
	{
		mArr[X] = newX;
	}
	void SetY(float newY)
	{
		mArr[Y] = newY;
	}
	void SetZ(float newZ)
	{
		mArr[Z] = newZ;
	}

	//Overloaded operators
	float &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	const float &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	friend bool operator== (Vector3F &coord1, Vector3F &coord2)
	{
		return (coord1.GetX() == coord2.GetX() && coord1.GetY() == coord2.GetY() && coord1.GetZ() == coord2.GetZ());
	}

private:
	float mArr[Num__Elements];
};


/*
* Global variables that class Triangle relies on
*/
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const int Z_NEAR = 0;
const int Z_FAR = -1000;
const unsigned int SLEEP_DURATION = 1;
float *pixelBuffer;


//Class prototypes
class Triangle;

//Function prototypes that class Triangle relies on
void SetPixel(int x, int y, const Color3 &color);
void UpdateTriangleAndDepthBuffer(Triangle &triangle, const Vector3F &newRelativePosition);



/*
* Some triangles aren't displaying correctly, such as the following case(s):
*		- Drawing a triangle with one vertical edge
* There may be other cases where the triangle doesn't display properly, but so far
* the above case(s) are the only ones that have been tested.
*/
class Triangle
{
public:
	/*
	* Constructor
	*/
	Triangle(){}
	Triangle(const Color4 &newColor, const Vector3F &p1, const Vector3F &p2, const Vector3F &p3) :
		Triangle(p1, p2, p3)
	{
		color = newColor;
	}
	Triangle(const Vector3F &p1, const Vector3F &p2, const Vector3F &p3)
	{
		/*
		* Sort points by y value, from least to greatest
		*/
		vertexArr[0] = p1;
		vertexArr[1] = p2;
		vertexArr[2] = p3;
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 3; j++)
			{
				if (vertexArr[j].GetY() < vertexArr[i].GetY())
				{
					Vector3F temp = vertexArr[j];
					vertexArr[j] = vertexArr[i];
					vertexArr[i] = temp;
				}
			}
		}

		/*
		* If this triangle has a horizontal edge, ensure the two vertices with the same
		* y-value are also sorted by x-value, from least to greatest x-value.
		*/
		if (vertexArr[0].GetY() == vertexArr[1].GetY())
		{
			if (vertexArr[0].GetX() > vertexArr[1].GetX())
			{
				float tempX = vertexArr[0].GetX();
				vertexArr[0].SetX(vertexArr[1].GetX());
				vertexArr[1].SetX(tempX);
			}
		}
		if (vertexArr[1].GetY() == vertexArr[2].GetY())
		{
			if (vertexArr[1].GetX() > vertexArr[2].GetX())
			{
				float tempX = vertexArr[1].GetX();
				vertexArr[1].SetX(vertexArr[2].GetX());
				vertexArr[2].SetX(tempX);
			}
		}

		SetRelativeXPairs();
		relativePosition = Vector3F(0, 0, 0);
		//UpdatePixelInfo(relativePosition);
		UpdateTriangleAndDepthBuffer(*this, relativePosition);
		SetNormalVector();
	}

	/*
	* Accessors
	*/
public:
	Color3 GetColor3() const
	{
		return Color3(color.GetR(), color.GetG(), color.GetB());
	}

	/*
	 * Instead of drawing a triangle directly to the screen (by calling SetPixel), now all Draw()
	 * functions should update the zbuffer, and each cycle the zbuffer should draw.
	 */
	void Draw(const Color3 &color) const
	{
		for (unsigned int relativeY = 0; relativeY < relativeXPairVec.size(); relativeY++)
			for (int x = relativeXPairVec[relativeY].GetX(); x < relativeXPairVec[relativeY].GetY(); x++)
				SetPixel(x + GetBaseX(relativeY + (int)vertexArr[0].GetY()) + (int)relativePosition.GetX(),
				relativeY + (int)vertexArr[0].GetY() + (int)relativePosition.GetY(),
				color);

		/*
		* Both of these functions update the pixels onscreen, so that each time a new pixel
		* is "added to" pixelBuffer, it immediately draws onscreen. This way, the lines
		* animate as they're drawn to the screen.
		*/
		//Draws pixel on screen, width and height must match pixel buffer dimension
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
		//Window refresh
		glFlush();
	}

	int GetBaseX(int worldY) const
	{
		//If has horizontal edge...
		if (vertexArr[0].GetY() == vertexArr[1].GetY() || vertexArr[1].GetY() == vertexArr[2].GetY())
			return (int)vertexArr[0].GetX();

		//Otherwise, no horizontal edge...
		if (worldY < vertexArr[1].GetY())
			return (int)vertexArr[0].GetX();
		else //if y >= vertexArr[1].GetY()
		{
			if (vertexArr[0].GetX() > vertexArr[1].GetX())
				return (int)vertexArr[1].GetX();

			//Otherwise, this triangle is oriented with its nose in the 3rd quadrant, if
			//seen as being pivoted by its y-middle vertex. Thus, the x offset is
			//the same as the bottom left vertex x value of its horizontal-split
			//upper-triangle.
			float slope = (vertexArr[2].GetY() - vertexArr[0].GetY()) / (vertexArr[2].GetX() - vertexArr[0].GetX());
			float pairPtX = ((vertexArr[1].GetY() - vertexArr[0].GetY()) / slope) + vertexArr[0].GetX();
			return (int)pairPtX;
		}
	}

	int GetWorldZ(int worldX, int worldY) const
	{
		return (int)((-1 / normalVec[2])*(normalVec[0] * (worldX - vertexArr[0].GetX()) + normalVec[1] * (worldY - vertexArr[0].GetY())) + vertexArr[0].GetZ());
	}

	/*
	* Mutators
	*/
public:
	void SetNormalVector()
	{
		Vector3F leftEdge(vertexArr[1].GetX() - vertexArr[0].GetX(),
							vertexArr[1].GetY() - vertexArr[0].GetY(), 
							vertexArr[1].GetZ() - vertexArr[0].GetZ());

		Vector3F rightEdge(vertexArr[2].GetX() - vertexArr[0].GetX(),
							vertexArr[2].GetY() - vertexArr[0].GetY(),
							vertexArr[2].GetZ() - vertexArr[0].GetZ());

		float normalVecX = leftEdge.GetY()*rightEdge.GetZ() - leftEdge.GetZ()*rightEdge.GetY();
		float normalVecY = leftEdge.GetZ()*rightEdge.GetX() - leftEdge.GetX()*rightEdge.GetZ();
		float normalVecZ = leftEdge.GetX()*rightEdge.GetY() - leftEdge.GetY()*rightEdge.GetX();
		normalVec = Vector3F(normalVecX, normalVecY, normalVecZ);
	}

	//This function should always be called before this triangle's Draw() function is called.
	//void UpdatePosition(const Vector3F &newRelativeXYPosition)
	//{
	//	//Turn all pixels representing previous position to black
	//	//for (unsigned int pixel = 0; pixel < pixelInfoVec.size(); pixel++)
	//	//	for (int colorIndex = 0; colorIndex < (int)Color3::Num__RGBParameters; colorIndex++)
	//	//		pixelBuffer[pixelInfoVec[pixel].GetX() * (int)Color3::Num__RGBParameters + pixelInfoVec[pixel].GetY() * WINDOW_WIDTH * (int)Color3::Num__RGBParameters + colorIndex] = 0.0f; //See pgs. 146-147 to optimize this.

	//	UpdatePixelInfo(newRelativeXYPosition);
	//}

private:
	//void UpdatePixelInfo(const Vector3F &newRelativePosition)
	//{
	//	relativePosition = newRelativePosition;
	//	pixelInfoVec.clear();

	//	int worldX, worldY, worldZ;
	//	for (unsigned int scanLine = 0; scanLine < relativeXPairVec.size(); scanLine++)
	//	{
	//		for (int pixelX = (int)relativeXPairVec[scanLine].GetX(); pixelX < (int)relativeXPairVec[scanLine].GetY(); pixelX++)
	//		{
	//			worldX = GetBaseX(scanLine + (int)vertexArr[0].GetY()) + pixelX + (int)relativePosition.GetX();
	//			worldY = (int)vertexArr[0].GetY() + (int)relativePosition.GetY() + scanLine;
	//			worldZ = GetWorldZ(worldX, worldY);
	//			pixelInfoVec.push_back(Vector3I(worldX, worldY, worldZ));
	//		}
	//	}
	//}

private:
	void SetRelativeXPairs()
	{
		/*
		* This function finds pairs of points along the edges of the triangle
		* that share the same y value. Since each scan line is an integer value,
		* relative y doesn't need to be explicitly stored, as it can be determined
		* for a given relativeXPair by adding the index that the given pair is found
		* at within the relativeXPairVec to the starting scan-line value.
		*
		* The xPairs are relative because the distance between edge-points on the same
		* scan line doesn't differ, but a triangle can move around and have different
		* screen position, thus affecting the offset x-values of all of its (edge) points.
		*/

		//If no two vertices share a y-value, horizontally split this triangle about its mid-vertex.
		if (vertexArr[0].GetY() != vertexArr[1].GetY() && vertexArr[1].GetY() != vertexArr[2].GetY())
		{
			//Get point that x-pairs with the y-middle point of the triangle vertices
			float slope = (vertexArr[2].GetY() - vertexArr[0].GetY()) / (vertexArr[2].GetX() - vertexArr[0].GetX());
			float pairingPtX = ((vertexArr[1].GetY() - vertexArr[0].GetY()) / slope) + vertexArr[0].GetX();
			Vector3F pairingPt = Vector3F(pairingPtX, vertexArr[1].GetY(), vertexArr[1].GetZ());

			Triangle bottomTriangle(vertexArr[0], pairingPt, vertexArr[1]);
			Triangle topTriangle(pairingPt, vertexArr[1], vertexArr[2]);

			relativeXPairVec.insert(relativeXPairVec.begin(),
				bottomTriangle.relativeXPairVec.begin(), bottomTriangle.relativeXPairVec.end());

			//Check to see if they share the same xPair along the horizontal split, then remove it
			//if (relativeXPairVec[relativeXPairVec.size() - 1] == topTriangle.relativeXPairVec[0])
			//topTriangle.relativeXPairVec.erase(topTriangle.relativeXPairVec.begin());

			relativeXPairVec.insert(relativeXPairVec.end(),
				topTriangle.relativeXPairVec.begin(), topTriangle.relativeXPairVec.end());

			return;
		}

		/*
		* Otherwise, this triangle already has one horizontal edge.
		* First, get the slope of the left edge and the slope of the right edge.
		* Then for a given y value, get the corresponding x values from both slopes.
		* Store those two x values as a pair (Vector2I) into the relativeXPairVec.
		*/
		float slopeLeftEdge;
		float slopeRightEdge;
		if (vertexArr[0].GetY() == vertexArr[1].GetY()) //If horizontal edge is along the bottom edge
		{
			slopeLeftEdge = (vertexArr[2].GetY() - vertexArr[0].GetY()) / (vertexArr[2].GetX() - vertexArr[0].GetX());
			slopeRightEdge = (vertexArr[2].GetY() - vertexArr[1].GetY()) / (vertexArr[2].GetX() - vertexArr[1].GetX());
		}
		else //Else the horizontal edge must be along the top edge
		{
			slopeLeftEdge = (vertexArr[1].GetY() - vertexArr[0].GetY()) / (vertexArr[1].GetX() - vertexArr[0].GetX());
			slopeRightEdge = (vertexArr[2].GetY() - vertexArr[0].GetY()) / (vertexArr[2].GetX() - vertexArr[0].GetX());
		}

		float leftPtX, rightPtX;
		for (int y = (int)vertexArr[0].GetY(); y < (int)vertexArr[2].GetY(); y++)
		{
			if (vertexArr[0].GetY() == vertexArr[1].GetY()) //If horizontal edge is along the bottom edge
			{
				leftPtX = ((float)y - vertexArr[0].GetY()) / slopeLeftEdge;
				rightPtX = (((float)y - vertexArr[1].GetY()) / slopeRightEdge) + (vertexArr[1].GetX() - vertexArr[0].GetX())/*+ 0.5f*/;
			}
			else //Else the horizontal edge must be along the top edge
			{
				leftPtX = ((float)y - vertexArr[0].GetY()) / slopeLeftEdge;
				rightPtX = (((float)y - vertexArr[0].GetY()) / slopeRightEdge) /*+ 0.5f*/;
			}
			relativeXPairVec.push_back(Vector2I((int)leftPtX, (int)rightPtX));
		}
	}

//Make this private later
public:
	Color4 color;
	Vector3F vertexArr[3];
	Vector3F relativePosition;
	std::vector<Vector2I> relativeXPairVec; //stored as ints for drawing optimization
	std::vector<Vector3I> pixelInfoVec;
	Vector3F normalVec; /*
						 * A vector normal to this triangle, and thus the plane containing this
						 * triangle. Used to get the world-z coordinate given a local (x, y) point.
						 * normalVec is relative to world coordinates.
						 */
};


class DepthBuffer
{
public:
	/*
	 * Constructor
	 */
	DepthBuffer()
	{
		zBuffer = new std::vector<DepthInfo>[WINDOW_WIDTH * WINDOW_HEIGHT];
		aBuffer = new std::vector<DepthInfo>[WINDOW_WIDTH * WINDOW_HEIGHT];
		DepthInfo backgroundDepthInfo(Z_FAR, Color4(0.0f, 0.0f, 0.0f, 1.0f));
		for (int y = 0; y < WINDOW_HEIGHT; y++)
			for (int x = 0; x < WINDOW_WIDTH; x++)
			{
				zBuffer[x + y * WINDOW_WIDTH].push_back(backgroundDepthInfo);
				aBuffer[x + y * WINDOW_WIDTH].push_back(backgroundDepthInfo);
			}
	}

	/*
	 * Accessors
	 */
	Color3 GetVisibleColor3(int x, int y) const
	{
		int bufferIndex = x + y * WINDOW_WIDTH;
		return Color3(aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetR(),
						aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetG(),
						aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetB());
	}

	void Draw() const
	{
		/*
		 * The pixel that should be drawn at a given location is given by the greatest-indexed
		 * aBuffer color.
		 */
		for (int y = 0; y < WINDOW_HEIGHT; y++)
			for (int x = 0; x < WINDOW_WIDTH; x++)
				SetPixel(x, y, GetVisibleColor3(x, y));

		/*
		* Both of these functions update the pixels onscreen, so that each time a new pixel
		* is "added to" pixelBuffer, it immediately draws onscreen. This way, the lines
		* animate as they're drawn to the screen.
		*/
		//Draws pixel on screen, width and height must match pixel buffer dimension
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
		//Window refresh
		glFlush();
	}

	/*
	 * Mutators
	 */
	void MaskBuffers(const Triangle &triangleMask)
	{
		unsigned int worldX;
		unsigned int worldY;
		int worldZ;
		unsigned int bufferIndex;
		unsigned int bufferSize;
		for (unsigned int i = 0; i < triangleMask.pixelInfoVec.size(); i++)
		{
			worldX = triangleMask.pixelInfoVec[i].GetX();
			worldY = triangleMask.pixelInfoVec[i].GetY();
			worldZ = triangleMask.pixelInfoVec[i].GetZ();
			bufferIndex = worldX + worldY * WINDOW_WIDTH;
			bufferSize = zBuffer[bufferIndex].size();
			for (unsigned int zDepth = 0; zDepth < bufferSize; zDepth++)
			{
				if (zBuffer[bufferIndex][zDepth].depth == worldZ)
				{
					zBuffer[bufferIndex].erase(zBuffer[bufferIndex].begin() + zDepth);
					aBuffer[bufferIndex].erase(aBuffer[bufferIndex].begin() + zDepth);
					Color3 drawColor = aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetColor3();
					SetPixel(worldX, worldY, drawColor);
					break;
				}
			}
		}
	}


	void UpdateBuffers(int worldX, int worldY, int worldZ, const Color4 &newColor)
	{
		unsigned int bufferIndex;
		unsigned int bufferSize;
		unsigned int i = 0;

		bufferIndex = worldX + worldY * WINDOW_WIDTH;
		bufferSize = zBuffer[bufferIndex].size();

		for (i = 0; i < bufferSize; i++)
		{
			if (zBuffer[bufferIndex][i].depth == worldZ)
			{
				zBuffer[bufferIndex][i].color = newColor;
				aBuffer[bufferIndex][i].color = newColor;
				break;
			}
			else if (worldZ < zBuffer[bufferIndex][i].depth)
			{
				zBuffer[bufferIndex].insert(zBuffer[bufferIndex].begin() + i, DepthInfo(worldZ, newColor));
				aBuffer[bufferIndex].insert(aBuffer[bufferIndex].begin() + i, DepthInfo(worldZ, newColor));
				break;
			}
		}
		if (i == bufferSize)
		{
			zBuffer[bufferIndex].push_back(DepthInfo(worldZ, newColor));
			aBuffer[bufferIndex].push_back(DepthInfo(worldZ, newColor));
		}

		//if (newColor.GetA() != 1.0f)
		BlendABuffer(worldX, worldY);
		SetPixel(worldX, worldY, aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetColor3());
	}
	//void UpdateBuffers(const Triangle &triangle)
	//{
	//	unsigned int worldX;
	//	unsigned int worldY;
	//	int worldZ;
	//	unsigned int bufferIndex;
	//	unsigned int bufferSize;
	//	unsigned int i = 0;
	//	for (unsigned int pixel = 0; pixel < triangle.pixelInfoVec.size(); pixel++)
	//	{
	//		worldX = triangle.pixelInfoVec[pixel].GetX();
	//		worldY = triangle.pixelInfoVec[pixel].GetY();
	//		worldZ = triangle.pixelInfoVec[pixel].GetZ();
	//		bufferIndex = worldX + worldY * WINDOW_WIDTH;
	//		bufferSize = zBuffer[bufferIndex].size();
	//		
	//		for (i = 0; i < bufferSize; i++)
	//		{
	//			if (zBuffer[bufferIndex][i].depth == worldZ)
	//			{
	//				zBuffer[bufferIndex][i].color = triangle.color;
	//				aBuffer[bufferIndex][i].color = triangle.color;
	//				break;
	//			}
	//			else if (worldZ < zBuffer[bufferIndex][i].depth)
	//			{
	//				zBuffer[bufferIndex].insert(zBuffer[bufferIndex].begin() + i, DepthInfo(worldZ, triangle.color));
	//				aBuffer[bufferIndex].insert(aBuffer[bufferIndex].begin() + i, DepthInfo(worldZ, triangle.color));
	//				break;
	//			}
	//		}
	//		if (i == bufferSize)
	//		{
	//			zBuffer[bufferIndex].push_back(DepthInfo(worldZ, triangle.color));
	//			aBuffer[bufferIndex].push_back(DepthInfo(worldZ, triangle.color));
	//		}

	//		//if (newColor.GetA() != 1.0f)
	//		BlendABuffer(worldX, worldY);
	//		Color3 drawColor = aBuffer[bufferIndex][aBuffer[bufferIndex].size() - 1].color.GetColor3();
	//		SetPixel(worldX, worldY, drawColor);
	//	}
	//}
private:
	void BlendABuffer(int x, int y)
	{
		int bufferIndex = x + y * WINDOW_WIDTH;

		//Assume the background color is always completely opaque.
		Color3 prevColor3 = aBuffer[bufferIndex][0].color.GetColor3();

		for (unsigned int i = 1; i < aBuffer[bufferIndex].size(); i++)
		{
			//If the current pixel is completely opaque, then move onto the next color.
			if (aBuffer[bufferIndex][i].color.GetA() == 1.0f)
			{
				prevColor3 = aBuffer[bufferIndex][i].color.GetColor3();
				continue;
			}

			//Blend the current pixel color with the pixel color behind it.
			aBuffer[bufferIndex][i].color.Set((aBuffer[bufferIndex][i].color.GetR() * aBuffer[bufferIndex][i].color.GetA() + prevColor3.GetR()) / 2,
				(aBuffer[bufferIndex][i].color.GetG() * aBuffer[bufferIndex][i].color.GetA() + prevColor3.GetG()) / 2,
				(aBuffer[bufferIndex][i].color.GetB() * aBuffer[bufferIndex][i].color.GetA() + prevColor3.GetB()) / 2,
				1.0f);
		}
	}

private:
	class DepthInfo
	{
	public:
		DepthInfo(int newDepth = Z_FAR, const Color4 &newColor = Color4(0.0f, 0.0f, 0.0f, 1.0f))
		{
			depth = newDepth;
			color = newColor;
		}
	public:
		int depth;
		Color4 color;
	};
	/*
	 * Both buffers store Color info sorted from most-positive z-value to most-negative z-value.
	 * Thus, the pixel that should be drawn at a given location is given by the greatest-indexed
	 * aBuffer color.
	 */
	//std::vector<DepthInfo> zBuffer[WINDOW_WIDTH * WINDOW_HEIGHT]; //Holds unmodified Color4 pixel info of polygons in the scene.
	//std::vector<DepthInfo> aBuffer[WINDOW_WIDTH * WINDOW_HEIGHT]; //Holds blended Color4 pixel info of polygons in the scene.
	std::vector<DepthInfo> *zBuffer; //Holds unmodified Color4 pixel info of polygons in the scene.
	std::vector<DepthInfo> *aBuffer; //Holds blended Color4 pixel info of polygons in the scene.
};



/*
* Global variables
*/
DepthBuffer depthBuffer;
Triangle sun;
std::vector<Triangle> planetVec;
std::vector<Triangle> asteroidVec;
Triangle alienPlanet;
std::vector<Triangle> alienPlanetRingsVec;
float theta = 0.0f;
//Triangle testTriangle(Vector2F(0, 0), Vector2F(100, 0), Vector2F(50, 50)); //works
//Triangle testTriangle(Vector2F(0, 100), Vector2F(100, 100), Vector2F(50, 150)); //works
//Triangle testTriangle(Vector2F(0, 80), Vector2F(100, 100), Vector2F(50, 150)); //work
//Triangle testTriangle(Vector2F(0, 0), Vector2F(10, 20), Vector2F(60, 300)); //works
//Triangle testTriangle(Vector2F(0, 300), Vector2F(100, 100), Vector2F(50, 400)); //works
//Triangle testTriangle(Vector2F(0, 100), Vector2F(100, 50), Vector2F(50, 150)); //works


/*
* Function prototypes
*/
void Display();
Color4 GetRandomColor();
void SetPixel(int x, int y, const Color3 &color);
void CreateSolarSystem();
void UpdatePlanets();
void UpdateSolarSystem();
void UpdateAsteroids();
//void UpdateTriangleAndDepthBuffer(Triangle &trianlge, const Vector3F &newRelativePosition);


/*
* Functions
*/
int main(int argc, char *argv[])
{
	//Seed the random number generator
	srand(((static_cast<int>(time(0)))));

	//Allocate new pixel buffer, need initialization!!
	pixelBuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT * 3];

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//Set window size to WINDOW_WIDTH*WINDOW_HEIGHT
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//Set window position
	glutInitWindowPosition(0, 0);

	//Create and set main window title
	int mainWindow = glutCreateWindow("Alpha Triangles in a 3D Coordinate System");
	glClearColor(0, 0, 0, 0); //Clears the buffer of OpenGL; sets the background color to black.

	//Init solar system
	CreateSolarSystem();

	//Sets display function
	glutDisplayFunc(Display);

	glutMainLoop();//Main display loop, will display until terminate
	return 0;
}



//Main display loop, this function will be called again and again by OpenGL
void Display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();


	while (true)
	{
		//Display triangles here...
		//testTriangle.Draw(GetRandomColor());
		UpdateSolarSystem();

		//Draws pixel on screen, width and height must match pixel buffer dimension
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);

		//Window refresh
		glFlush();

		//Sleep(SLEEP_DURATION);
	}
}

Color4 GetRandomColor()
{
	/*
	* Set R, G, and B to a random number between 0.5 and 1.0, inclusive,
	* and then return that color.
	*/
	float newR = 0.5f + ((rand() % 501) / 1000.0f);
	float newG = 0.5f + ((rand() % 501) / 1000.0f);
	float newB = 0.5f + ((rand() % 501) / 1000.0f);
	float newA = 0.8f + ((rand() % 201) / 1000.0f);
	return Color4(newR, newG, newB, newA);
}

void SetPixel(int x, int y, const Color3 &color)
{
	//Update the pixelBuffer
	for (int colorIndex = 0; colorIndex < Color3::Num__RGBParameters; colorIndex++)
	{
		if ((x * (int)Color3::Num__RGBParameters + y * WINDOW_WIDTH * (int)Color3::Num__RGBParameters + colorIndex) < 0 ||
			(x * (int)Color3::Num__RGBParameters + y * WINDOW_WIDTH * (int)Color3::Num__RGBParameters + colorIndex) >= WINDOW_WIDTH*WINDOW_HEIGHT * 3)
			break;
		pixelBuffer[x * (int)Color3::Num__RGBParameters + y * WINDOW_WIDTH * (int)Color3::Num__RGBParameters + colorIndex] = color[colorIndex]; //See pgs. 146-147 to optimize this.
	}

	//Sleep(SLEEP_DURATION);
}

void UpdateTriangleAndDepthBuffer(Triangle &triangle, const Vector3F &newRelativePosition)
{
	triangle.relativePosition = newRelativePosition;
	triangle.pixelInfoVec.clear();

	int worldX, worldY, worldZ;
	for (unsigned int scanLine = 0; scanLine < triangle.relativeXPairVec.size(); scanLine++)
	{
		for (int pixelX = (int)triangle.relativeXPairVec[scanLine].GetX(); pixelX < (int)triangle.relativeXPairVec[scanLine].GetY(); pixelX++)
		{
			worldX = triangle.GetBaseX(scanLine + (int)triangle.vertexArr[0].GetY()) + pixelX + (int)triangle.relativePosition.GetX();
			worldY = (int)triangle.vertexArr[0].GetY() + (int)triangle.relativePosition.GetY() + scanLine;
			worldZ = triangle.GetWorldZ(worldX, worldY);
			triangle.pixelInfoVec.push_back(Vector3I(worldX, worldY, worldZ));

			depthBuffer.UpdateBuffers(worldX, worldY, worldZ, triangle.color);
		}
	}
}



void CreateSolarSystem()
{
	sun = Triangle(Color4(1.0f, 1.0f, 0.0f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 50, -1.0f),
		Vector3F(WINDOW_WIDTH / 2 + 50, WINDOW_HEIGHT / 2 - 30, -1.0f),
		Vector3F(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 20, -1.0f));

	//Mercury
	planetVec.push_back(Triangle(Color4(8.0f, 0.1f, 0.3f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2, -2.0f),
		Vector3F(WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20, -2.0f),
		Vector3F(WINDOW_WIDTH / 2 + 35, WINDOW_HEIGHT / 2 + 30, -2.0f)));


	////Venus
	planetVec.push_back(Triangle(Color4(139 / 255.0f, 69 / 255.0f, 16 / 255.0f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 15, WINDOW_HEIGHT / 2 - 10, -3.0f),
		Vector3F(WINDOW_WIDTH / 2 + 5, WINDOW_HEIGHT / 2 - 5, -3.0f),
		Vector3F(WINDOW_WIDTH / 2 + 25, WINDOW_HEIGHT / 2 + 40, -3.0f)));

	////Earth
	planetVec.push_back(Triangle(Color4(0.0f, 1.0f, 0.8f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2, -4.0f),
		Vector3F(WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20, -4.0f),
		Vector3F(WINDOW_WIDTH / 2 + 35, WINDOW_HEIGHT / 2 + 30, -4.0f)));

	////Mars
	planetVec.push_back(Triangle(Color4(1.0f, 0.0f, 0.0f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2 - 10, -5.0f),
		Vector3F(WINDOW_WIDTH / 2 + 5, WINDOW_HEIGHT / 2 + 15, -5.0f),
		Vector3F(WINDOW_WIDTH / 2 + 15, WINDOW_HEIGHT / 2 + 10, -5.0f)));

	//Jupiter
	planetVec.push_back(Triangle(Color4(244 / 255.0f, 164 / 255.0f, 96 / 255.0f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 35, WINDOW_HEIGHT / 2 - 35, -6.0f),
		Vector3F(WINDOW_WIDTH / 2 + 35, WINDOW_HEIGHT / 2, -6.0f),
		Vector3F(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 33, -6.0f)));

	//Saturn
	planetVec.push_back(Triangle(Color4(218 / 255.0f, 165 / 255.0f, 32 / 255.0f, 0.95f),
		Vector3F(WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT / 2 + 5, -7.0f),
		Vector3F(WINDOW_WIDTH / 2 + 25, WINDOW_HEIGHT / 2 - 30, -7.0f),
		Vector3F(WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 + 28, -7.0f)));

	////Uranus
	//planetVec.push_back(Triangle(Color4(0.2f, 0.7f, 1.0f, 0.95f),
	//	Vector3F(WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2, -8.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20, -8.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 35, WINDOW_HEIGHT / 2 + 30, -8.0f)));

	//////Neptune
	//planetVec.push_back(Triangle(Color4(0.1f, 0.5f, 0.8f, 0.95f),
	//	Vector3F(WINDOW_WIDTH / 2 - 15, WINDOW_HEIGHT / 2 - 10, -9.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 5, WINDOW_HEIGHT / 2 - 5, -9.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 25, WINDOW_HEIGHT / 2 + 40, -9.0f)));

	////Pluto
	//planetVec.push_back(Triangle(Color4(0.7f, 0.7f, 1.0f, 0.95f),
	//	Vector3F(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2 - 5, -10.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 5, WINDOW_HEIGHT / 2 + 5, -10.0f),
	//	Vector3F(WINDOW_WIDTH / 2 + 15, WINDOW_HEIGHT / 2 + 10, -10.0f)));


	//Create alien planet
	alienPlanet = Triangle(Color4(120 / 255.0f, 81 / 255.0f, 169 / 255.0f, 1.0f),
		Vector3F(50, 0, -30.0f),
		Vector3F(100, 150, -30.0f),
		Vector3F(75, WINDOW_HEIGHT - 1, 0.0f));
}

//Still needs a prototype above
void UpdatePlanets()
{
	const float PI = 3.14159f;
	Vector3F newRelativePosition;
	int radius = 0;
	float speedFactor = 0;
	for (unsigned int planet = 0; planet < planetVec.size(); planet++)
	{
		radius = 40 * (planet + 1);
		speedFactor = (planetVec.size() - planet) * (PI / 2);
		newRelativePosition = Vector3F(radius * cos(theta * speedFactor),
			radius * sin(theta * speedFactor),
			0.0f);
		
		//Remove pixel colors from depthBuffer array corresponding to previous position
		depthBuffer.MaskBuffers(planetVec[planet]);

		//Update the planetVec[planet] triangle's relativePosition
		//planetVec[planet].UpdatePosition(newRelativePosition);

		//Update the pixel colors in depthBuffer array corresponding to new position
		//depthBuffer.UpdateBuffers(planetVec[planet]);

		UpdateTriangleAndDepthBuffer(planetVec[planet], newRelativePosition);
	}
	
	//This function is way too slow. Find ways to not have to loop through every single pixel,
	//and only redraw the pixels that need to be redrawn. (Most of the screen is the background
	//color and these pixels shouldn't need to be redrawn every frame).
	//depthBuffer.Draw();

	theta += 0.01f;
}


//Asteroid global variables
const int MAX_ASTEROIDS = 10;
const int NEEDED_ELAPSED_TIME = CLOCKS_PER_SEC / 2; //Half a second
const int ASTEROID_X_SPEEED = 40;
int timeOfLastCreatedAsteroid = -1;


void CreateAsteroid()
{
	float newOpacity = 0.5f + ((rand() % 6) / 5.0f);
	Vector3F newVertex = Vector3F(0.0f, (float)(15 + (rand() % (WINDOW_HEIGHT - 36))), -10.0f);
	asteroidVec.push_back(Triangle(GetRandomColor(),
		newVertex,
		Vector3F((float)(rand() % 30), newVertex.GetY() + 5.0f + (float)(rand() % 16), -10.0f),
		Vector3F(20.0f + (float)(rand() % 70), newVertex.GetY() - 15.0f + (float)(rand() % 16), -10.0f)));
	/*
	asteroidVec.push_back(Triangle(Color4((165 + (rand() % 16)) / 255.0f, (42 + (rand() % 16)) / 255.0f, (42 - (rand() % 16)) / 255.0f, newOpacity),
		newVertex,
		Vector3F((float)(rand() % 30), newVertex.GetY() + 5.0f + (float)(rand() % 16), -10.0f),
		Vector3F(20.0f + (float)(rand() % 70), newVertex.GetY() - 15.0f + (float)(rand() % 16), -10.0f)));
		*/
}

void UpdateAsteroids()
{
	Vector3F newRelativePosition;
	unsigned int vecSize = asteroidVec.size();
	for (unsigned int asteroid = 0; asteroid < vecSize; asteroid++)
	{
		newRelativePosition = Vector3F(asteroidVec[asteroid].relativePosition.GetX() + ASTEROID_X_SPEEED,
										0.0f,
										0.0f);
		depthBuffer.MaskBuffers(asteroidVec[asteroid]);

		//If an asteroid has gone off-screen, erase it.
		if (asteroidVec[asteroid].vertexArr[0].GetX() + asteroidVec[asteroid].relativePosition.GetX() >= WINDOW_WIDTH ||
			asteroidVec[asteroid].vertexArr[1].GetX() + asteroidVec[asteroid].relativePosition.GetX() >= WINDOW_WIDTH ||
			asteroidVec[asteroid].vertexArr[2].GetX() + asteroidVec[asteroid].relativePosition.GetX() >= WINDOW_WIDTH)
		{
			asteroidVec.erase(asteroidVec.begin() + asteroid);
			return;
		}

		UpdateTriangleAndDepthBuffer(asteroidVec[asteroid], newRelativePosition);
	}

	if (vecSize < MAX_ASTEROIDS && (int)clock() - timeOfLastCreatedAsteroid >= NEEDED_ELAPSED_TIME)
	{
		CreateAsteroid();
		timeOfLastCreatedAsteroid = (int)clock();
	}
}

void UpdateSolarSystem()
{
	UpdateTriangleAndDepthBuffer(sun, sun.relativePosition);

	UpdatePlanets();
	UpdateAsteroids();

	UpdateTriangleAndDepthBuffer(alienPlanet, alienPlanet.relativePosition);
}