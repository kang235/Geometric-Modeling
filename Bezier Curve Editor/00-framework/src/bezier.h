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
class BezierCurveControlPointsC : public ShapesC
{
public:
	BezierCurveControlPointsC(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
	BezierCurveControlPointsC(vector<glm::vec2> *);
	glm::vec2 GetP0();
	glm::vec2 GetP1();
	glm::vec2 GetP2();
	glm::vec2 GetP3();
	void SetP0(glm::vec2 p0);
	void SetP1(glm::vec2 p0);
	void SetP2(glm::vec2 p0);
	void SetP3(glm::vec2 p0);

private:
	//control points
	glm::vec2 p0;
	glm::vec2 p1;
	glm::vec2 p2;
	glm::vec2 p3;
};

//derived class from ShapesC
class BezierCurveC : public ShapesC
{
public:
	BezierCurveC();
	BezierCurveC(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
	BezierCurveC(vector<glm::vec2> *);
	BezierCurveC(glm::vec2, glm::vec2, glm::vec2, glm::vec2, int);
	BezierCurveControlPointsC* GetControlPoints();
	void SetControlPoints(BezierCurveControlPointsC*);

	virtual void Render();
private:
	void InitArrays();
	void Generate(glm::vec2, glm::vec2, glm::vec2, glm::vec2, int);
	glm::vec2 GetBezierCurvePoint(float, glm::vec2, glm::vec2, glm::vec2, glm::vec2);
	void DeCasteljau(vector<glm::vec2> *);
	void SplitCurve(vector<glm::vec2> *, vector<glm::vec2> *, vector<glm::vec2> *);

	//control points
	BezierCurveControlPointsC* controlPts;
};

#endif
