//  General purpose plane 
//
class Plane: public SceneObject {
public:
	Plane(glm::vec3 p, glm::vec3 n, ofColor diffuse = ofColor::green, float w = 20, float h = 20 ) {
		position = p; normal = n;
		width = w;
		height = h;
		diffuseColor = diffuse;
		if (normal == glm::vec3(0, 1, 0))
			plane.rotateDeg(-90, 1, 0, 0);
		else if (normal == glm::vec3(0, -1, 0))
			plane.rotateDeg(90, 1, 0, 0);
		else if (normal == glm::vec3(1, 0, 0))
			plane.rotateDeg(90, 0, 1, 0);
		else if (normal == glm::vec3(-1, 0, 0))
			plane.rotateDeg(-90, 0, 1, 0);
	}
	Plane() { 
		normal = glm::vec3(0, 1, 0);
		plane.rotateDeg(90, 1, 0, 0);
		isSelectable = false;
	}

	bool intersect(const Ray &ray, glm::vec3 & point, glm::vec3 & normal);
	float sdf(const glm::vec3 & p);
	glm::vec3 getNormal(const glm::vec3 &p) { return this->normal; }
	void draw() {
		plane.setPosition(position);
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setResolution(4, 4);
	//	plane.drawWireframe();
		plane.draw();
	}
	ofPlanePrimitive plane;
	glm::vec3 normal;
	
	float width = 20;
	float height = 20;

};
