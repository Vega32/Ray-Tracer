#include "RayTracer.h"
#include "Scene.h"
#include "Ray.h"
#include "../external/simpleppm.h"
#include <iostream>
#include <cmath>
#include <random>

using namespace std;

RayTracer::RayTracer(nlohmann::json j)
{
	RayTracer::jsonFile = j;
}

void RayTracer::run()
{
	cout << "TESTING RUN FUNCTION" << endl;
	Scene* scene = new Scene(RayTracer::jsonFile);
	for (auto output = scene->vOutput.begin(); output != scene->vOutput.end(); output++) {


		double height = output->size[1];
		double width = output->size[0];
		vector<double> buffer(3 * width * height);

		double delta = (2 * tan(output->fov / 2)) / height;
		Eigen::Vector3f r = output->lookat.cross(output->up);

		Eigen::Vector3f A = output->center + output->lookat;
		Eigen::Vector3f B = A + tan(output->fov / 2) * output->up;
		Eigen::Vector3f C = B - (width / 2) * delta * r;

		int maxPixelStrataX = 1;
		int maxPixelStrataY = 1;
		int rayPerStrata = 1;

		if (output->antialiasing) {
			if (output->raysperpixel.size() == 1) {
				rayPerStrata = output->raysperpixel.at(0);
			}
			else if (output->raysperpixel.size() == 2) {
				maxPixelStrataX = output->raysperpixel.at(0);
				maxPixelStrataY = output->raysperpixel.at(0);
				rayPerStrata = output->raysperpixel.at(1);
			}
			else if (output->raysperpixel.size() == 3) {
				maxPixelStrataX = output->raysperpixel.at(0);
				maxPixelStrataY = output->raysperpixel.at(1);
				rayPerStrata = output->raysperpixel.at(2);
			}
		}

		std::random_device rd;
		std::mt19937 g(rd());
		std::uniform_real_distribution<> rnd(0.0, 1.0);

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				Eigen::Vector3f color = { 0,0,0 };

				for (int pixelStrataX = 0; pixelStrataX < maxPixelStrataX; pixelStrataX++) {
					for (int pixelStrataY = 0; pixelStrataY < maxPixelStrataY; pixelStrataY++) {
						for (int strataRay = 0; strataRay < rayPerStrata; strataRay++) {
							Eigen::Vector3f pixelCenter;
							if (output->antialiasing == true) {
								pixelCenter = C + (i * delta + pixelStrataX * (delta / maxPixelStrataX) + rnd(g) * (delta / maxPixelStrataX)) * r - (j * delta + pixelStrataY * (delta / maxPixelStrataY) + rnd(g) * (delta / maxPixelStrataY)) * output->up;
							}
							else {
								pixelCenter = C + (i * delta + delta / 2) * r - (j * delta + delta / 2) * output->up;
							}

							Eigen::Vector3f rayDirection = (pixelCenter - output->center).normalized();
							Ray ray(output->center, rayDirection);
							vector<RayResult> intersections;
							vector<Geometry> intersectionsShapes;
							int minIndex = 0;

							for (auto shape = scene->vGeometry.begin(); shape != scene->vGeometry.end(); shape++) {

								RayResult result = ray.intersect(*shape);

								if (result.hit) {

									intersections.push_back(result);
									intersectionsShapes.push_back(*shape);

								}

							}

							if (intersections.size() == 0) {
								color += output->bkc;
							}
							else {


								for (int si = 0; si < intersections.size(); si++) {
									double currentMin = (output->center - intersections.at(minIndex).point).norm();
									double newMin = (output->center - intersections.at(si).point).norm();
									if (newMin < currentMin) {
										minIndex = si;
									}
								}
								// Color calculation

								vector<Eigen::Vector3f> lightDirections;
								vector<Light> visibleLightSource;
								bool lightBlocked = false;
								const double NEAR_ZERO = 0.0000001;

								//Ambiant Light
								color += output->ai.cwiseProduct(intersectionsShapes.at(minIndex).ac) * intersectionsShapes.at(minIndex).ka;

								// Iterate over each light source
								for (auto light = scene->vLight.begin(); light != scene->vLight.end(); light++) {
									if (!light->use) continue;
									lightBlocked = false;
									Eigen::Vector3f tempLightDirection;
									float tempLightDistance;
									if (light->type == "area" && light->usecenter == false) {

										Eigen::Vector3f areaLightColor = { 0,0,0 };

										for (int stratax = 0; stratax < light->n; stratax++) {
											for (int stratay = 0; stratay < light->n; stratay++) {
												lightBlocked = false;
												float u = (stratax + 0.5) / (float)light->n;
												float v = (stratay + 0.5) / (float)light->n;

												Eigen::Vector3f gridSectionCenter = light->p1 * (1 - u) * (1 - v) + light->p2 * u * (1 - v) + light->p3 * u * v + light->p4 * (1 - u) * v;

												tempLightDirection = gridSectionCenter - intersections.at(minIndex).point;

												tempLightDistance = tempLightDirection.norm();
												tempLightDirection = tempLightDirection.normalized();

												//Check if light is blocked
												for (auto shape = scene->vGeometry.begin(); shape != scene->vGeometry.end(); shape++) {

													Ray shadowRay(intersections.at(minIndex).point + intersections.at(minIndex).normal * NEAR_ZERO, tempLightDirection);
													RayResult result = shadowRay.intersect(*shape);

													if (result.hit && !(*shape == intersectionsShapes.at(minIndex)) && (result.point - intersections.at(minIndex).point).norm() < tempLightDistance) {

														lightBlocked = true;
														break;

													}

												}
												if (!lightBlocked) {
													Eigen::Vector3f normal = intersections.at(minIndex).normal;
													//Two side rendering
													Eigen::Vector3f L = tempLightDirection;
													if (normal.dot(L) < 0) {
														normal = -intersections.at(minIndex).normal;
													}

													//Diffuse Reflection
													areaLightColor += light->id.cwiseProduct(intersectionsShapes.at(minIndex).dc) * intersectionsShapes.at(minIndex).kd * max((normal.dot(L)), 0.0f);

													//Specular Reflection

													Eigen::Vector3f R = (2.0f * normal * (normal.dot(L)) - L).normalized();
													Eigen::Vector3f V = -rayDirection;
													Eigen::Vector3f H = (L + V).normalized();

													areaLightColor += light->is.cwiseProduct(intersectionsShapes.at(minIndex).sc) * intersectionsShapes.at(minIndex).ks * pow(max((normal.dot(H)), 0.0f), intersectionsShapes.at(minIndex).pc);
												}
											}
										}
										color += areaLightColor / (light->n * light->n);


									}
									else {
										if (light->type == "area" && light->usecenter == true) {
											tempLightDirection = (((light->p1 + light->p2 + light->p3 + light->p4) / 4) - intersections.at(minIndex).point);
										}
										else {
											tempLightDirection = (light->center - intersections.at(minIndex).point);
										}

										tempLightDistance = tempLightDirection.norm();
										tempLightDirection = tempLightDirection.normalized();

										//Check if light is blocked
										for (auto shape = scene->vGeometry.begin(); shape != scene->vGeometry.end(); shape++) {

											Ray shadowRay(intersections.at(minIndex).point + intersections.at(minIndex).normal * NEAR_ZERO, tempLightDirection);
											RayResult result = shadowRay.intersect(*shape);

											if (result.hit && !(*shape == intersectionsShapes.at(minIndex)) && (result.point - intersections.at(minIndex).point).norm() < tempLightDistance) {

												lightBlocked = true;
												break;

											}

										}
										if (!lightBlocked) {

											Eigen::Vector3f normal = intersections.at(minIndex).normal;
											//Two side rendering
											Eigen::Vector3f L = tempLightDirection;
											if (normal.dot(L) < 0) {
												normal = -intersections.at(minIndex).normal;
											}

											//Diffuse Reflection
											color += light->id.cwiseProduct(intersectionsShapes.at(minIndex).dc) * intersectionsShapes.at(minIndex).kd * max((normal.dot(L)), 0.0f);

											//Specular Reflection

											Eigen::Vector3f R = (2.0f * normal * (normal.dot(L)) - L).normalized();
											Eigen::Vector3f V = -rayDirection;
											Eigen::Vector3f H = (L + V).normalized();

											color += light->is.cwiseProduct(intersectionsShapes.at(minIndex).sc) * intersectionsShapes.at(minIndex).ks * pow(max((normal.dot(H)), 0.0f), intersectionsShapes.at(minIndex).pc);

										}
									}
								}
							}
						}
					}
				}
				color = color / (float)(rayPerStrata * maxPixelStrataX * maxPixelStrataY);
				color = color.cwiseMax(0.0f).cwiseMin(1.0f);
				buffer[3 * j * width + 3 * i + 0] = (color[0]);
				buffer[3 * j * width + 3 * i + 1] = (color[1]);
				buffer[3 * j * width + 3 * i + 2] = (color[2]);
			}
		}
		save_ppm(output->filename, buffer, width, height);
	}
}
