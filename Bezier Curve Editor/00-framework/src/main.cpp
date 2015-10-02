/**********************************/
/* Framework for CGT 581-G Geometric Modeling
   (C) Bedrich Benes 2014
   bbenes ~ at ~ purdue.edu       */
   /**********************************/

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <string.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//STL
#include <GL/glew.h>
#include <GL/glut.h>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/half_float.hpp>
#include "shaders.h"    
#include "shapes.h"    
#include "lights.h"  
#include "bezier.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

#define PROMPT0 "Press arrow keys to adjust camera; Press CTRL+Z to undo the changes for points;"
#define PROMPT1 "Press SHIFT to add a C1 continuity new point; Press ALT to enforce next point to be G1 continuity."

using namespace std;
using namespace glm;

bool needRedisplay = false;
//ShapesC* sphere;
CircleC* circle;
//BezierCurveC* curve;
vector<glm::vec2>* points = new vector<glm::vec2>();
vector<BezierCurveC*>* curves = new vector<BezierCurveC*>();
bool g1triggered = false;

//shader program ID
GLuint shaderProgram;
GLfloat ftime = 0.f;

GLfloat camX = .0f;
GLfloat camY = .0f;
GLfloat camZ = 20.0f;
GLfloat camStep = .1f;

glm::mat4 view = glm::mat4(1.0);					//viewing matrix is identity
glm::mat4 proj = glm::perspective(60.0f,			//fovy
	1.0f,			//aspect
	0.01f, -camZ); //near, far

class ShaderParamsC
{
public:
	GLint modelParameter;		//modeling matrix
	GLint modelViewNParameter;  //modeliview for normals
	GLint viewParameter;		//viewing matrix
	GLint projParameter;		//projection matrix
	//material
	GLint kaParameter;			//ambient material
	GLint kdParameter;			//diffuse material
	GLint ksParameter;			//specular material
	GLint shParameter;			//shinenness material
} params;


LightC light;


//the main window size
GLint wWindow = 800;
GLint hWindow = 800;

float sh = 1;

/*********************************
Some OpenGL-related functions
**********************************/

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	wWindow = w;
	hWindow = h;
}

void Arm(BezierCurveC *c, glm::mat4 m)
{
	c->SetModel(m);
	//now the normals
	glm::mat3 modelViewN = glm::mat3(view*m);
	modelViewN = glm::transpose(glm::inverse(modelViewN));
	c->SetModelViewN(modelViewN);
	c->Render();
}

void Points(glm::mat4 m)
{
	//let's use instancing
	m = glm::scale(m, glm::vec3(0.1f, 0.1f, 0.1f));
	m = glm::rotate(m, 90.0f, glm::vec3(1, 0, 0));
	circle->SetModel(m);
	//now the normals
	glm::mat3 modelViewN = glm::mat3(view*m);
	modelViewN = glm::transpose(glm::inverse(modelViewN));
	circle->SetModelViewN(modelViewN);
	circle->Render();
}

//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//set the projection and view once for the scene
	glUniformMatrix4fv(params.projParameter, 1, GL_FALSE, glm::value_ptr(proj));
	//view=glm::lookAt(glm::vec3(25*sin(ftime/40.f),5.f,15*cos(ftime/40.f)),//eye
	//			     glm::vec3(0,0,0),  //destination
	//			     glm::vec3(0,1,0)); //up
	view = glm::lookAt(glm::vec3(camX, camY, camZ),//eye
		glm::vec3(camX, camY, 0),  //destination
		glm::vec3(0, 1, 0)); //up

	glUniformMatrix4fv(params.viewParameter, 1, GL_FALSE, glm::value_ptr(view));
	//set the light
	static glm::vec4 pos;
	//pos.x = 20 * sin(ftime / 12); pos.y = -10; pos.z = 20 * cos(ftime / 12); pos.w = 1;
	light.SetPos(pos);
	light.SetShaders();

	vector<glm::vec2> &p = *points;

	for (vector<glm::vec2>::iterator i = p.begin(); i != p.end(); ++i) {
		glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(i->x, i->y, 0));
		Points(m);
	}

	for (vector<BezierCurveC *>::iterator i = curves->begin(); i != curves->end(); ++i) {
		Arm(*i, glm::mat4(1.0));
	}
}

//render text 
void RenderText(char *text, glm::vec2 pos)
{
	//use fixed pipeline
	glUseProgramObjectARB(0);

	glColor3f(0.0f, 1.0f, 1.0f);
	glWindowPos2i((GLint)pos.x, (GLint)pos.y);
	for (unsigned i = 0; i < strlen(text); ++i) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)text[i]);
	}
}

void Idle(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ftime += 0.15f;
	glUseProgram(shaderProgram);
	RenderObjects();
	RenderText(PROMPT0, glm::vec2(20, 40));
	RenderText(PROMPT1, glm::vec2(20, 20));
	glutSwapBuffers();
}

void Display(void)
{
}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	switch (a)
	{
		case 27: {exit(0); break; }
		case 26: 
		{
			cout << "Ctrl + Z pressed - Undo" << endl; 

			//count points remaining that can not form a curve
			int n = points->size() % 4; 

			if (n == 1 && points->size() > 4) { 
				curves->pop_back();
				//the 1st point is the 4th point of last curve, pop twice
				points->pop_back();
				points->pop_back();
			}
			else {
				if (points->size() == 0)
					return;
				points->pop_back();
			}
			break; 
		}
	}

	glutPostRedisplay();
}


