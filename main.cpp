#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "ObjModel.h"

std::vector<Shader*> shaders;

Shader* defaultDraw;
std::vector<ObjModel*> models;
std::vector<float> distances;

int activeModel = 0;

int currentShader;

GLFWwindow* window;
glm::ivec2 screenSize;
float rotation;
double lastTime;

void checkShaderErrors(GLuint shaderId)
{
	GLint status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);					//kijk of het compileren is gelukt
	if (status == GL_FALSE)
	{
		int length, charsWritten;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);				//haal de lengte van de foutmelding op
		char* infolog = new char[length + 1];
		memset(infolog, 0, length + 1);
		glGetShaderInfoLog(shaderId, length, &charsWritten, infolog);		//en haal de foutmelding zelf op
		std::cout << "Error compiling shader:\n" << infolog << std::endl;
		delete[] infolog;
	}
}

#ifdef _WIN32
void GLAPIENTRY onDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
#else
void onDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
#endif
{
	std::cout << message << std::endl;
}
static void glfw_error_callback(int error, const char* description)
{
	std::cerr << "Glfw Error" << error << " -> " << description << std::endl;
}

void init()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClearColor(.5f, .5f, .5f, 1.0f);

	currentShader = 0;
	shaders.push_back(new Shader("assets/shaders/fur"));
	shaders.push_back(new Shader("assets/shaders/simple"));
	shaders.push_back(new Shader("assets/shaders/johanLava"));
	defaultDraw = new Shader("assets/shaders/defaultDraw");

	models.push_back(new ObjModel("assets/models/sphere3/sphere3.obj"));
	distances.push_back(.5);
	models.push_back(new ObjModel("assets/models/rabbit/rabbit.obj"));
	distances.push_back(10);
	models.push_back(new ObjModel("assets/models/sphere/sphere.obj"));
	distances.push_back(2);
	models.push_back(new ObjModel("assets/models/plane/plane.obj"));
	distances.push_back(2);
	models.push_back(new ObjModel("assets/models/alpaca/alpaca2.obj"));
	distances.push_back(10);
	

	glEnableVertexAttribArray(0);							// positie
	glEnableVertexAttribArray(1);							// texcoord
	glEnableVertexAttribArray(2);							// normal
	glEnableVertexAttribArray(3);							// tangent

	if (glDebugMessageCallback)
	{
		glDebugMessageCallback(&onDebug, NULL);
		glEnable(GL_DEBUG_OUTPUT);
	}

	rotation = 0;
	lastTime = glfwGetTime();
}

void display()
{
	glfwGetWindowSize(window, &screenSize.x, &screenSize.y);
	glViewport(0, 0, screenSize.x, screenSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* shader = shaders[currentShader];

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), screenSize.x / (float)screenSize.y, 0.01f, 200.0f);		//begin met een perspective matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0, -2, distances[activeModel]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));					//vermenigvuldig met een lookat
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(0, 0, -1));													//of verplaats de camera gewoon naar achter
	model = glm::rotate(model, rotation, glm::vec3(0, 1, 0));											//roteer het object een beetje

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

	shader->use();

	glUniformMatrix4fv(shader->getUniform("modelMatrix"), 1, 0, glm::value_ptr(model));								//en zet de matrix in opengl
	glUniformMatrix4fv(shader->getUniform("viewMatrix"), 1, 0, glm::value_ptr(view));								//en zet de matrix in opengl
	glUniformMatrix4fv(shader->getUniform("projectionMatrix"), 1, 0, glm::value_ptr(projection));								//en zet de matrix in opengl
	glUniformMatrix3fv(shader->getUniform("normalMatrix"), 1, 0, glm::value_ptr(normalMatrix));								//en zet de matrix in opengl
	glUniform1f(shader->getUniform("time"), (float)glfwGetTime());
	glUniform1i(shader->getUniform("s_texture"), 0);

	models[activeModel]->draw();

	//defaultDraw->use();
	//glUniformMatrix4fv(defaultDraw->getUniform("modelMatrix"), 1, 0, glm::value_ptr(model));								//en zet de matrix in opengl
	//glUniformMatrix4fv(defaultDraw->getUniform("viewMatrix"), 1, 0, glm::value_ptr(view));								//en zet de matrix in opengl
	//glUniformMatrix4fv(defaultDraw->getUniform("projectionMatrix"), 1, 0, glm::value_ptr(projection));								//en zet de matrix in opengl
	//glUniform1i(defaultDraw->getUniform("s_texture"), 0);

	//models[activeModel]->draw();
	//en tekenen :)

	glfwSwapBuffers(window);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
		if (key == '[')
		{
			currentShader = (currentShader + shaders.size() - 1) % (int)shaders.size();
			std::cout << "Shader " << currentShader << std::endl;
		}
		if (key == ']')
		{
			currentShader = (currentShader + 1) % shaders.size();
			std::cout << "Shader " << currentShader << std::endl;
		}
		if (key == ',' && activeModel - 1 >= 0) 
		{
			
			activeModel = (activeModel - 1);
			std::cout << "activemodel: " << activeModel << "size: " << models.size() << "\n";
		}
		if (key == '.' && activeModel + 1 < models.size()) 
		{
			
			activeModel = (activeModel + 1);
			std::cout << "activemodel: " << activeModel << "size: " << models.size() << "\n";
		}
	}
}

void update()
{
	double time = glfwGetTime();
	double elapsed = time - lastTime;
	
	rotation += (float)elapsed / 4;


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);



	lastTime = time;
}

int main(int argc, char* argv[])
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return false;
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1280, 1024, "help", NULL, NULL);

	if (window == nullptr)
		return false;
	glfwSetKeyCallback(window, keyboard);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	init();
	
	while (true)
	{
		if (glfwWindowShouldClose(window))
			break;
		glfwPollEvents();

		update();
		display();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}