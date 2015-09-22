#include "osculating.h"

//magical barycentric coordinates
OsculatingCircleInfo::OsculatingCircleInfo(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc)
{
	// triangle "edges"
	const glm::vec3 t = pb - pa;
	const glm::vec3 u = pc - pa;
	const glm::vec3 v = pc - pb;

	// triangle normal
	const glm::vec3 w = glm::cross(t, u);
	const float wsl = glm::dot(w, w);

	// helpers
	const float iwsl2 = 1.0f / (2.0f * wsl);
	const float tt = glm::dot(t, t);
	const float uu = glm::dot(u, u);

	// result circle
	glm::vec3 circleCenter = pa + (u*tt*glm::dot(u, v) - t*uu*glm::dot(t, v)) * iwsl2;
	float circleRadius = sqrt(tt * uu * (glm::dot(v, v)) * iwsl2 * 0.5f);
	glm::vec3 circleAxis = w * (float)(1 / sqrt(wsl));

	pcenter = circleCenter;
	radius = circleRadius;
	vaxis = circleAxis;
}

OsculatingCircleInfo::OsculatingCircleInfo(std::vector<glm::vec3> container)
{
	int half = (container.size() - 1) / 2;
	glm::vec3 sum1 = glm::vec3(0.f);
	glm::vec3 sum2 = glm::vec3(0.f);

	for (int i = 0; i < half; ++i) {
		sum1 += container[i];
		sum2 += container[container.size() - 1 - i];
	}
	glm::vec3 pa = sum1 * (1.f / half);
	glm::vec3 pc = sum2 * (1.f / half);
	glm::vec3 pb = container[(container.size() - 1) / 2];

	// triangle "edges"
	const glm::vec3 t = pb - pa;
	const glm::vec3 u = pc - pa;
	const glm::vec3 v = pc - pb;

	// triangle normal
	const glm::vec3 w = glm::cross(t, u);
	const float wsl = glm::dot(w, w);

	// helpers
	const float iwsl2 = 1.0f / (2.0f * wsl);
	const float tt = glm::dot(t, t);
	const float uu = glm::dot(u, u);

	// result circle
	glm::vec3 circleCenter = pa + (u*tt*glm::dot(u, v) - t*uu*glm::dot(t, v)) * iwsl2;
	float circleRadius = sqrt(tt * uu * (glm::dot(v, v)) * iwsl2 * 0.5f);
	glm::vec3 circleAxis = w * (float)(1 / sqrt(wsl));

	pcenter = circleCenter;
	radius = circleRadius;
	vaxis = circleAxis;
}