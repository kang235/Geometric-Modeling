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

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

using namespace std;
using namespace glm;

bool needRedisplay = false;
ShapesC* sphere;
ShapesC* circle;
CurveC* curve;
GLuint pointsCount = 10240;
GLuint selectPointIdx = 0;
GLuint selectPointInc = 1;
glm::vec3 selectPointPos;

//shader program ID
GLuint shaderProgram;
GLfloat ftime = 0.f;
glm::mat4 view = glm::mat4(1.0);					//viewing matrix is identity
glm::mat4 proj = glm::perspective(60.0f,			//fovy
	1.0f,			//aspect
	0.01f, 1000.f); //near, far
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

void Arm(glm::mat4 m)
{
	//let's use instancing
	//m=glm::translate(m,glm::vec3(0,0.5,0.0));
	//m=glm::scale(m,glm::vec3(1.0f,1.0f,1.0f));
	//circle->SetModel(m);
	////now the normals
	//glm::mat3 modelViewN=glm::mat3(view*m);
	//modelViewN= glm::transpose(glm::inverse(modelViewN));
	//circle->SetModelViewN(modelViewN);
	//circle->Render();

	//m=glm::translate(m,glm::vec3(0.0,0.5,0.0));
	//m=glm::rotate(m,-20.0f*ftime,glm::vec3(0.0,0.0,1.0));
	//m=glm::translate(m,glm::vec3(0.0,1.5,0.0));
	//circle->SetModel(glm::scale(m,glm::vec3(0.5f,1.0f,0.5f)));
	//modelViewN=glm::mat3(view*m);
	//modelViewN= glm::transpose(glm::inverse(modelViewN));
	//circle->SetModelViewN(modelViewN);
	//circle->Render();

	curve->SetModel(m);
	glm::mat3 modelViewN = glm::mat3(view*m);
	modelViewN = glm::transpose(glm::inverse(modelViewN));
	curve->SetModelViewN(modelViewN);
	curve->Render();

	m = glm::translate(m, glm::vec3(selectPointPos.x, selectPointPos.y, selectPointPos.z));
	m = glm::scale(m, glm::vec3(0.08f, 0.08f, 0.08f));
	sphere->SetModel(m);
	//now the normals
	modelViewN = glm::mat3(view*m);
	modelViewN = glm::transpose(glm::inverse(modelViewN));
	sphere->SetModelViewN(modelViewN);
	sphere->Render();
}

//the main rendering function
void RenderObjects()
{
	//const int range=3;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 1, 1);
	glPointSize(2);
	glLineWidth(1);
	//set the projection and view once for the scene
	glUniformMatrix4fv(params.projParameter, 1, GL_FALSE, glm::value_ptr(proj));
	view = glm::lookAt(glm::vec3(10.f, 10.f, 10.f),//eye
		glm::vec3(0, 0, 0),  //destination
		glm::vec3(0, 1, 0)); //up
	//view = glm::lookAt(glm::vec3(25 * sin(ftime / 180.f), 0.f, 25 * cos(ftime / 180.f)),//eye
	//	glm::vec3(0, 0, 0),  //destination
	//	glm::vec3(0, 1, 0)); //up

	glUniformMatrix4fv(params.viewParameter, 1, GL_FALSE, glm::value_ptr(view));
	//set the light
	static glm::vec4 pos;
	//pos.x = 20 * sin(ftime / 12); pos.y = -10; pos.z = 20 * cos(ftime / 12); pos.w = 1;
	pos.x = 0; pos.y = 0; pos.z = -0; pos.w = 1;
	light.SetPos(pos);
	light.SetShaders();
	/*for (int i=-range;i<range;i++)
	{
	for (int j=-range;j<range;j++)
	{
	glm::mat4 m=glm::translate(glm::mat4(1.0),glm::vec3(4*i,0,4*j));
	Arm(m);
	}
	}*/

	glm::mat4 m = glm::mat4(1.0);
	Arm(m);
}

void Idle(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ftime += 0.15f;
	glUseProgram(shaderProgram);
	RenderObjects();
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
	case 27: exit(0); break;
	case '=':
	case '+':
		{
			if (selectPointIdx >= pointsCount - 1)
				selectPointIdx = 0;
			else
				selectPointIdx += selectPointInc;
			selectPointPos = curve->GetVetex(selectPointIdx);
			cout << "Selected point index: " << selectPointIdx << endl;
			break;
		}
	case '-':
	case '_':
		{
			if (selectPointIdx <= 0)
				selectPointIdx = pointsCount - 1;
			else
				selectPointIdx -= selectPointInc;
			selectPointPos = curve->GetVetex(selectPointIdx);
			cout << "Selected point index: " << selectPointIdx << endl;
			break;
		}
	}
	//cout << "keyboard triggered" << endl;
	glutPostRedisplay();
}


//special keyboard callback
void SpecKbdPress(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}

	}
	glutPostRedisplay();
}

//called when a special key is released
void SpecKbdRelease(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}
	}
	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y)
{
	//cout << "Location is " << "[" << x << "'" << y << "]" << endl;
}


void InitializeProgram(GLuint *program)
{
	std::vector<GLuint> shaderList;

	//load and compile shaders 	
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/phong.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/phong.frag")));
	//shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/phong.vert")));
	//shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/pass.frag")));

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
	sphere = new SphereC();
	sphere->SetKa(glm::vec3(0.f, 1.f, 0.f));
	sphere->SetSh(200);
	sphere->SetModel(glm::mat4(1.0));
	sphere->SetModelMatrixParamToShader(params->modelParameter);
	sphere->SetModelViewNMatrixParamToShader(params->modelViewNParameter);
	sphere->SetKaToShader(params->kaParameter);
	sphere->SetKdToShader(params->kdParameter);
	sphere->SetKsToShader(params->ksParameter);
	sphere->SetShToShader(params->shParameter);

	circle = new CircleC(50);
	circle->SetKa(glm::vec3(0.f, 0.f, 1.f));
	circle->SetSh(200);
	circle->SetModel(glm::mat4(1.0));
	circle->SetModelMatrixParamToShader(params->modelParameter);
	circle->SetModelViewNMatrixParamToShader(params->modelViewNParameter);
	circle->SetKaToShader(params->kaParameter);
	circle->SetKdToShader(params->kdParameter);
	circle->SetKsToShader(params->ksParameter);
	circle->SetShToShader(params->shParameter);

	curve = new CurveC(pointsCount);
	selectPointPos = curve->GetVetex(selectPointIdx);
	cout << "Selected point index: " << selectPointIdx << endl;
	curve->SetKa(glm::vec3(1.f, 0.f, 0.f));
	curve->SetSh(200);
	curve->SetModel(glm::mat4(1.0));
	curve->SetModelMatrixParamToShader(params->modelParameter);
	curve->SetModelViewNMatrixParamToShader(params->modelViewNParameter);
	curve->SetKaToShader(params->kaParameter);
	curve->SetKdToShader(params->kdParameter);
	curve->SetKsToShader(params->ksParameter);
	curve->SetShToShader(params->shParameter);
}

int main(int argc, char **argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Model View Projection GLSL");
	GLenum err = glewInit();
	if (GLEW_OK != err){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(SpecKbdRelease); //smooth motion
	glutSpecialFunc(SpecKbdPress);
	InitializeProgram(&shaderProgram);
	InitShapes(&params);
	glutMainLoop();
	return 0;
}
