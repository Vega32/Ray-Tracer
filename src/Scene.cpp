#include "Scene.h"
using namespace std;
using namespace nlohmann;

Scene::Scene(json j)
{
    for (auto itr = j["geometry"].begin(); itr != j["geometry"].end(); itr++) {
        Geometry* geometry = new Geometry();

        geometry->type = (*itr)["type"].get<std::string>();

        if (geometry->type == "sphere") {
            int i = 0;
            //Center
            for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
                geometry->center[i++] = (*itr2).get<float>();
            }
            //Radius
            geometry->radius = (*itr)["radius"].get<float>();

        }
        else if (geometry->type == "rectangle") {
            int i = 0;
            //Points 1->4
            for (auto itr2 = (*itr)["p1"].begin(); itr2 != (*itr)["p1"].end(); itr2++) {
                geometry->p1[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p2"].begin(); itr2 != (*itr)["p2"].end(); itr2++) {
                geometry->p2[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p3"].begin(); itr2 != (*itr)["p3"].end(); itr2++) {
                geometry->p3[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p4"].begin(); itr2 != (*itr)["p4"].end(); itr2++) {
                geometry->p4[i++] = (*itr2).get<float>();
            }
        }

        // ac, dc, sc
        int i = 0;
        for (auto itr2 = (*itr)["ac"].begin(); itr2 != (*itr)["ac"].end(); itr2++) {
            geometry->ac[i++] = (*itr2).get<float>();
        }
        i = 0;
        for (auto itr2 = (*itr)["dc"].begin(); itr2 != (*itr)["dc"].end(); itr2++) {
            geometry->dc[i++] = (*itr2).get<float>();
        }
        i = 0;
        for (auto itr2 = (*itr)["sc"].begin(); itr2 != (*itr)["sc"].end(); itr2++) {
            geometry->sc[i++] = (*itr2).get<float>();
        }
        
        //ka, kd, ks, pc
        geometry->ka= (*itr)["ka"].get<float>();

        geometry->kd = (*itr)["kd"].get<float>();

        geometry->ks = (*itr)["ks"].get<float>();

        geometry->pc = (*itr)["pc"].get<float>();

        //splitting rectangles into two triangles
        if (geometry->type == "rectangle") {
            Geometry tri1 = *geometry;
            Geometry tri2 = *geometry;

            tri1.type = "triangle";
            // Triangle 1: p1, p2, p3
            vGeometry.push_back(tri1);

            tri2.type = "triangle";
            // Triangle 2: p1, p3, p4
            tri2.p2 = tri2.p3;
            tri2.p3 = tri2.p4;
            vGeometry.push_back(tri2);
        }
        else {
            vGeometry.push_back(*geometry);
        }
    }

    for (auto itr = j["light"].begin(); itr != j["light"].end(); itr++) {
        Light* light = new Light();

        light->type = (*itr)["type"].get<std::string>();

        if (light->type == "point") {
            int i = 0;
            //Center
            for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
                light->center[i++] = (*itr2).get<float>();
            }
        }
        else if(light->type == "area") {
            this->isAreaLight = true;
            int i = 0;
            //Points 1->4
            for (auto itr2 = (*itr)["p1"].begin(); itr2 != (*itr)["p1"].end(); itr2++) {
                light->p1[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p2"].begin(); itr2 != (*itr)["p2"].end(); itr2++) {
                light->p2[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p3"].begin(); itr2 != (*itr)["p3"].end(); itr2++) {
                light->p3[i++] = (*itr2).get<float>();
            }
            i = 0;
            for (auto itr2 = (*itr)["p4"].begin(); itr2 != (*itr)["p4"].end(); itr2++) {
                light->p4[i++] = (*itr2).get<float>();
            }
            if ((*itr).contains("n")) light->n = (*itr)["n"].get<float>();
            if ((*itr).contains("usecenter") && (*itr)["usecenter"] == true) light->usecenter = true;
        }
        //id, is
        int i = 0;
        for (auto itr2 = (*itr)["id"].begin(); itr2 != (*itr)["id"].end(); itr2++) {
            light->id[i++] = (*itr2).get<float>();
        }
        i = 0;
        for (auto itr2 = (*itr)["is"].begin(); itr2 != (*itr)["is"].end(); itr2++) {
            light->is[i++] = (*itr2).get<float>();
        }

        if ((*itr).contains("use") && (*itr)["use"] == false) light->use = false;

        vLight.push_back(*light);
    }

    for (auto itr = j["output"].begin(); itr != j["output"].end(); itr++) {
        Output* output = new Output();
        //filename
        output->filename = (*itr)["filename"].get<std::string>();
        //size
        int i = 0;
        for (auto itr2 = (*itr)["size"].begin(); itr2 != (*itr)["size"].end(); itr2++) {
            output->size[i++] = (*itr2).get<float>();
        }
        //lookat
        i = 0;
        for (auto itr2 = (*itr)["lookat"].begin(); itr2 != (*itr)["lookat"].end(); itr2++) {
            output->lookat[i++] = (*itr2).get<float>();
        }
        //up
        i = 0;
        for (auto itr2 = (*itr)["up"].begin(); itr2 != (*itr)["up"].end(); itr2++) {
            output->up[i++] = (*itr2).get<float>();
        }
        //fov
        output->fov = (*itr)["fov"].get<float>()*(2*acos(0.0))/180.0;
        //center
        i = 0;
        for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
            output->center[i++] = (*itr2).get<float>();
        }
        //ai
        i = 0;
        for (auto itr2 = (*itr)["ai"].begin(); itr2 != (*itr)["ai"].end(); itr2++) {
            output->ai[i++] = (*itr2).get<float>();
        }
        //bkc
        i = 0;
        for (auto itr2 = (*itr)["bkc"].begin(); itr2 != (*itr)["bkc"].end(); itr2++) {
            output->bkc[i++] = (*itr2).get<float>();
        }

        if ((*itr).contains("raysperpixel")) {
            for (auto itr2 = (*itr)["raysperpixel"].begin(); itr2 != (*itr)["raysperpixel"].end(); itr2++) {
                output->raysperpixel.push_back(*itr2);
            }
        }

        if ((*itr).contains("antialiasing") && (*itr)["antialiasing"] == true && this->isAreaLight == false) output->antialiasing = true;

        vOutput.push_back(*output);
    }
}

bool Geometry::operator==(Geometry g)
{
    if (g.type != this->type) return false;

    if (g.type == "sphere") {
        return g.center == this->center && g.radius == this->radius;
    }
    else {
        return g.p1 == this->p1 && g.p2 == this->p2 && g.p3 == this->p3 && g.p4 == this->p4;
    }
}
