#pragma once

#include <vector>
#include <glm/gtx/intersect.hpp>
#include "ofApp.h"

// By: Aramina Lee

using namespace std;

class Mesh : public SceneObject
{
public:
	vector<glm::vec3> verts;		// world position of vertices
	vector<glm::ivec3> tInd;		// triangle vertex indices
	vector<glm::vec3> tCentroid;	// world position of triangle centroids
	vector<glm::vec3> tNormal;		// unit vectors of triangle normals

	// p is world position, meshFile = name of meshFile or NULL
	Mesh(glm::vec3 p, const char* meshFile, ofColor diffuse);

	void loadMesh();	// load simple pyramid if no meshfile
	void loadFile(const char* fname);	// load mesh file
	void printStats();					// print mesh statistics
	void calcNormal();					// calculate normal of every triangle
	void draw();						// draw mesh
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);	// determine if ray intersects mesh
};
