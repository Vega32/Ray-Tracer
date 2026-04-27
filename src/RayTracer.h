#define RAYTRACER_H

#include "../external/json.hpp"
#include <iostream>

class RayTracer {
public:
	RayTracer(nlohmann::json j);
	nlohmann::json jsonFile;
	void run();

};