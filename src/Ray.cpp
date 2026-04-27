#include "Ray.h"
#include <iostream>

RayResult Ray::intersectSphere(Geometry sphere)
{
	
	auto a = direction.dot(direction);
	auto b = ((-2) * direction).dot(sphere.center - origine);
	auto c = (sphere.center - origine).dot(sphere.center - origine) - sphere.radius * sphere.radius;

	auto discriminant = b * b - 4 * a * c;

	RayResult result;
	
	if (discriminant > 0) {
		auto t1 = (-b + std::sqrt(discriminant)) / (2 * a);
		auto t2 = (-b - std::sqrt(discriminant)) / (2 * a);
		
		if (t1 >= 0 && t2 >= 0) { //both points in front of camera
			if (t1 > t2) {
				result.point = origine + t2 * direction;
				result.hit = true;
			}
			else {
				result.point = origine + t1 * direction;
				result.hit = true;
			}
		}
		else if (t1 >= 0 && t2 < 0) { //t1 in front of camera
			result.point = origine + t1 * direction;
			result.hit = true;
		}
		else if (t1 < 0 && t2 >= 0) { //t2 in front of camera
			result.point = origine + t2 * direction;
			result.hit = true;
		}
		else { //both behind camera
			result.hit = false;
		}
	}
	else if(discriminant == 0){
		result.point = origine + direction * ( - b / (2 * a));
		result.hit = true;
	}
	else {
		result.hit = false;
	}

	//normal calculation
	if (result.hit) {
		result.normal = (result.point - sphere.center).normalized();
	}

	return result;
}

RayResult Ray::intersectTriangle(Geometry triangle)
{
	RayResult result;

	const double NEAR_ZERO = 0.0000001;

	Eigen::Vector3f edge1 = triangle.p2 - triangle.p1;
	Eigen::Vector3f edge2 = triangle.p3 - triangle.p1;

	Eigen::Vector3f directionCrossEdge2 = direction.cross(edge2);

	double determinant = edge1.dot(directionCrossEdge2);

	//check if ray is parallel
	if (determinant > -NEAR_ZERO && determinant < NEAR_ZERO) {
		result.hit = false;
		return result;
	}

	double invertedDeterminant = 1.0 / determinant;

	Eigen::Vector3f p1ToOrigine = origine - triangle.p1;
	
	double u = invertedDeterminant * p1ToOrigine.dot(directionCrossEdge2);

	Eigen::Vector3f p1ToOrigineCrossEdge1 = p1ToOrigine.cross(edge1);

	double v = invertedDeterminant * direction.dot(p1ToOrigineCrossEdge1);

	//check if ray intersects triangle
	if (u < 0 || v < 0 || u + v > 1) {
		result.hit = false;
		return result;
	}

	double t = invertedDeterminant * edge2.dot(p1ToOrigineCrossEdge1);

	if (t > NEAR_ZERO) {// in front of camera
		result.point = origine + t * direction;
		result.hit = true;

		//Normal calculation
		result.normal = edge1.cross(edge2).normalized();
		return result;
	}
	else {
		result.hit = false;
		return result;
	}
}

Ray::Ray(Eigen::Vector3f origine, Eigen::Vector3f direction)
{
	Ray::origine = origine;
	Ray::direction = direction;
}

RayResult Ray::intersect(Geometry geometry)
{
	if (geometry.type == "sphere") {
		return intersectSphere(geometry);
	}
	else {
		return intersectTriangle(geometry);
	}
}
