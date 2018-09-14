#include <SDL.h>
#include <glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//#include "mouse.h"
//#include "keyboard.h"

#define MAX_ACCELERATION  0.1f

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_FPS 60

using namespace std;

SDL_Window *mainWindow;
SDL_GLContext mainContext;
GLuint rendering_program;
GLuint vertex_array_object;

glm::mat4 proj_matrix = glm::mat4(1.0f);

struct vertex
{
	// Position
	float x;
	float y;
	float z;
	float w;

	// Color
	float r;
	float g;
	float b;
	float a;
};

int check_program(GLuint program)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(program);

		std::cout << "ERROR: ";
		for (auto i : infoLog)
		{
			std::cout << i;
		}
		// Provide the infolog in whatever manner you deem best.
		// Exit with failure.
		return 0;
	}
	return 1;
}

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint tesselation_control_shader;
	GLuint tesselation_evaluation_shader;
	GLuint geometry_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] =
	{
		"#version 410 core \n"
		"\n"
		"in vec4 position; \n"
		"out VS_OUT \n"
		"{ \n"
		"	vec4 color; \n"
		"} vs_out; \n"
		"\n"
		"uniform mat4 mv_matrix; \n"
		"uniform mat4 proj_matrix; \n"
		"void main(void) \n"
		"{ \n"
		"   gl_Position = proj_matrix * mv_matrix * position; \n"
		"   vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0); \n"
		"} \n"
	};

	static const GLchar *tesselation_control_shader_source[] =
	{
		"#version 410 core \n"
		"\n"
		"layout (vertices = 3) out; \n"
		"\n"
		"in vec4 vs_color[]; \n"
		"patch out vec4 patch_color; \n"
		"void main(void) \n"
		"{ \n"
		"   if (gl_InvocationID == 0) \n"
		"	{ \n"
		"		gl_TessLevelInner[0] = 5.0; \n"
		"		gl_TessLevelOuter[0] = 5.0; \n"
		"		gl_TessLevelOuter[1] = 5.0; \n"
		"		gl_TessLevelOuter[2] = 5.0; \n"
		"	} \n"
		"	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; \n"
		"	patch_color = vs_color[gl_InvocationID]; \n"
		"} \n"
	};

	static const GLchar *tesselation_evaluation_shader_source[] =
	{
		"#version 410 core \n"
		"\n"
		"layout (triangles, equal_spacing, cw) in; \n"
		"\n"
		"patch in vec4 patch_color; \n"
		"out vec4 tes_color; \n"
		"void main(void) \n"
		"{ \n"
		"	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position + \n"
		"				   gl_TessCoord.y * gl_in[1].gl_Position + \n"
		"				   gl_TessCoord.z * gl_in[2].gl_Position); \n"
		"	tes_color = patch_color; \n"
		"} \n"
	};

	static const GLchar *geometry_shader_source[] =
	{
		"#version 410 core \n"
		"\n"
		"layout (triangles) in; \n"
		"layout (points, max_vertices = 3) out; \n"
		"\n"
		"in vec4 tes_color[]; \n"
		"out vec4 gs_color; \n"
		"void main(void) \n"
		"{ \n"
		"	int i; \n"
		"	\n"
		"	for (i = 0; i < gl_in.length(); i++) \n"
		"	{ \n"
		"		gs_color = tes_color[i]; \n"
		"		gl_Position = gl_in[i].gl_Position; \n"
		"		EmitVertex(); \n"
		"	} \n"
		"} \n"
	};

	static const GLchar *fragment_shader_source[] =
	{
		"#version 410 core \n"
		"\n"
		"out vec4 color; \n"
		"in VS_OUT \n"
		"{ \n"
		"	vec4 color; \n"
		"} fs_in; \n"
		"\n"
		"void main(void) \n"
		"{ \n"
		"   color =  fs_in.color; \n"
		"} \n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	/*tesselation_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tesselation_control_shader, 1, tesselation_control_shader_source, NULL);
	glCompileShader(tesselation_control_shader);

	tesselation_evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tesselation_evaluation_shader, 1, tesselation_evaluation_shader_source, NULL);
	glCompileShader(tesselation_evaluation_shader);

	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader, 1, geometry_shader_source, NULL);
	glCompileShader(geometry_shader);*/

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	//glAttachShader(program, tesselation_control_shader);
	//glAttachShader(program, tesselation_evaluation_shader);
	//glAttachShader(program, geometry_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	/*glDeleteShader(tesselation_control_shader);
	glDeleteShader(tesselation_evaluation_shader);
	glDeleteShader(geometry_shader);*/
	glDeleteShader(fragment_shader);

	if (!check_program(program))
	{
		return 0;
	}

	return program;
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

