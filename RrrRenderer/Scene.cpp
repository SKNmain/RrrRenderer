#include "Scene.h"
#include "Sphere.h"
#include "DirectionLight.h"
#include "PointLight.h"

#include <random>
#include "Helpers/FunctionHelper.h"

Scene::Scene()
{
   this->allObjectsPositionMatrix.insert_cols(0, arma::dvec4{ 0,0,0,0 });
   Object_A::setAllObjectsPositionMatrix(&this->allObjectsPositionMatrix);
}

void Scene::addObjectToScene(ObjectUptr object)
{
   this->objects.push_back(std::move(object));
}

std::vector<ObjectUptr>& Scene::getObjects()
{
   return this->objects;
}

std::vector<LightUptr>& Scene::getLights()
{
   return this->lights;
}

void Scene::createSphere(const arma::vec3& position, float radius, const Material* material, RrrColor::RGBA color)
{
   int positionIndex = this->allObjectsPositionMatrix.n_cols;
   this->allObjectsPositionMatrix.insert_cols(positionIndex, arma::dvec4{ position.x(), position.y(), position.z(), 1 });

   ObjectUptr sphere(new Sphere(positionIndex, radius, color, material));
   this->objects.push_back(std::move(sphere));

   this->objsCounter.spheres++;
}

void Scene::createDirectionalLight(const arma::vec3& direction, float intensity, const RrrColor::RGBA& color)
{
   LightUptr dirLight(new DirectionLight(direction, intensity, color));
   this->lights.push_back(std::move(dirLight));

   this->objsCounter.distantLights++;
}

void Scene::createPointLight(const arma::vec3& position, float intensity, const RrrColor::RGBA& color)
{
   LightUptr pLight(new PointLight(position, intensity, color));
   this->lights.push_back(std::move(pLight));

   this->objsCounter.pointLights++;
}

void Scene::transformWorld(const arma::dmat44& transformMatrix)
{
   this->allObjectsPositionMatrix = transformMatrix * this->allObjectsPositionMatrix;
}

void Scene::pitch(float angle)
{
   arma::dmat44 pitchMatrix{
                           {cos(rrr::deg2rad(angle)), 0, sin(rrr::deg2rad(angle)), 0},
                                {0, 1, 0, 0},
                                {-sin(rrr::deg2rad(angle)), 0, cos(rrr::deg2rad(angle)), 0},
                                {0,0,0,1} };
   this->transformWorld(pitchMatrix);
}

void Scene::yaw(float angle)
{
   arma::dmat44 yawMatrix{ 
                          {cos(rrr::deg2rad(angle)), sin(rrr::deg2rad(angle)), 0, 0},
                               {-sin(rrr::deg2rad(angle)), cos(rrr::deg2rad(angle)), 0, 0},
                               {0,0,1,0},
                               {0,0,0,1} };
   this->transformWorld(yawMatrix);
}

void Scene::roll(float angle)
{
   arma::dmat44 rollMatrix{
                             {1,0,0,0},
                             {0, cos(rrr::deg2rad(angle)), sin(rrr::deg2rad(angle)), 0},
                             {0, -sin(rrr::deg2rad(angle)), cos(rrr::deg2rad(angle)), 0},
                             {0,0,0,1} };
   this->transformWorld(rollMatrix);
}

void Scene::translate(const arma::vec3 transformVec)
{
   arma::dmat44 trans{
                      {1, 0, 0, transformVec.x()},
                           {0, 1, 0, transformVec.y()},
                           {0, 0, 1, transformVec.z()},
                           {0, 0, 0, 1} };
   this->transformWorld(trans);

}

std::ostream& operator<<(std::ostream& os, Scene& scene)
{
   os << "========SCENE========" << std::endl;
   os << "Spheres       " << scene.objsCounter.spheres << std::endl;
   os << "DistantLights " << scene.objsCounter.distantLights << std::endl;
   os << "PointLights   " << scene.objsCounter.pointLights << std::endl;
   os << std::endl;

   os << "========LIGHTS========" << std::endl;
   for(const auto& l : scene.lights)
   {
      os << *(l.get()) << std::endl;
   }

   os << "========OBJECTS========" << std::endl;
   for (const auto& o : scene.objects)
   {
      os << *(o.get()) << std::endl;
   }

   return os;
}
