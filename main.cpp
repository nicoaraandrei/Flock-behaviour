#include <SDL.h>
#include <glew.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "GLSLProgram.h"
#include "Camera.h"
#include "Mouse.h"
//#include "keyboard.h"
#include "Sprite.h"
#include "Errors.h"
#include "GLTexture.h"
#include "SpriteBatch.h"
#include "ResourceManager.h"
#include "MovingEntity.h"
#include "Timing.h";

#define PI 3.14159

#define MAX_FPS 300

using namespace std;

SDL_Window *mainWindow;
SDL_GLContext mainContext;
GLuint rendering_program;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

glm::mat4 proj_matrix = glm::mat4(1.0f);
Camera camera;
const float CAMERA_SPEED = 10.0f;
const float SCALE_SPEED = 0.1f;

//std::vector<Sprite*> sprites;
SpriteBatch spriteBatch;
GLSLProgram colorProgram;

FpsLimiter fpsLimiter;
float fps = 0.0f;

std::vector<MovingEntity> fishBullets;
MovingEntity blueFish;

float currentTime;
float deltaTime;
bool isRunning = true;

float randomFloat() //return a float number between -1 and 1
{
	return  ((float)rand()) / (RAND_MAX + 1.0);
}


void SetOpenGLAttributes()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void CheckSDLError(int line = -1)
{
	std::string error = SDL_GetError();

	if (error != "")
	{
		std::cout << "SLD Error : " << error << std::endl;

		if (line != -1)
			std::cout << "\nLine : " << line << std::endl;

		SDL_ClearError();
	}
}

static int onResize(void* data, SDL_Event* event)
{
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
	{
		SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
		if (win == (SDL_Window*)data)
		{
			int windowWidth, windowHeight;
			SDL_GetWindowSize(win, &windowWidth, &windowHeight);
			glViewport(0, 0, windowWidth, windowHeight);

			camera.init(windowWidth, windowHeight);
			camera.setScale(1.0f);
			WINDOW_WIDTH = windowWidth;
			WINDOW_HEIGHT = windowHeight;
			printf("resizing.....\n");
		}
	}
	return 0;
}

void initShaders()
{
	colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	colorProgram.addAttribute("vertexPosition");
	colorProgram.addAttribute("vertexColor");
	colorProgram.addAttribute("vertexUV");
	colorProgram.linkShaders();
}

void initEntities()
{

	/*sprites.push_back(new Sprite());
	sprites.back()->init(0.0f, 0.0f, WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, "Textures/Fish/blue_fish_1.png");

	sprites.push_back(new Sprite());
	sprites.back()->init(WINDOW_WIDTH / 2, 0.0f, WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, "Textures/Fish/blue_fish_1.png");*/

	//glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);

	//blueFish.init(pos, glm::vec2(0.0f, 1.0f), 3.0f, -1, "Textures/Fish/blue_fish.png");]

	blueFish.init(glm::vec2(0.0f, 0.0f), randomFloat() * 2 * PI, glm::vec2(0.0f, 0.0f), 1.0f, 0.1f, 3.7f, PI, 3.0f, "Textures/Fish/blue_fish.png");

	spriteBatch.init();


}

bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		cout << "SDL initialization succeeded!\n";
	}

	SetOpenGLAttributes();


	mainWindow = SDL_CreateWindow("Flock behaviour", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!mainWindow)
	{
		std::cout << "Unable to create window\n";
		CheckSDLError(__LINE__);
		return false;
	}

	SDL_AddEventWatch(onResize, mainWindow);

	mainContext = SDL_GL_CreateContext(mainWindow);
	if (!mainContext)
	{
		glGetError();
		CheckSDLError(__LINE__);
		return false;
	}

	//set vsync 1 - enable, 2 - disable
	if (SDL_GL_SetSwapInterval(0) < 0)
	{
		printf("Unable to set VSYNC!");
	}

	glewExperimental = GL_TRUE;
	glewInit();

	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);

	initShaders();
	initEntities();

	fpsLimiter.init(MAX_FPS);

	return true;
}

void DrawGame()
{
	const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	colorProgram.use();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = colorProgram.getUniformLocation("mSampler");
	glUniform1i(textureLocation, 0);

	//GLuint timeLocation = colorProgram.getUniformLocation("time");
	//glUniform1f(timeLocation, currentTime/1000.0f);

	GLint proj_location = colorProgram.getUniformLocation("proj_matrix");

	glm::mat4 cameraMatrix = camera.getCameraMatrix();
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &(cameraMatrix[0][0]));



	spriteBatch.begin();

	glm::vec4 pos(-WINDOW_WIDTH/(2 * camera.getScale()), -WINDOW_HEIGHT/(2 * camera.getScale()), WINDOW_WIDTH / camera.getScale(), WINDOW_HEIGHT / camera.getScale());
	glm::vec4 uv(0.0f, 0.0f, WINDOW_WIDTH/(480.0f * camera.getScale()), (float)WINDOW_HEIGHT/(480.0f * camera.getScale()));
	static GLTexture waterTexture = ResourceManager::getTexture("Textures/Background/Water.png");
	ColorRGBA8 whiteColor;
	whiteColor.r = 255.0f;
	whiteColor.g = 255.0f;
	whiteColor.b = 255.0f;
	whiteColor.a = 255.0f;

	spriteBatch.draw(pos, uv, waterTexture.id, 1.0f, whiteColor);


	blueFish.draw(spriteBatch);

	for (int i = 0; i < fishBullets.size(); i++)
	{
		fishBullets[i].draw(spriteBatch);
	}

	spriteBatch.end();
	spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);

	colorProgram.unuse();

	GLenum e = glGetError();
	SDL_GL_SwapWindow(mainWindow);

}


