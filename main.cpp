#include <SDL.h>
#include <glew.h>
#include <string>
#include <vector>
#include <iostream>
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
#include "ImageLoader.h"

#define MAX_ACCELERATION  0.1f

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_FPS 60

using namespace std;

SDL_Window *mainWindow;
SDL_GLContext mainContext;
GLuint rendering_program;

glm::mat4 proj_matrix = glm::mat4(1.0f);
Camera camera;

Sprite sprite;
GLTexture fishTexture;
GLSLProgram colorProgram;

float currentTime;

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
	sprite.init(-0.5f, -0.5f, 1.0f, 1.0f);
	fishTexture = ImageLoader::loadPNG("Textures/Fish/blue_fish_1.png");
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


	mainWindow = SDL_CreateWindow("Simple program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!mainWindow)
	{
		std::cout << "Unable to create window\n";
		CheckSDLError(__LINE__);
		return false;
	}


	camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);


	//should calculate proj matrix


	SDL_AddEventWatch(onResize, mainWindow);

	mainContext = SDL_GL_CreateContext(mainWindow);
	if (!mainContext)
	{
		glGetError();
		CheckSDLError(__LINE__);
		return false;
	}

	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Unable to set VSYNC!");
	}

	glewExperimental = GL_TRUE;
	glewInit();
	
	initShaders();
	initEntities();

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
	glBindTexture(GL_TEXTURE_2D, fishTexture.id);
	GLint textureLocation = colorProgram.getUniformLocation("mSampler");
	glUniform1i(textureLocation, 0);

	GLuint timeLocation = colorProgram.getUniformLocation("time");
	glUniform1f(timeLocation, currentTime/1000.0f);

	/*GLint proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glm::mat4 cameraMatrix = camera.getCameraMatrix();
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &(cameraMatrix[0][0]));*/

	sprite.draw();
	
	glBindTexture(GL_TEXTURE_2D, 0);

	colorProgram.unuse();

	GLenum e = glGetError();
	SDL_GL_SwapWindow(mainWindow);

}

