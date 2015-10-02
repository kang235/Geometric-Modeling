#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "shapes.h"
#include "bezier.h"

using namespace std;

//help procedure that send values from glm::vec3 to a STL vector of float
//used for creating VBOs
inline void AddVertex(vector <GLfloat> *a, const glm::vec2 *v)
{
	a->push_back(v->x);
	a->push_back(v->y);
}

BezierCurveControlPointsC::BezierCurveControlPointsC(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
}
BezierCurveControlPointsC::BezierCurveControlPointsC(vector<glm::vec2> *points) {
	vector<glm::vec2> &p = *points;

	this->p0 = p[0];
	this->p1 = p[1];
	this->p2 = p[2];
	this->p3 = p[3];
}

glm::vec2 BezierCurveControlPointsC::GetP0() {
	return p0;
}

void BezierCurveControlPointsC::SetP0(glm::vec2 p) {
	p0 = p;
}

glm::vec2 BezierCurveControlPointsC::GetP1() {
	return p1;
}

void BezierCurveControlPointsC::SetP1(glm::vec2 p) {
	p1 = p;
}

glm::vec2 BezierCurveControlPointsC::GetP2() {
	return p2;
}

void BezierCurveControlPointsC::SetP2(glm::vec2 p) {
	p2 = p;
}

glm::vec2 BezierCurveControlPointsC::GetP3() {
	return p3;
}

void BezierCurveControlPointsC::SetP3(glm::vec2 p) {
	p3 = p;
}

//normal Bezier
BezierCurveC::BezierCurveC(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int n)
{
	controlPts = new BezierCurveControlPointsC(p0, p1, p2, p3);

	Generate(p0, p1, p2, p3, n);
	InitArrays();
}

//with De Casteljau Algrithom
BezierCurveC::BezierCurveC(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	controlPts = new BezierCurveControlPointsC(p0, p1, p2, p3);

	vector<glm::vec2> *points = new vector<glm::vec2>();
	points->push_back(p0);
	points->push_back(p1);
	points->push_back(p2);
	points->push_back(p3);

	DeCasteljau(points);
	InitArrays();
}

//with De Casteljau Algrithom
BezierCurveC::BezierCurveC(vector<glm::vec2> *points)
{
	controlPts = new BezierCurveControlPointsC(points);

	DeCasteljau(points);
	InitArrays();
}

//default constructor
BezierCurveC::BezierCurveC()
{
	vector<glm::vec2> *points = new vector<glm::vec2>();
	points->push_back(glm::vec2(-10, 0));
	points->push_back(glm::vec2(-5, 5));
	points->push_back(glm::vec2(5, -5));
	points->push_back(glm::vec2(10, 0));

	controlPts = new BezierCurveControlPointsC(points);

	DeCasteljau(points);

	InitArrays();
}

//p0, p1, p2, p3 are the control points
//n is the number of lines used for approximation
void BezierCurveC::Generate(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int n)
{
	float t, deltat;
	glm::vec2 p;
	deltat = 1.0f / n;
	t = 0;

	for (int i = 0; i < n; ++i) {
		p = GetBezierCurvePoint(t + deltat, p0, p1, p2, p3);

		AddVertex(&vertex, &p); //add the vertex
		glm::normalize(p);      //normalize it 
		AddVertex(&normal, &p); //and add the normal vector
		t += deltat;
	}
}

glm::vec2 BezierCurveC::GetBezierCurvePoint(float t, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	float b0, b1, b2, b3;
	static glm::vec2 point;

	b0 = (1 - t)*(1 - t)*(1 - t);
	b1 = 3 * t*(1 - t)*(1 - t);
	b2 = 3 * t *t*(1 - t);
	b3 = t * t * t;

	point.x = p0.x*b0 + p1.x*b1 + p2.x*b2 + p3.x*b3;
	point.y = p0.y*b0 + p1.y*b1 + p2.y*b2 + p3.y*b3;

	return point;
}

void BezierCurveC::DeCasteljau(vector<glm::vec2> *points)
{
	vector<glm::vec2>& ref = *points;
	vector<glm::vec2> *left, *right;

	left = new vector<glm::vec2>(4);
	right = new vector<glm::vec2>(4);

	if (SplitEndCondition(ref)) {
		AddVertex(&vertex, &ref[0]); //add the vertex
		glm::normalize(ref[0]);      //normalize it 
		AddVertex(&normal, &ref[0]); //and add the normal vector

		AddVertex(&vertex, &ref[3]); //add the vertex
		glm::normalize(ref[3]);      //normalize it 
		AddVertex(&normal, &ref[3]); //and add the normal vector

		return;
	}

	SplitCurve(points, left, right);
	DeCasteljau(left);
	DeCasteljau(right);
}

