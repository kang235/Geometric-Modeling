#ifndef __OSCULATINGH__
#define __OSCULATINGH__

#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
using namespace std;

class OsculatingCircleInfo
{
public:
	OsculatingCircleInfo(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc);
	OsculatingCircleInfo(std::vector<glm::vec3> v);

	glm::vec3 pcenter;
	float radius;
	glm::vec3 vaxis;
};

#endif