void updateAgents(float deltaTime)
{
	blueFish.update(deltaTime);

	for (int i = 0; i < fishBullets.size(); i++) {
		fishBullets[i].update(deltaTime);
	}
}

void processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			isRunning = false;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			Mouse::updateButton(event.button.button, event.type, event.motion.x, event.motion.y);
			if (Mouse::left_isPressed())
			{
				// add a new fish
				glm::vec2 mouseCoords = glm::vec2(Mouse::getX(), Mouse::getY());
				mouseCoords = camera.convertScreenToWorld(mouseCoords);
				std::cout << mouseCoords.x << "  " << mouseCoords.y << std::endl;

				glm::vec2 playerPosition(0.0f);
				glm::vec2 direction = mouseCoords - playerPosition;
				direction = glm::normalize(direction);

				//fishBullets.emplace_back(mouseCoords, direction, 2.0f, -1, "Textures/Fish/yellow_fish.png");
				fishBullets.emplace_back(mouseCoords, randomFloat() * 2 * PI, glm::vec2(0.0f, 0.0f), 1.0f, 0.1f, 3.5f, PI, 3.0f, "Textures/Fish/yellow_fish.png");
				//fishBullets.back().setTargetEntity(&blueFish);
				//fishBullets.back().getSteering()->setOffset(glm::vec2(0.1f*i, 0.0f));
				fishBullets.back().getSteering()->wanderOn();
				
				for (int i = 0; i < fishBullets.size(); i++)
				{
				
					//fishBullets[i].getSteering()->setOffset(glm::vec2(32.0f, 0.0f));
					fishBullets[i].getSteering()->setAgent(&fishBullets[i]);
				}

				/*blueFish.setTargetEntity(&fishBullets.front());
				blueFish.getSteering()->evadeOn();*/
			}

			if (Mouse::right_isPressed())
			{
				glm::vec2 mouseCoords = glm::vec2(Mouse::getX(), Mouse::getY());
				mouseCoords = camera.convertScreenToWorld(mouseCoords);
				//std::cout << mouseCoords.x << "  " << mouseCoords.y << std::endl;

				glm::vec2 direction = mouseCoords - blueFish.getPos();
				direction = glm::normalize(direction);

				blueFish.setTarget(mouseCoords);
				blueFish.getSteering()->arriveOn();
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			Mouse::updateButton(event.button.button, event.type, event.motion.x, event.motion.y);
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			// zoom
			std::cout << "scale: " << camera.getScale() + event.wheel.y * SCALE_SPEED << std::endl;
			camera.setScale(camera.getScale() + event.wheel.y * SCALE_SPEED);

			//std::cout << "yoffset: " << event.wheel.y << std::endl;
		}

	}

	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);


	if (currentKeyStates[SDL_SCANCODE_W])
	{
		camera.setPosition(camera.getPosition() + glm::vec2(0.0f, deltaTime * CAMERA_SPEED));
	}
	else if (currentKeyStates[SDL_SCANCODE_S])
	{
		camera.setPosition(camera.getPosition() + glm::vec2(0.0f, deltaTime * -CAMERA_SPEED));
	}
	else if (currentKeyStates[SDL_SCANCODE_A])
	{
		camera.setPosition(camera.getPosition() + glm::vec2(deltaTime * -CAMERA_SPEED, 0.0f));
	}
	else if (currentKeyStates[SDL_SCANCODE_D])
	{
		camera.setPosition(camera.getPosition() + glm::vec2(deltaTime * CAMERA_SPEED, 0.0f));
	}

}

void RunGame()
{
	int frameTime;
	const int frameDelay = 1000 / MAX_FPS;

	const float DESIRED_FPS = 60.0f;
	const int MAX_PHYSICS_STEPS = 6;
	const float MS_PER_SECOND = 1000.0f;
	const float  DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float previousTicks = SDL_GetTicks();

	while (isRunning)
	{

		fpsLimiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		processInput();

		camera.update();

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateAgents(deltaTime);
			totalDeltaTime -= deltaTime;
			i++;
		}

		DrawGame();

		fps = fpsLimiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10)
		{
			//std::cout << fps << std::endl;
			frameCounter = 0;
		}
	}
}

void Cleanup()
{


	SDL_GL_DeleteContext(mainContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}



int main(int argc, char *argv[])
{
	if (!Init())
	{
		cin.get();
		return -1;
	}

	RunGame();

	Cleanup();
	cin.get();
	return 0;
}