bool BezierCurveC::SplitEndCondition(vector<glm::vec2>& points)
{
	//distance
	if (glm::distance(points[0], points[3]) < END_DISTANCE
		&& glm::distance(points[0], points[1]) < END_DISTANCE
		&& glm::distance(points[1], points[2]) < END_DISTANCE
		&& glm::distance(points[2], points[3]) < END_DISTANCE)
		return true;

	//don't need the following conditions for practical use
	//area - Bretschneider's formula
	float a = glm::distance(points[0], points[1]);
	float b = glm::distance(points[1], points[2]);
	float c = glm::distance(points[2], points[3]);
	float d = glm::distance(points[3], points[0]);
	float s = (a + b + c + d) / 2;
	float t1 = glm::angle(glm::normalize(points[1] - points[0]), glm::normalize(points[3] - points[0]));
	float t2 = glm::angle(glm::normalize(points[1] - points[2]), glm::normalize(points[3] - points[2]));
	float t = (t1 + t2) / 2;
	double sqrArea = glm::abs((s - a) * (s - b) * (s - c) * (s - d) - a * b * c * d * glm::cos(t) * glm::cos(t));
	//cout << a << " " << b << " " << c << " " << d << " " << "theta1: " << t1 << " theta2: " << t2 << "; sqrArea: " << sqrArea << endl;
	if (sqrArea < END_SQR_AREA)
		return true;

	//flat
	glm::vec2 v0 = glm::normalize(points[3] - points[0]);
	glm::vec2 v1 = glm::normalize(points[1] - points[0]);
	glm::vec2 v2 = glm::normalize(points[2] - points[1]);
	glm::vec2 v3 = glm::normalize(points[3] - points[2]);

	float a0 = glm::angle(v0, v1);
	float a1 = glm::angle(v0, v2);
	float a2 = glm::angle(v0, v3);

	//cout << a0 << " " << a1 << " " << a2 << " " << endl;

	if (a0 <= END_ANGLE && a1 <= END_ANGLE && a2 <= END_ANGLE)
		return true;

	return false;
}

void BezierCurveC::SplitCurve(vector<glm::vec2> *points, vector<glm::vec2> *left, vector<glm::vec2> *right)
{
	vector<glm::vec2>& lr = *left; //reference of left
	vector<glm::vec2>& rr = *right;
	vector<glm::vec2>& pr = *points;
	glm::vec2 hlp;

	lr[0] = pr[0];
	lr[1] = (pr[0] + pr[1]) / 2.0f;
	hlp = (pr[1] + pr[2]) / 2.0f;
	lr[2] = (lr[1] + hlp) / 2.0f;
	rr[3] = pr[3];
	rr[2] = (pr[2] + pr[3]) / 2.0f;
	rr[1] = (rr[2] + hlp) / 2.0f;
	lr[3] = rr[0] = (lr[2] + rr[1]) / 2.0f;
}

BezierCurveControlPointsC* BezierCurveC::GetControlPoints()
{
	return controlPts;
}

void BezierCurveC::SetControlPoints(BezierCurveControlPointsC* p)
{
	controlPts = p;
}

glm::vec2 BezierCurveC::GetC1NextPoint()
{
	glm::vec2 p;

	p.x = 2 * controlPts->GetP3().x - controlPts->GetP2().x;
	p.y = 2 * controlPts->GetP3().y - controlPts->GetP2().y;

	return p;
}

glm::vec2 BezierCurveC::GetG1NextPoint(float k)
{
	glm::vec2 p;

	p.x = (controlPts->GetP3().x - controlPts->GetP2().x) / k + controlPts->GetP3().x;
	p.y = (controlPts->GetP3().y - controlPts->GetP2().y) / k + controlPts->GetP3().y;

	return p;
}

//Bezier Curve
void BezierCurveC::InitArrays()
{
	points = vertex.size();
	normals = normal.size();

	//get the vertex array handle and bind it
	glGenVertexArrays(1, &vaID);
	glBindVertexArray(vaID);

	//the vertex array will have two vbos, vertices and normals
	glGenBuffers(2, vboHandles);
	GLuint verticesID = vboHandles[0];
	GLuint normalsID = vboHandles[1];

	//send vertices
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, points*sizeof(GLfloat), &vertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	vertex.clear(); //no need for the vertex data, it is on the GPU now

					//send normals
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glBufferData(GL_ARRAY_BUFFER, normals*sizeof(GLfloat), &normal[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	normal.clear(); //no need for the normal data, it is on the GPU now
}

void BezierCurveC::Render()
{
	glBindVertexArray(vaID);
	//material properties
	glUniform3fv(kaParameter, 1, glm::value_ptr(ka));
	glUniform3fv(kdParameter, 1, glm::value_ptr(kd));
	glUniform3fv(ksParameter, 1, glm::value_ptr(ks));
	glUniform1fv(shParameter, 1, &sh);
	//model matrix
	glUniformMatrix4fv(modelParameter, 1, GL_FALSE, glm::value_ptr(model));
	//model for normals
	glUniformMatrix3fv(modelViewNParameter, 1, GL_FALSE, glm::value_ptr(modelViewN));
	glLineWidth(2);
	glDrawArrays(GL_LINE_STRIP, 0, points / 2);
	glLineWidth(1);
}