void RunGame()
{
	bool loop = true;
	int frameTime;
	const int frameDelay = 1000 / MAX_FPS;

	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;

	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				loop = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				Mouse::updateButton(event.button.button, event.type, event.motion.x, event.motion.y);
				if (Mouse::left_isPressed())
				{
					// add a new fish
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				Mouse::updateButton(event.button.button, event.type, event.motion.x, event.motion.y);
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				// zoom
				camera.setScale(camera.getScale() + event.wheel.y * SCALE_SPEED);

				std::cout << "yoffset: " << event.wheel.y << std::endl;
			}
			if (Mouse::right_isPressed())
			{
			}
		}

		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);


		if (currentKeyStates[SDL_SCANCODE_W])
		{
			camera.setPosition(camera.getPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
		}
		else if (currentKeyStates[SDL_SCANCODE_S])
		{
			camera.setPosition(camera.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
		}
		else if (currentKeyStates[SDL_SCANCODE_A])
		{
			camera.setPosition(camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
		}
		else if (currentKeyStates[SDL_SCANCODE_D])
		{
			camera.setPosition(camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
		}
			
			
		camera.update();

		currentTime = SDL_GetTicks();
		
		DrawGame();

		frameTime = SDL_GetTicks() - currentTime;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void Cleanup()
{


	SDL_GL_DeleteContext(mainContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}

float randomClamped() //return a float number between -1 and 1
{
    return -1+2*((float)rand())/RAND_MAX;
}

struct MaterialPoint
{
    float x, y, z; //pozitie
    float vx=0, vy=0, vz=0; //viteza
    float ax=0, ay=0, az=0; //acceleratie

    void setAcceleration(float accx = 0.0f, float accy = 0.0f, float accz = 0.0f)
    {
        ax = accx;
        ay = accy;
        az = accz;
    }

    void moveMe()
    {
        glTranslatef(x, y, z);

        vx += ax;
        vy += ay;
        vz += az;

        x += vx;
        y += vy;
        z += vz;

        vx = 0;
        vy = 0;

    }

};

class body: public MaterialPoint
{
private:
    bool tagged = false;
public:
    body(float x=0.0f, float y=0.0f, float z=0.0f)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void setColor(float r=1.0,float g=1.0, float b=0.0)
    {
        _r=r;
        _g=g;
        _b=b;
    };
    void show()
    {
        //glColor3f(_r,_g,_b);
        //glutSolidCube(2); //cub cu latura 2
    }

    void tag()
    {
        tagged = true;
        this->setColor(1.0f, 0.0f, 1.0f);
    }

    void untag()
    {
        this->setColor(1.0f,1.0f,0.0f);
        tagged = false;
    }
    bool isTagged()
    {
        return tagged;
    }

    void tagNeighbors(std::vector<body>& neighbours, double radius)
    {
        //iterate through all neighbors
        for (std::vector<body>::iterator it = neighbours.begin() ; it != neighbours.end(); ++it)
        {
            //clear any current tag
            it->untag();

            //distance between the neighbor and the current body
            float toX = it->x - this->x;
            float toY = it->y - this->y;
            float toLength = sqrt(toX*toX + toY*toY);

            //range of detection
            double range = radius + 2;


            //if the neighbor is within range, tag it
            if(toLength < range*range)
            {
                it->tag();
            }

        }
    }

    void wander(float radius, float distance, float jitter)
    {
        //add a random vector to the target's position
        float targetX = randomClamped() * jitter;
        float targetY = randomClamped() * jitter;
        //normalize the new vector and increase the length of it to the same as the radius
        float targetLength = sqrt(targetX*targetX + targetY*targetY);
        targetX = targetX/targetLength * radius;
        targetY = targetY/targetLength * radius;

        this->_targetX=(targetX - this->x)/10.0f;
        this->_targetY=(targetY - this->y)/10.0f;

    }

    void offsetPursuit(body& leader, float offsetX, float offsetY)
    {
        //the look-ahead is proportional to the distance between the leader and the pursuer;
        //and is inversely proportional to the sum of both
        float toOffsetX = leader.x - offsetX;
        float toOffsetY = leader.y - offsetY;
        float toOffsetLength = sqrt(toOffsetX*toOffsetX + toOffsetY*toOffsetY);
        float lookAhead = 2*toOffsetLength/(leader.vx+leader.vy+1.0f);

        //set the body's target  the predicted future position of the offset
        this->_targetX = toOffsetX+leader.vx*lookAhead;
        this->_targetY = toOffsetY+leader.vy*lookAhead;

    }

    void cohesion(std::vector<body>& neighbours)
    {
        float centerOfMassX=0.0f, centerOfMassY=0.0f;

        int neighbourCount = 0;

        for (std::vector<body>::iterator it = neighbours.begin() ; it != neighbours.end(); ++it)
        {
                //find the center of mass of all entities
                centerOfMassX += it->x;
                centerOfMassY += it->y;

                ++neighbourCount;
        }

        if (neighbourCount>0)
        {
            //the center of mass is the average of the sum of positions
            centerOfMassX/=(float)neighbourCount;
            centerOfMassY/=(float)neighbourCount;

            //set the body's target that position
            this->_targetX=centerOfMassX;
            this->_targetY=centerOfMassY;

        }

    }

    void separation(std::vector<body>& neighbours)
    {
        for (std::vector<body>::iterator it = neighbours.begin() ; it != neighbours.end(); ++it)
        {
            if(it->isTagged())
            {
                //the distance between this body and the neighbour
                float distX = this->x - it->x;
                float distY = this->y - it->y;
                float dist = sqrt(distX*distX + distY * distY);

                //add that distance to the body's velocity
                it->vx = -1*distX/dist/dist*2;
                it->vy  = -1*distY/dist/dist*2;
            }
        }
    }

    void moveTo(float px, float py)
    {
        this->setColor(1.0f,50.0f,0.0f);
        //glPushMatrix();

        //the distance between this body and the point
        double distX = px - this->x;
        double distY = py - this->y;
        double dist = sqrt(distX*distX + distY * distY);

        //if the body has not yet reached its destination
        if (dist > 1.0f)
        {
            //calculate the speed relative to the distance
            double speed = dist/(50.0f);


            this->vx = distX*speed/dist;
            this->vy = distY*speed/dist;
            this->isMoving = true;


        }
        else
        {
            //stop the body
            this->vx = 0.0f;
            this->vy = 0.0f;
            this->isMoving = false;
        }

        this->moveMe();
        this->show();



        //glPopMatrix();

        //spawn a red cube indicating the point
        //glPushMatrix();
        //glColor3f(1.0f,0.0f,0.0f);
        //glTranslatef(px,py,-100.0f);
        //glutSolidCube(2);
        //glPopMatrix();

    }
    float _r,_g,_b;
    float _targetX, _targetY;
    bool isMoving = false;
};
body shark;
std::vector<body> neighbours;
float px = 0.0f, py = 0.0f;
bool wanderOn = false, followOn = false, cohesionOn = false;


//static void display(void)
//{
//    static int frame,timebase=0;
//    int time;
//    char s[100];
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glLoadIdentity(); //initializarea sistemului de coordonate
//
//        shark.moveTo(shark._targetX,shark._targetY);
//        shark.tagNeighbors(neighbours,1.0f);
//        shark.separation(neighbours);
//
//
//    for (std::vector<body>::iterator it = neighbours.begin() ; it != neighbours.end(); ++it)
//    {
//
//        glPushMatrix();
//        glTranslatef(it->x, it->y, it->z);
//        it->show();
//
//        if(!it->isMoving && wanderOn && it==neighbours.begin())
//        {
//
//            it->wander(300.0f, 200.0f,200.0f);
//        }
//        else if(!it->isMoving && followOn )
//        {
//            it->offsetPursuit(neighbours.front(),2.0f,2.0f);
//
//        }
//        else if(!it->isMoving && cohesionOn)
//        {
//            it->cohesion(neighbours);
//        }
//        else if(!it->isMoving && wanderOn)
//        {
//            it->wander(300.0f,200.0f,200.0f);
//        }
//        if(it->isTagged())
//        {
//            it->moveMe();
//        }
//        else if(wanderOn || cohesionOn || followOn)
//        {
//            it->moveTo(it->_targetX,it->_targetY);
//        }
//        glPopMatrix();
//    }
//
//
//    glutSwapBuffers();
//
//}

//static void timer(int v)
//{
//    static int frame,timebase=0;
//    int time;
//    char s[100];
//    frame++;
//    time=glutGet(GLUT_ELAPSED_TIME);
//    if(Keyboard::isPressed('w'))
//    {
//        wanderOn = true;
//        printf("wander activated\n");
//    }
//    if(Keyboard::isPressed('s'))
//    {
//        wanderOn = false;
//        printf("wander deactivated\n");
//    }
//    if(Keyboard::isPressed('q'))
//    {
//        followOn = true;
//        printf("follow activated");
//    }
//    if(Keyboard::isPressed('a'))
//    {
//        followOn = false;
//        printf("follow deactivated");
//    }
//
//    if(Keyboard::isPressed('d'))
//    {
//
//        cohesionOn = false;
//        printf("cohesion deactivated");
//    }
//    if(Keyboard::isPressed('e'))
//    {
//        cohesionOn = true;
//        printf("cohesion activated");
//    }
//    if(Mouse::right_isPressed())
//    {
//        shark._targetX = (float)Mouse::getX()/((float)glutGet(GLUT_WINDOW_WIDTH)/108.0f)-54;
//        shark._targetY = -1*((float)Mouse::getY()/((float)glutGet(GLUT_WINDOW_HEIGHT)/80.0f)-40);
//    }
//
//    if(Mouse::left_isPressed())
//    {
//
//        float x = (float)Mouse::getX()/((float)glutGet(GLUT_WINDOW_WIDTH)/108.0f)-54;
//        float y = -1*((float)Mouse::getY()/((float)glutGet(GLUT_WINDOW_HEIGHT)/80.0f)-40);
//        body n = body(x,y,-100.f);
//        n.setColor(0.0f,0.0f,1.0f);
//
//        neighbours.push_back(n);
//    }
//
//    if (time - timebase > 0)
//    {
//        sprintf(s,"[FPS:%4.2f] Lab 2: Imagine+Imagine+Imagine+...= Animatie !",frame*1000.0/(time-timebase));
//        glutSetWindowTitle(s);
//        timebase = time;
//        frame = 0;
//    }
//
//    glutPostRedisplay();
//    glutTimerFunc(1000/FPS,timer,1);
//
//
//}


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
