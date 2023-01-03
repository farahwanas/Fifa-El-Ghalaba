#define GL_SILENCE_DEPRECATION
#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#ifdef __APPLE__
#include <OpenGL/gl3ext.h>
#include <math.h>
#include <vector>
#include <random>
#include<iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "audio.h"




//#include "irrKlang.h"
//using namespace irrklang;
//#pragma comment(lib, "irrKlang.lib")

using namespace std;
#endif
#define SPACE 32


//#include "TextureBuilder.h"
//#include "Model_3DS.h"
//#include "GLTexture.h"
//#include <glut.h>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";




#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float moveFelX=0;
float moveFelZ=0;
float boundsLeft=0;
float boundsRight=0;
float boundsUp=0;
float boundsDown=0;
float moveYaball=0;
float ballBounds=0.3;

float timer=0;

float rotatePlayer=0;




class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(Vector3f &v) {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(Vector3f &v) {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() {
        return *this / sqrt(x * x + y * y + z * z);
    }

    Vector3f cross(Vector3f v) {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        Vector3f rightBelD = up.cross(center - eye).unit()*d;

        eye = eye + rightBelD;
        center = center + rightBelD;
    }

    void moveY(float d) {
        Vector3f upBelD = up.unit() * d;
        eye = eye + upBelD;
        center = center + upBelD;
    }
    
    void gameOver(float d) {
        Vector3f upBelD = up.unit() * d;
        eye = eye + upBelD;
        center = center + upBelD;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        Vector3f veiwBelD = view * d;
        eye = eye + veiwBelD;
        center = center + veiwBelD;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        Vector3f cosA = view * cos(DEG2RAD(a));
        Vector3f sinA = up * sin(DEG2RAD(a));

        view = cosA + sinA;
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        
        
        
        Vector3f cosA = view * cos(DEG2RAD(a));
        Vector3f sinA = right * sin(DEG2RAD(a));
        
        view = cosA+sinA;
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
    
    
    //    gluLookAt(1.8, 1.5, 0.4, 0, 0.0, 0.4, 0.0, 1.0, 0.0); //--> top view
    //      gluLookAt(1.8, 0.6, 0.4, 0.4, 0.9, 0.4, 0.0, 1.0, 0.0); //--> front view
    //    gluLookAt(0.9, 1, -1.9, 0.9, 0.0, 1, 0.0, 1.0, 0.0); //--> side view
    
    void topView() {
            eye.x = 1.8;
            eye.y = 1.5;
            eye.z = 0.4;
            center.x = 0;
            center.y = 0;
            center.z = 0.4;
        }

        void sideView() {
            eye.x = 1.8;
            eye.y = 0.6;
            eye.z = 0.4;
            center.x = 0.4;
            center.y = 0.9;
            center.z = 0.4;

        }

        void frontView() {
            eye.x = 0.9;
            eye.y = 1;
            eye.z = -1.9;
            center.x =0.9;
            center.y = 0.0;
            center.z = 1;

        }
    
    
    
    
};

Camera camera;



void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}




//draw  ground1 (green)
void drawGround1(double topWid, double topThick, double legThick, double legLen){
    glPushMatrix();
    glColor4f(0.0f,1.0f,0.0f,0.0f);
    glTranslated(0, legLen, 0);
    glScaled(topWid, topThick, topWid);
    glutSolidCube(1.2);
    glPopMatrix();
}

//draw  ground2
void drawGround2(double topWid, double topThick, double legThick, double legLen){
    glPushMatrix();
    glColor4f(1.0f,0.0f,0.3f,0.0f);
    glTranslated(0, legLen, 0);
    glScaled(topWid, topThick, topWid);
    glutSolidCube(1.2);
    glPopMatrix();
    
    
}

void drawBall(){
    glPushMatrix();
    
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glTranslated(0, 0.33, 0.1+moveYaball);
    glutSolidSphere(0.014, 14, 14);
    glPopMatrix();
}



void drawTableLeg(double thick, double len) {
    glPushMatrix();
    glTranslated(0, len / 2, 0);
    glScaled(thick, len, thick);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawGoal(double topWid,  double legThick, double legLen) {
    
    double dist = 0.95 * topWid / 2.0 - legThick / 2.0;

    glPushMatrix();
    glTranslated(dist, 0, dist);
    drawTableLeg(legThick, legLen);
    glTranslated(0, 0, -2 * dist);
    drawTableLeg(legThick, legLen);
    glTranslated(-4.0 * dist, 0, 2 * dist);
    drawTableLeg(legThick, legLen);
    glTranslated(0, 0, -2 * dist);
    drawTableLeg(legThick, legLen);
    glPopMatrix();
}



void drawWall(double thickness) {
    glPushMatrix();
    glRotated(90, 0, 0, 1.0);
    glTranslated(0.6, 0.3, 0.08);
    glScaled(1.0, thickness, 1.0);
    glutSolidCube(0.4);
    glPopMatrix();
}


void drawScoreBoard() {

    
    
    glPushMatrix();
    

    glPushMatrix();
    glTranslated(-0.89, 0.3, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.3, 0.2);
    glVertex3f(0.6, 0.3, 0.4);
    glVertex3f(0.6, 0.4, 0.4);
    glVertex3f(0.6, 0.4, 0.2);
    glEnd();
    glPopMatrix();

    glTranslated(0, 0,0.03);

    
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.32);
    glVertex3f(0.6, 0.38, 0.32);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    
    
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.24);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.37, 0.3);
    glVertex3f(0.6, 0.37, 0.31);
    glVertex3f(0.6, 0.38, 0.31);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.26, -0.24);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.37, 0.3);
    glVertex3f(0.6, 0.37, 0.31);
    glVertex3f(0.6, 0.38, 0.31);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.28);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.31);
    glVertex3f(0.6, 0.38, 0.31);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    
    
    glPopMatrix();
    
    
    
}