//special keyboard callback
void SpecKbdPress(int a, int x, int y)
{
	int state = glutGetModifiers();

	if (state == GLUT_ACTIVE_SHIFT)
	{
		int n = points->size();
		if (n % 4 == 1 && n > 4)
		{
			glm::vec2 p;

			p = curves->at(curves->size() - 1)->GetC1NextPoint();
			points->push_back(p);

			cout << "Shift pressed - enforce C1 continuity" << endl;
		}
	}
	else if (state == GLUT_ACTIVE_ALT)
	{
		int n = points->size();
		if (n % 4 == 1 && n > 4)
		{
			g1triggered = true;
			cout << "ALT pressed - enforce G1 continuity" << endl;
		}
	}


	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		camX -= camStep;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		camX += camStep;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		camY -= camStep;
		break;
	}
	case GLUT_KEY_UP:
	{
		camY += camStep;
		break;
	}
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cout << "Location is " << "[" << x << "'" << y << "]" << endl;
		glm::vec3 windowView = glm::vec3(wWindow - x, y, 1);
		glm::vec4 viewport = glm::vec4(0.0f, 0.0f, (float)wWindow, (float)hWindow);
		glm::vec3 pos = glm::unProject(windowView, view, proj, viewport);

		//cout << "World Location is " << "[" << pos.x << "'" << pos.y << "]" << endl;

		if (g1triggered) { //if alt pressed, enforce next point to be on G1 continuity
			g1triggered = false;

			glm::vec2 p;

			BezierCurveC *c = curves->at(curves->size() - 1);
			glm::vec2 p2 = c->GetControlPoints()->GetP2();
			glm::vec2 p3 = c->GetControlPoints()->GetP3();

			if ((p2.y - p3.y) * (p3.y - pos.y) < 0) //take care of inverse case
			{
				pos.y = p3.y - pos.y + p3.y;
			}
			pos.x = (pos.y - p3.y)*(p2.x - p3.x) / (p2.y - p3.y) + p3.x; //put the point on tangent line
		}

		vector<glm::vec2> &p = *points;
		p.push_back(glm::vec2(pos.x, pos.y));

		int curveCount = p.size() >> 2;
		if (curveCount != curves->size()) {  //new four points added
			BezierCurveC *curve = new BezierCurveC(p[p.size() - 4], p[p.size() - 3], p[p.size() - 2], p[p.size() - 1]);
			curve->SetKa(glm::vec3(0.0, 1.0, 0.0));
			curve->SetSh(200);
			curve->SetModel(glm::mat4(1.0));
			curve->SetModelMatrixParamToShader(params.modelParameter);
			curve->SetModelViewNMatrixParamToShader(params.modelViewNParameter);
			curve->SetKaToShader(params.kaParameter);
			curve->SetShToShader(params.shParameter);
			curves->push_back(curve);

			p.push_back(glm::vec2(pos.x, pos.y)); //end point and start point
		}
	}
}

void InitializeProgram(GLuint *program)
{
	std::vector<GLuint> shaderList;

	//load and compile shaders 	
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/phong.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/phong.frag")));
	/*shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/phong.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/pass.frag")));*/

	//create the shader program and attach the shaders to it
	*program = CreateProgram(shaderList);

	//delete shaders (they are on the GPU now)
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	params.modelParameter = glGetUniformLocation(*program, "model");
	params.modelViewNParameter = glGetUniformLocation(*program, "modelViewN");
	params.viewParameter = glGetUniformLocation(*program, "view");
	params.projParameter = glGetUniformLocation(*program, "proj");
	//now the material properties
	params.kaParameter = glGetUniformLocation(*program, "mat.ka");
	params.kdParameter = glGetUniformLocation(*program, "mat.kd");
	params.ksParameter = glGetUniformLocation(*program, "mat.ks");
	params.shParameter = glGetUniformLocation(*program, "mat.sh");
	//now the light properties
	light.SetLaToShader(glGetUniformLocation(*program, "light.la"));
	light.SetLdToShader(glGetUniformLocation(*program, "light.ld"));
	light.SetLsToShader(glGetUniformLocation(*program, "light.ls"));
	light.SetLposToShader(glGetUniformLocation(*program, "light.lPos"));
}

void InitShapes(ShaderParamsC *params)
{
	//create circle for indicating the control points of Bezier Curve
	circle = new CircleC(16);
	circle->SetKa(glm::vec3(0.0, 0.0, 1.0));
	circle->SetSh(200);
	circle->SetModel(glm::mat4(1.0));
	circle->SetModelMatrixParamToShader(params->modelParameter);
	circle->SetModelViewNMatrixParamToShader(params->modelViewNParameter);
	circle->SetKaToShader(params->kaParameter);
	circle->SetShToShader(params->shParameter);
}

int main(int argc, char **argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Bezier Curve Editor");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd);
	glutSpecialFunc(SpecKbdPress);
	InitializeProgram(&shaderProgram);
	InitShapes(&params);
	glutMainLoop();
	return 0;
}
