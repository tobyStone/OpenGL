#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Sphere.h"
#include "Utils.h"
using namespace std;

#define numVAOs 1
#define numVBOs 6

static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;
float earthLocX, earthLocY, earthLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint sunTexture;
GLuint earthTexture;
float rotAmt = 0.0f;


//camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


//time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
stack<glm::mat4> mvStack;

Sphere mySphere = Sphere(48);

//sun sphere set-up

void setupVertices(void) {
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

	Sphere earthSphere = Sphere(48);

	//earth sphere

	void setupEarthVertices(void) {
		std::vector<int> ind = earthSphere.getIndices();
		std::vector<glm::vec3> vert = earthSphere.getVertices();
		std::vector<glm::vec2> tex = earthSphere.getTexCoords();
		std::vector<glm::vec3> norm = earthSphere.getNormals();

		std::vector<float> pvalues;
		std::vector<float> tvalues;
		std::vector<float> nvalues;

		int numIndices = mySphere.getNumIndices();
		for (int i = 0; i < numIndices; i++) {
			pvalues.push_back((vert[ind[i]]).x);
			pvalues.push_back((vert[ind[i]]).y);
			pvalues.push_back((vert[ind[i]]).z);
			tvalues.push_back((tex[ind[i]]).s);
			tvalues.push_back((tex[ind[i]]).t);
			nvalues.push_back((norm[ind[i]]).x);
			nvalues.push_back((norm[ind[i]]).y);
			nvalues.push_back((norm[ind[i]]).z);
		}
	

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window, double currentTime) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 2.0f; cameraY = 2.0f; cameraZ = 7.0f;
	/*glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	*/
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = -1.0f;
	earthLocX = 7.0f; earthLocY = 0.0f; earthLocZ = -20.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	sunTexture = Utils::loadTexture("sunmap.jpg");
	earthTexture = Utils::loadTexture("earth.jpg");
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	float radius = 15.0f;
	float camZ = sin(glfwGetTime()) * radius * cameraZ;
	float camX = cos(glfwGetTime()) * radius * cameraX;
	float camY = sin(glfwGetTime()) * radius * cameraY;
	//vMat = glm::lookAt(glm::vec3(camX, camY, cameraZ), glm::vec3(sphLocX, sphLocY, sphLocZ), glm::vec3(0.0f, 1.0f, 0.0f));
	vMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	mvStack.push(vMat);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// ----------------------  sun motion
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sphLocX, sphLocY, sphLocZ));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.5, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(1.25f, 1.25f, 1.25f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	
//------------------------- earth motion

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, earthSphere.getNumIndices());
	mvStack.pop();
	mvStack.pop(); mvStack.pop(); mvStack.pop();

}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}


void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;



}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter6 - program1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window, glfwGetTime());

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


static void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos)
{
	std::cout << xPos << " : " << yPos << std::endl;

}

