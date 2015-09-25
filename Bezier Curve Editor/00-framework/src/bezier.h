#ifndef __BEZIERH__
#define __BEZIERH__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "shapes.h"

using namespace std;

//derived class from ShapesC
class BezierCurveC : public ShapesC
{
public:
	BezierCurveC();
	BezierCurveC(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
	BezierCurveC(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int n);
	virtual void Render();
private:
	void InitArrays();
	void Generate(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int n);
	glm::vec2 GetBezierCurvePoint(float t, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
	void DeCasteljau(vector<glm::vec2> *points);
	void SplitCurve(vector<glm::vec2> *points, vector<glm::vec2> *left, vector<glm::vec2> *right);
};

#endif
