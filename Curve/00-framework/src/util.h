#ifndef __UTILH__
#define __UTILH__

#include <glm/glm.hpp>

class Util
{
public:
	static glm::mat3x3 GetRotationMatrixWith2Vectors(glm::vec3 vectorBefore, glm::vec3 vectorAfter);
	static glm::mat3x3 CalculateRotationMatrix(double angle, glm::vec3 u);
};

#endif