#include "util.h"

glm::mat3x3 Util::GetRotationMatrixWith2Vectors(glm::vec3 vectorBefore, glm::vec3 vectorAfter) {
	glm::vec3 rotationAxis;
	double rotationAngle;
	glm::mat3x3 rotationMatrix;
	rotationAxis = glm::cross(vectorBefore, vectorAfter);
	rotationAngle = glm::acos(glm::dot(vectorBefore, vectorAfter) / glm::sqrt(glm::dot(vectorBefore, vectorBefore)) / glm::sqrt(glm::dot(vectorAfter, vectorAfter)));
	rotationMatrix = CalculateRotationMatrix(rotationAngle, rotationAxis);
	return rotationMatrix;
}

glm::mat3x3 Util::CalculateRotationMatrix(double angle, glm::vec3 u) {
	float norm = glm::sqrt(glm::dot(u, u));
	glm::mat3x3 rotationMatrix;

	u.x = u.x / norm;
	u.y = u.y / norm;
	u.z = u.z / norm;

	rotationMatrix[0] = glm::vec3(glm::cos(angle) + u.x * u.x * (1 - glm::cos(angle)),
		u.x * u.y * (1 - glm::cos(angle) - u.z * glm::sin(angle)),
		u.y * glm::sin(angle) + u.x * u.z * (1 - glm::cos(angle)));

	rotationMatrix[1] = glm::vec3(u.z * glm::sin(angle) + u.x * u.y * (1 - glm::cos(angle)),
		glm::cos(angle) + u.y * u.y * (1 - glm::cos(angle)),
		-u.x * glm::sin(angle) + u.y * u.z * (1 - glm::cos(angle)));

	rotationMatrix[2] = glm::vec3(-u.y * glm::sin(angle) + u.x * u.z * (1 - glm::cos(angle)),
		u.x * glm::sin(angle) + u.y * u.z * (1 - glm::cos(angle)),
		glm::cos(angle) + u.z * u.z * (1 - glm::cos(angle)));

	return rotationMatrix;
}