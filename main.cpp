#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include "mouse.h"
#include "keyboard.h"
#define FPS 60

#define MAX_ACCELERATION  0.1f

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
        glColor3f(_r,_g,_b);
        glutSolidCube(2); //cub cu latura 2
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
        glPushMatrix();

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



        glPopMatrix();

        //spawn a red cube indicating the point
        glPushMatrix();
        glColor3f(1.0f,0.0f,0.0f);
        glTranslatef(px,py,-100.0f);
        glutSolidCube(2);
        glPopMatrix();

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
    const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 2.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    // init bodies
    shark.x = 0.0f;
    shark.y = 0.0f;
    shark.z = -100.0f;
    shark.setColor(1.0f,0.0f,0.0f);

    body n = body(10.0f,-5.0f,-100.f);
    n.setColor(0.0f,0.0f,1.0f);

    neighbours.push_back(n);


}
static void display(void)
{
    static int frame,timebase=0;
    int time;
    char s[100];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity(); //initializarea sistemului de coordonate

        shark.moveTo(shark._targetX,shark._targetY);
        shark.tagNeighbors(neighbours,1.0f);
        shark.separation(neighbours);


    for (std::vector<body>::iterator it = neighbours.begin() ; it != neighbours.end(); ++it)
    {

        glPushMatrix();
        glTranslatef(it->x, it->y, it->z);
        it->show();

        if(!it->isMoving && wanderOn && it==neighbours.begin())
        {

            it->wander(300.0f, 200.0f,200.0f);
        }
        else if(!it->isMoving && followOn )
        {
            it->offsetPursuit(neighbours.front(),2.0f,2.0f);

        }
        else if(!it->isMoving && cohesionOn)
        {
            it->cohesion(neighbours);
        }
        else if(!it->isMoving && wanderOn)
        {
            it->wander(300.0f,200.0f,200.0f);
        }
        if(it->isTagged())
        {
            it->moveMe();
        }
        else if(wanderOn || cohesionOn || followOn)
        {
            it->moveTo(it->_targetX,it->_targetY);
        }
        glPopMatrix();
    }


    glutSwapBuffers();

}

static void timer(int v)
{
    static int frame,timebase=0;
    int time;
    char s[100];
    frame++;
    time=glutGet(GLUT_ELAPSED_TIME);
    if(Keyboard::isPressed('w'))
    {
        wanderOn = true;
        printf("wander activated\n");
    }
    if(Keyboard::isPressed('s'))
    {
        wanderOn = false;
        printf("wander deactivated\n");
    }
    if(Keyboard::isPressed('q'))
    {
        followOn = true;
        printf("follow activated");
    }
    if(Keyboard::isPressed('a'))
    {
        followOn = false;
        printf("follow deactivated");
    }

    if(Keyboard::isPressed('d'))
    {

        cohesionOn = false;
        printf("cohesion deactivated");
    }
    if(Keyboard::isPressed('e'))
    {
        cohesionOn = true;
        printf("cohesion activated");
    }
    if(Mouse::right_isPressed())
    {
        shark._targetX = (float)Mouse::getX()/((float)glutGet(GLUT_WINDOW_WIDTH)/108.0f)-54;
        shark._targetY = -1*((float)Mouse::getY()/((float)glutGet(GLUT_WINDOW_HEIGHT)/80.0f)-40);
    }

    if(Mouse::left_isPressed())
    {

        float x = (float)Mouse::getX()/((float)glutGet(GLUT_WINDOW_WIDTH)/108.0f)-54;
        float y = -1*((float)Mouse::getY()/((float)glutGet(GLUT_WINDOW_HEIGHT)/80.0f)-40);
        body n = body(x,y,-100.f);
        n.setColor(0.0f,0.0f,1.0f);

        neighbours.push_back(n);
    }

    if (time - timebase > 0)
    {
        sprintf(s,"[FPS:%4.2f] Lab 2: Imagine+Imagine+Imagine+...= Animatie !",frame*1000.0/(time-timebase));
        glutSetWindowTitle(s);
        timebase = time;
        frame = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(1000/FPS,timer,1);


}

static void
idle(void)
{
    glutPostRedisplay();

}
/* Punct de intrare in program */
int
main(int argc, char *argv[])
{
    int width = 640;
    int height = 480;

    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(""); //titlu vid, il setez in display()
    glutKeyboardFunc(Keyboard::keyDown);
    glutKeyboardUpFunc(Keyboard::keyUp);
    glutMotionFunc(Mouse::updatePosition);
    glutPassiveMotionFunc(Mouse::updatePosition);
    glutMouseFunc(Mouse::updateButton);
    glutDisplayFunc(display);
    //glutIdleFunc(idle);
    glutTimerFunc(1000/FPS,timer,1);
    initGL(width, height);
    glutMainLoop();
    return EXIT_SUCCESS;
}
