#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "../external/json.hpp"
#include <Eigen/Core>
#include <Eigen/Dense>

using std::vector;
using std::string;

class Geometry {
public:
	string type;

	//sphere
	Eigen::Vector3f center{0, 0, 0};
	float radius;

	//rectangle
	Eigen::Vector3f p1{0, 0, 0};
	Eigen::Vector3f p2{0, 0, 0};
	Eigen::Vector3f p3{0, 0, 0};
	Eigen::Vector3f p4{0, 0, 0};

	Eigen::Vector3f ac{0, 0, 0};
	Eigen::Vector3f dc{0, 0, 0};
	Eigen::Vector3f sc{0, 0, 0};

	float ka;
	float kd;
	float ks;

	float pc;

	Eigen::Matrix4f transform{};

	bool operator==(Geometry g);
};

class Light {
public:
	string type;
	
	//point
	Eigen::Vector3f center{0, 0, 0};
	
	//area
	Eigen::Vector3f p1{0, 0, 0};
	Eigen::Vector3f p2{0, 0, 0};
	Eigen::Vector3f p3{0, 0, 0};
	Eigen::Vector3f p4{0, 0, 0};

	Eigen::Vector3f id{0, 0, 0};
	Eigen::Vector3f is{0, 0, 0};

	Eigen::Matrix4f transform{};
	int n = 1;
	bool usecenter = false;
	bool use = true;
};

class Output {
public:
	string filename;
	Eigen::Vector2f size{0,0};
	Eigen::Vector3f lookat{0, 0, 0};
	Eigen::Vector3f up{0, 0, 0};
	float fov = 90;
	Eigen::Vector3f center{0, 0, 0};
	Eigen::Vector3f ai{0, 0, 0};
	Eigen::Vector3f bkc{0, 0, 0};

	std::vector<int> raysperpixel = {};
	bool antialiasing = false;
	bool twosiderendereing = true;
	bool globalillum = false;

};




class Scene {
public:
	Scene(nlohmann::json j);
	vector<Geometry> vGeometry;
	vector<Light> vLight;
	vector<Output> vOutput;
	bool isAreaLight = false;

};


#endif