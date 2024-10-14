#include "ofApp.h"
#include "mesh.h"

/*
 * Intersect Ray with Plane  (wrapper on glm::intersect)
 *
 * @param const Ray& ray - given ray
 * @param glm::vec3& point - point of intersection
 * @param glm::vec3& - normal Intersection
 * @return Plane - 3D Plane
 */
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normalAtIntersect) {
	float dist;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit) {
		Ray r = ray;
		point = r.evalPoint(dist);
	}
	return (hit);
}

// Convert (u, v) to (x, y, z)
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v) {
	float w = width();
	float h = height();
	return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v) {
	glm::vec3 pointOnPlane = view.toWorld(u, v);
	return(Ray(position, glm::normalize(pointOnPlane - position)));
}

// This could be drawn a lot simpler but I wanted to use the getRay call
// to test it at the corners.
// 
void RenderCam::drawFrustum() {
	Ray r1 = getRay(0, 0);
	Ray r2 = getRay(0, 1);
	Ray r3 = getRay(1, 1);
	Ray r4 = getRay(1, 0);
	float dist = glm::length((view.toWorld(0, 0) - position));
	r1.draw(dist);
	r2.draw(dist);
	r3.draw(dist);
	r4.draw(dist);
}

// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(glm::vec3 position) {

	ofPushMatrix();
	ofTranslate(position);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::drawGrid() {

	float u = 0;
	float v = 0;
	float pixelWidth = 1.0 / imageWidth;
	float pixelHeight = 1.0 / imageHeight;
	for (int x = 0; x < imageWidth; x++) {
		glm::vec3 p1 = renderCam.view.toWorld(u, 0);
		glm::vec3 p2 = renderCam.view.toWorld(u, 1);
		ofDrawLine(p1, p2);
		u += pixelWidth;
	}
	for (int y = 0; y < imageHeight; y++) {
		glm::vec3 p1 = renderCam.view.toWorld(0, v);
		glm::vec3 p2 = renderCam.view.toWorld(1, v);
		ofDrawLine(p1, p2);
		v += pixelHeight;
	}
}

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetBackgroundColor(ofColor::black);
	ofEnableDepthTest();
	mainCam.setDistance(15);
	mainCam.setNearClip(.1);
	sideCam.setPosition(25, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));

	topCam.setPosition(0, 25, 0);
	topCam.lookAt(glm::vec3(0, -1, 0));

	previewCam.setPosition(renderCam.position);
	previewCam.setNearClip(.1);
	previewCam.lookAt(glm::vec3(0, 0, 0));
	theCam = &mainCam;

	gui.setup(); // most of the time you don't need a name
	gui.add(intensity.setup("intensity", 0.5, 0, 1));
	gui.add(power.setup("power", 10, 10, 10000));

	// add 3 spheres and a pyramidal mesh
	scene.push_back(new Sphere(glm::vec3(-2, -2, 3), 0.6, ofColor::lightSeaGreen));
	scene.push_back(new Sphere(glm::vec3(-1, -1, 0), 1.5, ofColor::ivory));
	scene.push_back(new Sphere(glm::vec3(1, 1, -7), 3, ofColor::rosyBrown));
	scene.push_back(new Mesh(glm::vec3(-5, -1, -5), NULL, ofColor::darkBlue));
	// ground plane
	//
	scene.push_back(new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0), ofColor::brown));
	
	// vertical plane
	scene.push_back(new Plane(glm::vec3(0, -2, -10), glm::vec3(0, 0, 1), ofColor::greenYellow));

	image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	imageFile = "3_spheres_pyramid.png";

	// add 3 point light sources; light intensities are overridden by ofxFloatSlider
	lights.push_back(new Light(glm::vec3(0, 10, 0), 0.5));
	lights.push_back(new Light(glm::vec3(5, 10, 5), 0.5));
	lights.push_back(new Light(glm::vec3(-7, 10, -7), 0.5));
	ambientIntensity = 0.10;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	theCam->begin();

	ofSetColor(ofColor::green);
	ofNoFill();

	drawAxis(glm::vec3(0, 0, 0));

	//  draw objects in scene
	//

	ofSetColor(ofColor::green);
	ofNoFill();

	//  draw objects in scene
	//
	for (int i = 0; i < scene.size(); i++) {
		ofSetColor(scene[i]->diffuseColor);
		scene[i]->draw();
	}
	
	// draw light sources in scene
	for (int i = 0; i < lights.size(); i++) {
		ofSetColor(lights[i]->diffuseColor);
		lights[i]->draw();
	}

	ofDisableLighting();
	ofSetColor(ofColor::lightSkyBlue);
	renderCam.drawFrustum();
	renderCam.view.draw();
	ofSetColor(ofColor::blue);
	renderCam.draw();

	theCam->end();

	ofDisableDepthTest();
	/*if (!bHide)
		ofEnableDepthTest();
	else
		ofDisableDepthTest();*/
	gui.draw();
}

