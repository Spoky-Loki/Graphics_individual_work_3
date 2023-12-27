#include "Header.h"
#include "Shader.h"
#include "Model.h"

// Camera
vec3 cameraPos = vec3(0.0f, 10.0f, 19.0f);
vec3 cameraFront = vec3(0.0f, -0.3f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
GLfloat Yaw = -90.0f;
GLfloat Pitch = 0.0f;
GLfloat lastX = Width / 2.0;
GLfloat lastY = Height / 2.0;
bool firstMouse = true;

// Light
glm::vec3 lightPos(-25.0f, 15.0f, 0.0f);
glm::vec3 lightDir(1.0f, -0.3f, 0.0f);

float moveX = 0;
float moveY = 0;
float moveZ = 0;

const char* VertexShader = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0f));
    TexCoord = texCoord;
	Normal = mat3(transpose(inverse(model))) * normal;
}
)";

const char* FragShader = R"(
#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture0;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirect;
uniform vec3 viewPos;

float specularStrength = 0.5f;

out vec4 color;
void main() {
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-lightDirect);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (0.2 + diffuse + specular) * vec3(texture(texture0, TexCoord));

	color = vec4(result, 1.0f);
}
)";

const char* VertexShaderLight = R"(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
}
)";

const char* FragShaderLight = R"(
#version 330 core
out vec4 color;

void main()
{
    color = vec4(1.0f);
}
)";

const char* VertexShaderRotate = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float rotationY;

void main() {
	mat4 positionV = mat4(
            cos(rotationY),  0, sin(rotationY), 0,
            0,               1, 0,              0,
            -sin(rotationY), 0, cos(rotationY), 0,
            0,               0, 0,              1) 
        * model;
	gl_Position = projection * view * positionV * vec4(position, 1.0);
	FragPos = vec3(positionV * vec4(position, 1.0f));
    TexCoord = texCoord;
	Normal = mat3(transpose(inverse(positionV))) * normal;
    TexCoord = texCoord;
}
)";

const char* VertexShaderInstance = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in mat4 InstanceMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * InstanceMatrix * vec4(position, 1.0);
    FragPos = vec3(InstanceMatrix * vec4(position, 1.0f));
    TexCoord = texCoord;
	Normal = mat3(transpose(inverse(InstanceMatrix))) * normal;
    TexCoord = texCoord;
}
)";

void SetTreeTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetGroundTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(60.0f, 30.0f, 40.0f));
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetAirBalloonTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	model = glm::translate(model, glm::vec3(0.0f + moveX, 3500.0f + moveY, 0.0f + moveZ));
	model = glm::rotate(model, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetSleighTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	model = glm::translate(model, glm::vec3(0.0f, 400.0f, 2000.0f));
	model = glm::rotate(model, radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetHouseTargetTranslation(GLuint Program, Model house)
{
	unsigned int amount = 5;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(static_cast<unsigned int>(glfwGetTime()));
	float radius = 5.0;
	float offset = 1.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, 0.2f, z));

		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

		model = glm::rotate(model, (GLfloat)glfwGetTime() * radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		modelMatrices[i] = model;
	}

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), value_ptr(modelMatrices[0]), GL_STATIC_DRAW);

	for (unsigned int i = 0; i < house.meshes.size(); i++)
	{
		unsigned int VAO = house.meshes[i].VAO;
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}

void SetHouseTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(-30.0f, 0.3f, -30.0f));
	model = glm::rotate(model, radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetBalloonTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
	model = glm::translate(model, glm::vec3(50.0f, 0.5f, 50.0f));
	model = glm::rotate(model, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetGiftTranslation(GLuint Program)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
	model = glm::translate(model, glm::vec3(-30.0f, 0.8f, 30.0f));
	model = glm::rotate(model, radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetLightObjTranslation(GLuint Program)
{
	mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.5f));
	GLint modelLoc = glGetUniformLocation(Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SetLightUniform(GLuint Program)
{
	GLint lightColorLoc = glGetUniformLocation(Program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	GLint lightPosLoc = glGetUniformLocation(Program, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	GLint lightDirLoc = glGetUniformLocation(Program, "lightDirect");
	glUniform3f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z);
	GLint viewPosLoc = glGetUniformLocation(Program, "viewPos");
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
}

void SetCameraUniform(GLuint Program) {
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);
	view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(radians(45.0f), (GLfloat)Width / (GLfloat)Height, 0.1f, 100.0f);

	GLint viewLoc = glGetUniformLocation(Program, "view");
	GLint projLoc = glGetUniformLocation(Program, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void InitCube(GLuint VAO)
{
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void InitTexture(GLuint Program, GLuint textureID)
{
	glActiveTexture(GL_TEXTURE0 + 0);
	glUniform1i(glGetUniformLocation(Program, ("textures" + 0)), 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	GLfloat cameraSpeed = 1.0f;
	if (key == GLFW_KEY_UP)
		cameraPos += cameraSpeed * cameraFront;
	if (key == GLFW_KEY_DOWN)
		cameraPos -= cameraSpeed * cameraFront;
	if (key == GLFW_KEY_LEFT)
		cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_RIGHT)
		cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_S)
		moveZ += 10;
	if (key == GLFW_KEY_W)
		moveZ -= 10;
	if (key == GLFW_KEY_D)
		moveX += 10;
	if (key == GLFW_KEY_A)
		moveX -= 10;
	if (key == GLFW_KEY_LEFT_SHIFT)
		moveY += 10;
	if (key == GLFW_KEY_LEFT_CONTROL)
		moveY -= 10;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
}

int main() {
	setlocale(LC_ALL, "Russian");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(Width, Height, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, Width, Height);
	glEnable(GL_DEPTH_TEST);

	shader treeShader(VertexShader, FragShader);
	Model tree("C:\\Models\\tree\\tree obj.obj");

	shader airBalloonShader(VertexShader, FragShader);
	Model airBalloon("C:\\Models\\AirBalloon\\11809_Hot_air_balloon_l2.obj");

	shader sleighShader(VertexShaderRotate, FragShader);
	Model sleigh("C:\\Models\\sleigh\\santasleigh.obj");

	shader houseTargetShader(VertexShaderInstance, FragShader);
	Model houseTarget("C:\\Models\\house2\\Snow covered CottageOBJ.obj");
	
	houseTargetShader.Use();
	SetHouseTargetTranslation(houseTargetShader.Program, houseTarget);

	shader houseShader(VertexShader, FragShader);
	Model house("C:\\Models\\house1\\medieval house.obj");

	shader balloonShader(VertexShader, FragShader);
	Model balloon("C:\\Models\\balloon\\13499_Balloon_Cluster_v1_L2.obj");

	shader giftShader(VertexShader, FragShader);
	Model gift("C:\\Models\\gift\\11563_gift_box_V3.obj");

	shader lightingShader(VertexShaderLight, FragShaderLight);
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	InitCube(lightVAO);

	shader groundShader(VertexShader, FragShader);
	GLuint groundVAO;
	glGenVertexArrays(1, &groundVAO);
	InitCube(groundVAO);
	GLuint texture0;
	TextureFromFile("snow.png", texture0);
	InitTexture(groundShader.Program, texture0);

	GLfloat rotationAngle = 0.0f;
	sf::Time elapsedTime;
	sf::Clock clock;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		elapsedTime = clock.getElapsedTime();
		if (elapsedTime > sf::milliseconds(5))
		{
			rotationAngle += 0.01f;
			if (rotationAngle > 360)
				rotationAngle = 360.0f;
			elapsedTime = clock.restart();
		}

		glClearColor(0.8f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Light
		lightingShader.Use();
		SetCameraUniform(lightingShader.Program);
		SetLightObjTranslation(lightingShader.Program);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Ground
		groundShader.Use();
		InitTexture(groundShader.Program, texture0);
		SetLightUniform(groundShader.Program);
		SetCameraUniform(groundShader.Program);
		SetGroundTranslation(groundShader.Program);
		glBindVertexArray(groundVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Air Balloon
		airBalloonShader.Use();
		SetLightUniform(airBalloonShader.Program);
		SetCameraUniform(airBalloonShader.Program);
		SetAirBalloonTranslation(treeShader.Program);
		airBalloon.Draw(airBalloonShader);

		// Tree
		treeShader.Use();
		SetLightUniform(treeShader.Program);
		SetCameraUniform(treeShader.Program);
		SetTreeTranslation(treeShader.Program);
		tree.Draw(treeShader);

		// Sleigh
		sleighShader.Use();
		GLint rotationLoc = glGetUniformLocation(sleighShader.Program, "rotationY");
		glUniform1f(rotationLoc, rotationAngle);
		SetLightUniform(sleighShader.Program);
		SetCameraUniform(sleighShader.Program);
		SetSleighTranslation(sleighShader.Program);
		sleigh.Draw(sleighShader);

		// House Target
		houseTargetShader.Use();
		SetLightUniform(houseTargetShader.Program);
		SetCameraUniform(houseTargetShader.Program);

		glActiveTexture(GL_TEXTURE0 + 0);
		glUniform1i(glGetUniformLocation(houseTargetShader.Program, ("textures" + std::to_string(0)).c_str()), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, houseTarget.meshes[0].textures[0]);
		for (unsigned int i = 0; i < houseTarget.meshes.size(); i++)
		{
			glBindVertexArray(houseTarget.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(houseTarget.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, 5);
			glBindVertexArray(0);
		}

		// House
		houseShader.Use();
		SetLightUniform(houseShader.Program);
		SetCameraUniform(houseShader.Program);
		SetHouseTranslation(houseShader.Program);
		house.Draw(houseShader);

		// Balloon
		balloonShader.Use();
		SetLightUniform(balloonShader.Program);
		SetCameraUniform(balloonShader.Program);
		SetBalloonTranslation(balloonShader.Program);
		balloon.Draw(balloonShader);

		// Gift
		giftShader.Use();
		SetLightUniform(giftShader.Program);
		SetCameraUniform(giftShader.Program);
		SetGiftTranslation(giftShader.Program);
		gift.Draw(giftShader);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}