void drawTimer() {
    
    if((0.2-timer)>0.4){
        
        
            glRasterPos3f(-0.5, 0.8, 0.18); //define position on the screen
            
            char *string = "Match is OVER";
            
            while(*string){
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string++);
            }
            
            glutPostRedisplay();


    }

    else{
        glPushMatrix();
        glTranslated(-0.89, 0.4, -0.2);
        glBegin(GL_POLYGON);
        glColor4f(10.0f,0,0,0);
        glVertex3f(0.6, 0.38, 0.2-timer);
        glVertex3f(0.6, 0.38, 0.4);
        glVertex3f(0.6, 0.4, 0.4);
        glVertex3f(0.6, 0.4, 0.2-timer);
        glEnd();
        glPopMatrix();
    }
    
}


void drawFlag() {

    //flag1
    glPushMatrix();

    glTranslated(0, -0.08,0.03);
    
    glPushMatrix();
    glColor4f(10.0f,0.0f,0.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(0.0f,0.0f,1.0f,1.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glPopMatrix();

    
    //flag2

    glPushMatrix();

    glTranslated(0, -0.08,-0.1);
    
    glPushMatrix();
    glColor4f(90.0f,30.0f,0.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(10.0f,0.0f,0.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(50.0f,50.0f,0.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glPopMatrix();
    
    
    //flag3

    glPushMatrix();

    glTranslated(0, 0.1,-0.2);

    glPushMatrix();
    glColor4f(0.0f,0.0f,50.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(10.0f,10.0f,10.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();
    glPopMatrix();
    
    glTranslated(0, -0.03,0);
    glPushMatrix();
    glColor4f(10.0f,0.0f,0.0f,0.0f);
    glTranslated(-0.885, 0.28, -0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.6, 0.35, 0.3);
    glVertex3f(0.6, 0.35, 0.36);
    glVertex3f(0.6, 0.38, 0.36);
    glVertex3f(0.6, 0.38, 0.3);
    glEnd();

    glPopMatrix();
    
    glPopMatrix();

    
    
}


//henaaa
void drawBench(){
    glTranslated(0.07, -0.06, -0.1);

    
    glPushMatrix();
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.87 / 2, 0.25);
    glScaled(0.2, 0.2, 0.8);
    glutSolidCube(0.15);
    glPopMatrix();
    
    
    glPushMatrix();
    glColor4f(0.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.8 / 2, 0.3);
    glScaled(0.1/2,0.2, 0.2/2);
    glutSolidCube(0.2);
    glPopMatrix();
    
    glPushMatrix();
    glColor4f(0.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.8 / 2, 0.2);
    glScaled(0.1/2,0.2, 0.2/2);
    glutSolidCube(0.2);
    glPopMatrix();

    
    glPushMatrix();
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.87 / 2, 0.25-0.18);
    glScaled(0.2, 0.2, 0.8);
    glutSolidCube(0.15);
    glPopMatrix();


    glPushMatrix();
    glColor4f(0.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.8 / 2, 0.3-0.18);
    glScaled(0.1/2,0.2, 0.2/2);
    glutSolidCube(0.2);
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.0f,1.0f,1.0f,1.0f);
    glTranslated(0.46, 0.8 / 2, 0.2-0.18);
    glScaled(0.1/2,0.2, 0.2/2);
    glutSolidCube(0.2);
    glPopMatrix();

    
    
}





//Player
void drawPlayer(){
    
    glRotated(rotatePlayer, 0, 1, 0);

   
    glColor4f(1.0f,0.0f,1.0f,1.0f);

    //karoosh
    glPushMatrix();
    cout<<"dakhalt";
    glScaled(0.3, 0.4, 0.3);
    glTranslated(0.055, 0.9, 0.01);
    glutSolidCube(0.1);
    glPopMatrix();

    //leg1
    glPushMatrix();
    glScaled(0.1, 0.4, 0.1);
    glTranslated(0, 0.8, 0);
    glutSolidCube(0.1);
    glPopMatrix();

    //leg2
    glPushMatrix();
    glScaled(0.1, 0.4, 0.1);
    glTranslated(0.2, 0.8, 0);
    glutSolidCube(0.1);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0.014, 0.39, 0);
    glutSolidSphere(0.014, 14, 14);
    glPopMatrix();
//
//
//    //hands1
//    glPushMatrix();
//    glTranslated(0.16, -0.37, 0.19);
//    glBegin(GL_LINES);
//    glColor4f(0.0f,0.0f,1.0f,1.0f);
//    glLineWidth(100);
//    glVertex3f(0.08, 0.94, 0.01);
//    glVertex3f(0.08, 0.94, 0.06);
//    glEnd();
//    glPopMatrix();
//
//    //hands2
//    glPushMatrix();
//
//    glTranslated(0.1, -0.39, 0.18);
//
//    glBegin(GL_LINES);
//
//    glLineWidth(100);
//
//    glVertex3f(0.08, 0.94, 0.01);
//    glVertex3f(0.08, 0.94, 0.06);
//
//    glEnd();
//    glPopMatrix();


    //kom1
//    glPushMatrix();
//    glTranslated(0.16, -0.37, 0.19);
//    glBegin(GL_POLYGON);
//    glVertex3f(0.08, 0.93, 0.01);
//    glVertex3f(0.08, 0.93, 0.03);
//    glVertex3f(0.08, 0.95, 0.03);
//    glVertex3f(0.08, 0.95, 0.01);
//    glEnd();
//    glPopMatrix();

    //kom2
//    glPushMatrix();
//    glTranslated(0.1, -0.39, 0.18);
//    glBegin(GL_POLYGON);
//    glVertex3f(0.08, 0.93, 0.01);
//    glVertex3f(0.08, 0.93, 0.03);
//    glVertex3f(0.08, 0.95, 0.03);
//    glVertex3f(0.08, 0.95, 0.01);
//    glEnd();
//    glPopMatrix();

}



void key(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_DOWN:
            rotatePlayer=-90;
            if(boundsDown<0.3){
                boundsUp+=0.05;
                boundsDown+=0.05;
                moveFelX+=0.05;

            }
            break;
        case GLUT_KEY_UP:
            rotatePlayer=90;
            if(boundsUp>-0.3){
                boundsUp-=0.05;
                boundsDown-=0.05;
                moveFelX-=0.05;

            }glutPostRedisplay();
            break;
      
        case GLUT_KEY_RIGHT:
            
            rotatePlayer=0;

            if(boundsRight>-0.3){
                boundsRight-=0.05;
                boundsLeft-=0.05;
                moveFelZ-=0.05;
            }
            break;
            
        case GLUT_KEY_LEFT:
            
            rotatePlayer=180;

            
            if(boundsLeft<0.3){
                boundsRight+=0.05;
                boundsLeft+=0.05;
                moveFelZ+=0.05;
            }
            break;
             
        
        
        
            
    };
    
    
    
    
    glutPostRedisplay();
}



void Keyboard(unsigned char key, int x, int y) {
    float d = 0.01;
    float a = 1.0;

    switch (key) {
    case 'w':
        camera.moveY(d);
        break;
    case 's':
        camera.moveY(-d);
        break;
    case 'a':
        camera.moveX(d);
        break;
    case 'd':
        camera.moveX(-d);
        break;
    case 'q':
        camera.moveZ(d);
        break;
    case 'e':
        camera.moveZ(-d);
        break;
    case 49:  //press 1 for top view
        camera.topView();
        break;
    case 50:  //press 2 for side view
        camera.sideView();
        break;
    case 51: //press 3 for front view
        camera.frontView();
        break;
    case 'l': //press i to rotate X
        camera.rotateY(a);
        break;
    case 'j': //press j to rotate X
        camera.rotateY(-a);
        break;
    case 'i': //press i to rotate Y
        
        camera.rotateX(a);
        break;
    case 'k': //press 5 to rotate Y
        
        camera.rotateX(-a);
        break;

        case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }
    glutPostRedisplay();
}


void ballMoveHelper(int x){
    if (ballBounds>=0 ) {
        moveYaball+=0.05;
        ballBounds-=0.1;
        glutPostRedisplay();
        glutTimerFunc(100, ballMoveHelper,0);
        
    }
    
}


void ballMove(int x){
    if ( (0.455+moveFelX)>=0.45 && (0.455+moveFelX)<=0.5 && (0.41+moveFelZ)<=0.55 && (0.41+moveFelZ)>=0.5 )  {
        glutTimerFunc(100, ballMoveHelper,0);
    }

    if (ballBounds>=0 ) {
        glutTimerFunc(100, ballMove,0);
    }
    
}


void timesUp(int x){
    
        timer-=0.002;
        glutPostRedisplay();
        glutTimerFunc(100, timesUp,0);

}


void setupCamera() {
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    //glOrtho(-0.5, 0.5, -0.5, 0.5, -1, 1);
//
//    gluPerspective(60, 640 / 480, 0.001, 100);
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    
    
    //    gluLookAt(1.8, 1.5, 0.4, 0, 0.0, 0.4, 0.0, 1.0, 0.0); //--> top view
    //      gluLookAt(1.8, 0.6, 0.4, 0.4, 0.9, 0.4, 0.0, 1.0, 0.0); //--> front view
    //    gluLookAt(0.9, 1, -1.9, 0.9, 0.0, 1, 0.0, 1.0, 0.0); //--> side view
    
    
    //////////////(x camera position, Y camera position, z camera position, x center(elcamera lens basa 3ala eh), y ellens basa 3aleh, z ellens basa 3aleh)
    //awl 3 arkam dol eleye (position elcamera)\
    //tany talat arkam da elcenter -- ellense basa feen
    
    /////////// CAMERA ROTATE
   
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 640 / 480, 0.001, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        camera.look();

    
    
    
    
}




















//void RenderGround()
//{
//    glDisable(GL_LIGHTING);    // Disable lighting
//
//    glColor3f(0.6, 0.6, 0.6);    // Dim the ground texture a bit
//
//    glEnable(GL_TEXTURE_2D);    // Enable 2D texturing
//
//    glBindTexture(GL_TEXTURE_2D, tex_court.texture[0]);    // Bind the ground texture
//
//    glPushMatrix();
//    glBegin(GL_QUADS);
//    glNormal3f(0, 1, 0);    // Set quad normal direction.
//    glTexCoord2f(0, 0);        // Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
//    glVertex3f(-20, 0, -20);
//    glTexCoord2f(5, 0);
//    glVertex3f(20, 0, -20);
//    glTexCoord2f(5, 5);
//    glVertex3f(20, 0, 20);
//    glTexCoord2f(0, 5);
//    glVertex3f(-20, 0, 20);
//    glEnd();
//    glPopMatrix();
//
//    glEnable(GL_LIGHTING);    // Enable lighting again for other entites coming throung the pipeline.
//
//    glColor3f(1, 1, 1);    // Set material back to white instead of grey used for the ground texture.
//}


void Display() {
    setupLights();
    setupCamera();
    
    
    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslated(0.4, 0.0, 0.4);
    
    
//The Player
    glPushMatrix();
    glTranslated(moveFelX, 0, moveFelZ);
    drawPlayer();
    glPopMatrix();
    
    glPushMatrix();
    drawBall();
    drawGround1(0.6, 0.02, 0.02, 0.3);
    drawGround2(0.6*1.6, 0.02*1.6, 0.02*1.6, 0.29);
    glTranslated(0, 0, -0.1);
    drawBench();
    glPopMatrix();

    
//The Goal
       glPushMatrix();
       glTranslated(0, 0.3, 0.3);
       drawGoal(0.1/2,0.01/2, 0.2/2);
       glPopMatrix();
       glPushMatrix();
       glTranslated(0, 0.3, -0.3);
       drawGoal(0.1/2,0.01/2, 0.2/2);
       glPopMatrix();
       
//The Wall
       glPushMatrix();
       glTranslated(0, 0, -0.1);
       drawWall(0.02);
       drawScoreBoard();
       drawFlag();
        drawTimer();
       glPopMatrix();

    if(ballBounds==0){
        glRasterPos3f(-0.5, 0.8, 0.18); //define position on the screen
        
        char *string = "Match is OVER";
        
        while(*string){
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string++);
        }
    }
    

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("FIFA");
    glutDisplayFunc(Display);
    
    glutSpecialFunc(key);
    glutKeyboardFunc(Keyboard);
    
    
    
    glutTimerFunc(1, ballMove, 0);
    glutTimerFunc(1, timesUp, 0);
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glutMainLoop();
    
    
    //    PlaySound(TEXT("audio.wav"), NULL, ND_ASYNC|SND_FILENAME|SND_LOOP);
    
    //    private static SoundPlayer backgroundsound = new SoundPlayer("audio.wav");
    //            backgroundsound.Load();
    //            do{
    //
    //                c_wait(0.01);}while(!backgroundsound.IsLoadCompleted);
    //            backgroundsound.PlayLooping ();
    //            //rest of program runs here
    //            loaddata ();
    
    //    // start the sound engine with default parameters
    //      ISoundEngine* engine =createIrrKlangDevice();
    //    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    //      irrklang::ISoundEngine* engine = createIrrKlangDevice();
    
    
    //      if (!engine)
    //        return 0; // error starting up the engine
    //
    //      // play some sound stream, looped
    //      engine->play2D("Genius.mp3", true);
    //
    //      char i = 0;
    //      std::cin >> i; // wait for user to press some key
    //
    //      engine->drop(); // delete engine
    //      return 0;
    //
    
    //    PlaySound(("audio.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
    
    //MAC SOLUTION FOR AUDIO
    
    //    const int numSamples = 44100;
    //       float sampleRate = 44100.0f;
    //       float* sineWave = new float[numSamples];
    //       float frequency = 440.0f;
    //
    //       float radsPerSamp = 2.0f * 3.1415926536f * frequency / sampleRate;
    //
    //       for (unsigned long i = 0; i < numSamples; i++)
    //       {
    //           sineWave[i] = std::sin (radsPerSamp * (float) i);
    //       }
    //
    //       playSound(sineWave,numSamples,"audio.wav");
    //
    //       return 0;
    //}
}