glm::mat4 calculateProjMatrix(float fovy, int windowWidth, int windowHeight, float near, float far)
{
	float aspect = (float)windowWidth / (float)windowHeight;
	return glm::perspective(fovy, aspect, near, far);
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
			proj_matrix = calculateProjMatrix(50.f, windowWidth, windowHeight, 0.1f, 1000.0f);
			printf("resizing.....\n");
		}
	}
	return 0;
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

	int windowWidth, windowHeight;
	SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);
	proj_matrix = calculateProjMatrix(50.f, windowWidth, windowHeight, 0.1f, 1000.0f);

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

	

	rendering_program = compile_shaders();

	if (!rendering_program)
	{
		return false;
	}

	//create the vertex buffer
	GLuint buffer;

	static const GLfloat vertices[] =
	{
		-0.25f,  0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		 0.25f, -0.25f, -0.25f,

		 0.25f, -0.25f, -0.25f,
		 0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f, -0.25f,
		 0.25f, -0.25f,  0.25f,
		 0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f,  0.25f,
		 0.25f,  0.25f,  0.25f,
		 0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		 0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		 0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		 0.25f, -0.25f,  0.25f,
		 0.25f, -0.25f, -0.25f,

		 0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f,  0.25f,

		-0.25f,  0.25f, -0.25f,
		 0.25f,  0.25f, -0.25f,
		 0.25f,  0.25f,  0.25f,

		 0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f, -0.25f
	};

	//static const GLfloat positions[] =
	//{
	//	0.25, -0.25, 0.5, 1.0,
	//	-0.25, -0.25, 0.5, 1.0,
	//	0.25, 0.25, 0.5, 1.0
	//};

	//static const GLfloat colors[] =
	//{
	//	1.0, 0.0, 0.0, 1.0,
	//	0.0, 1.0, 0.0, 1.0,
	//	0.0, 0.0, 1.0, 1.0
	//};

	glCreateVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glFrontFace(GL_CW);


	/*


	glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);

	glVertexArrayAttribFormat(vertex_array_object, 0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
	glVertexArrayAttribBinding(vertex_array_object, 0, 0);
	glEnableVertexAttribArray(0);

	glVertexArrayAttribFormat(vertex_array_object, 1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
	glVertexArrayAttribBinding(vertex_array_object, 1, 0);
	glEnableVertexAttribArray(1);

	glVertexArrayVertexBuffer(vertex_array_object, 0, buffer, 0, sizeof(vertex));*/

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	return true;
}

bool one_cube = false;

void RunGame()
{
	bool loop = true;
	 float currentTime;
	int frameTime;
	const int frameDelay = 1000 / MAX_FPS;

	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				loop = false;
			}
		}

		currentTime = (float) SDL_GetTicks() / 1000.0f;
		std::cout << "SDL TIME: "<< currentTime << std::endl;
		const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		const GLfloat color[] = { (float)sin(currentTime / 1000) * 0.5f + 0.5f, (float)cos(currentTime / 1000) * 0.5f + 0.5f, 0.0f, 1.0f };
		const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		glUseProgram(rendering_program);

		GLint mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
		GLint proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));


		if (one_cube)
		{

			float f = (float)currentTime * (float)M_PI * 0.1f;
			glm::mat4 mv_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)) *
				glm::translate(glm::mat4(1.0f), glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)) *
				glm::rotate(glm::mat4(1.0f), (float)currentTime  * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), (float)currentTime  * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else
		{
			for (int i = 0; i < 24; i++)
			{
				float f = (float)i + (float)currentTime * 0.3f;
				glm::mat4 mv_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f)) *
					glm::rotate(glm::mat4(1.0f), (float)currentTime  * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(glm::mat4(1.0f), (float)currentTime  * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::translate(glm::mat4(1.0f), glm::vec3(sinf(2.1f * f) * 2.0f, cosf(1.7f * f) * 2.0f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));

				glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}



		//glPointSize(5.0f);
		//for the tesselation thing
		//glDrawArrays(GL_PATCHES, 0, 3);



		GLenum e = glGetError();
		SDL_GL_SwapWindow(mainWindow);

		frameTime = SDL_GetTicks() - currentTime;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void Cleanup()
{
	glDisableVertexArrayAttrib(vertex_array_object, 0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);

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
        this->setColor(1.0f,0.5f,0.0f);
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

void initGL(int width, int height)
{
    //const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    //glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHTING);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    //glEnable(GL_COLOR_MATERIAL);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 2.0f, 100.0f);
    //glMatrixMode(GL_MODELVIEW);

    //// init bodies
    //shark.x = 0.0f;
    //shark.y = 0.0f;
    //shark.z = -100.0f;
    //shark.setColor(1.0f,0.0f,0.0f);

    //body n = body(10.0f,-5.0f,-100.f);
    //n.setColor(0.0f,0.0f,1.0f);

    //neighbours.push_back(n);


}

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

//static void idle(void)
//{
//    glutPostRedisplay();
//
//}

int main(int argc, char *argv[])
{
    //int width = 640;
    //int height = 480;

    //srand(time(NULL));
    //glutInit(&argc, argv);
    //glutInitWindowSize(width,height);
    //glutInitWindowPosition(10,10);
    //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    //glutCreateWindow(""); //titlu vid, il setez in display()
    //glutKeyboardFunc(Keyboard::keyDown);
    //glutKeyboardUpFunc(Keyboard::keyUp);
    //glutMotionFunc(Mouse::updatePosition);
    //glutPassiveMotionFunc(Mouse::updatePosition);
    //glutMouseFunc(Mouse::updateButton);
    //glutDisplayFunc(display);
    ////glutIdleFunc(idle);
    //glutTimerFunc(1000/FPS,timer,1);
    //initGL(width, height);
    //glutMainLoop();
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
