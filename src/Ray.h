#define Ray_H
#include <Eigen/Core>
#include <Eigen/Dense>
#include "Scene.h"

using std::string;

struct RayResult {
	Eigen::Vector3f point{ 0,0,0 };
	Eigen::Vector3f normal{ 0,0,0 };
	bool hit = false;
};

class Ray {
private:
	RayResult intersectSphere(Geometry sphere);
	RayResult intersectTriangle(Geometry triangle);
	Eigen::Vector3f origine{ 0, 0, 0 };
	Eigen::Vector3f direction{ 0, 0, 0 };
public:
	Ray(Eigen::Vector3f origine, Eigen::Vector3f direction);
	RayResult intersect(Geometry geometry);
};