// draw image in ViewFinder using Lambertian Shading, Phong Shading, and ambient lighting
void ofApp::drawImage()
{
	// for each pixel in image
	float u = 0;
	float v = 0;
	float pixelWidth = 1.0 / imageWidth;
	float pixelHeight = 1.0 / imageHeight;
	for (size_t x = 0; x < imageWidth; x++, u += pixelWidth) {
		v = 0;
		for (size_t y = 0; y < imageHeight; y++, v += pixelHeight) {
			ofColor L = ofColor::black;

			// translate (u,v) position to 3D world position
			glm::vec3 pixelPos = renderCam.view.toWorld(u, v);
			
			// create ray from camera position to image pixel position
			Ray cameraToImage = renderCam.getRay(u, v);

			// find intersection point and normal of closest object to camera/image
			glm::vec3 intersectPos;
			glm::vec3 intersectNorm;
			SceneObject* intersectScene = NULL;
			if ((intersectScene = findIntersection(cameraToImage, intersectPos, intersectNorm)) != NULL)
			{
				// Calculate lambert shading
				L = L + lambert(intersectPos, intersectNorm, intersectScene->diffuseColor);
				
				// Calculate Phong shading
				L = L + phong(intersectPos, intersectNorm, intersectScene->diffuseColor, intersectScene->specularColor, power);

				// Calculate the ambient shading, set ambient color same as diffuse
				ofColor ambientCoef = intersectScene->diffuseColor;
				ofColor La = ambientCoef * ambientIntensity;	// La = ambient coefficient * Ia
				L = L + La;

				// Calculate distance from image to intersection point
				glm::vec3 pixelToIntersect = intersectPos - pixelPos;
				float dist2 = glm::dot(pixelToIntersect, pixelToIntersect);

				// Scale the combined shading intensity by distance
				//L = L / dist2 * 20;
			}
			// Store in image pixel
			image.setColor(x, imageHeight - y - 1, L);		// invert image
		}
	}
	// Save image to file
	image.update();
	//image.draw(0,0,0);

	cout << "Save image " << imageFile;
	bool saved = image.save(imageFile);
	if (saved)
		cout << "... done" << endl;
	else
		cout << " failed" << endl;
}

// Returns pointer to closest object that ray intersects among all SceneObjects in vector scenes
// Also outputs the position and normal of the intersection
// Returns NULL if no object intersects ray
SceneObject* ofApp::findIntersection(const Ray& ray, glm::vec3& intersectPos, glm::vec3& intersectNorm)
{
	SceneObject* intersectScene = NULL;
	float minDist2 = std::numeric_limits<float>::max();
	glm::vec3 p;
	glm::vec3 n;
	// for each object in the scene
	for (int i = 0; i < scene.size(); i++) {
		// if ray intersects object
		if (scene[i]->intersect(ray, p, n))
		{
			// calculate squared distance from ray to intersection point
			glm::vec3 rayToPoint = p - ray.p;
			float dist2 = glm::dot(rayToPoint, rayToPoint);
			// if distance is less than current closest object's distance
			if (dist2 < minDist2)
			{
				// save the new closest object, intersection position, and normal
				minDist2 = dist2;
				intersectScene = scene[i];
				intersectPos = p;
				intersectNorm = n;
			}
		}
	}
	return intersectScene;
}

// Check if there is any other object in scene between two pos1 and pos2,
// Return true if there is a clear line of sight (i.e., no object) between pos1 and pos2
bool ofApp::isClearLineOfSight(const glm::vec3& pos1, const glm::vec3& pos2)
{
	// Calculate square distance from pos1 to pos2
	glm::vec3 posDiff = pos2 - pos1;
	float posDist2 = glm::dot(posDiff, posDiff);

	// Create ray from pos1 to pos2
	Ray ray = Ray(pos1, glm::normalize(pos2 - pos1));
	glm::vec3 p;
	glm::vec3 n;

	// for each object in the scene
	for (int i = 0; i < scene.size(); i++) {
		// if ray intersects object
		if (scene[i]->intersect(ray, p, n))
		{
			// Calculate square distance from pos1 to intersection point
			glm::vec3 diff = p - pos1;
			float dist2 = glm::dot(diff, diff);

			// If distance from pos1 to intersection point to less than distance from pos1 to pos2
			if (dist2 < posDist2)
				return false; // then object is between pos1 and pos2, so line of sight is blocked
		}
	}
	return true; // no object is between pos1 and pos2
}

// Lambertian Shading: L = d * (I/r^2) * max(0, n dot l)
ofColor ofApp::lambert(const glm::vec3& p, const glm::vec3& norm, const ofColor diffuse)
{
	ofColor L = ofColor::black;
	// move intersection point a tiny distance from the surface
	glm::vec3 pos = p + norm * 0.01;
	for (int i = 0; i < lights.size(); i++)
	{
		// if closest object to camera/image is not blocked from light source
		if (isClearLineOfSight(lights[i]->position, pos))
		{
			glm::vec3 object2Light = lights[i]->position - pos;
			// create ray from intersection point to light source
			Ray lightRay = Ray(pos, glm::normalize(object2Light));
			// lightRay direction and intersectNorm should both be unit length

			// calculate dot product between light ray and normal
			float diffuseDot = glm::dot(lightRay.d, norm);
			if (diffuseDot < 0)
				diffuseDot = 0;

			// calculate the Lambertian shading
			ofColor Ld = diffuse * diffuseDot * intensity;
			// Scale by distance from light source to intersection point
			//cout << "dist2 " << glm::dot(object2Light, object2Light) << endl;
			Ld = Ld / (0.01 * glm::dot(object2Light, object2Light));
			L = L + Ld;
		}
	}
	return L;
}

// Phong Shading: s * (I/r^2) * max(0, n dot h) ^ power
ofColor ofApp::phong(const glm::vec3& p, const glm::vec3& norm, const ofColor diffuse,
	const ofColor specular, float power)
{
	ofColor L = ofColor::black;
	// move intersection point a tiny distance from the surface
	glm::vec3 pos = p + norm * 0.01;
	for (int i = 0; i < lights.size(); i++)
	{
		// if closest object to camera/image is not blocked from light source
		if (isClearLineOfSight(lights[i]->position, pos))
		{
			glm::vec3 object2Light = lights[i]->position - pos;
			// create ray from intersection point to light source
			Ray lightRay = Ray(pos, glm::normalize(object2Light));
			// lightRay direction and intersectNorm should both be unit length

			// create ray from intersection point to view point
			Ray viewRay = Ray(pos, glm::normalize(renderCam.position - pos));
			// create bisector between light ray and view ray
			Ray halfRay = Ray(pos, glm::normalize(viewRay.d + lightRay.d));


			// calculate dot product between normal and half ray
			float specularDot = glm::dot(halfRay.d, norm);
			if (specularDot < 0)
				specularDot = 0;

			// calculate the Phong shading
			ofColor Ls = specular * glm::pow(specularDot, power) * intensity;
			// Scale by distance from light source to intersection point
			//cout << "dist2 " << glm::dot(object2Light, object2Light) << endl;
			Ls = Ls / (0.01 * glm::dot(object2Light, object2Light));
			L = L + Ls;
			
		}
	}
	return L;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}


void ofApp::keyReleased(int key) {
	switch (key) {
	case 'C':
	case 'c':
		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
		else mainCam.enableMouseInput();
		break;
	case 'F':
	case 'b':
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		drawImage();
		break;
	case 'n':
		scene.push_back(new Sphere(glm::vec3(0, 0, 0), 1.0, ofColor::violet));
		break;
	case 'r':
		rayTrace();
		cout << "done..." << endl;
		break;
	case 'm':
		break;
	case OF_KEY_F1:
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &previewCam;
		break;
	case OF_KEY_F4:
		theCam = &topCam;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
