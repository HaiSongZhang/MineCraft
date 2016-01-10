#pragma once

#include <iostream>
#include <cmath>
#include <windows.h>
#include <gl/GLUT.h>
#include "Point.h"
#include "Crawler.h"
#include "Man.h"
#include "Cube.h"
#include "TexCube.h"
using namespace std;

#define DELAY 12
#define NUM_OF_LINE 32
#define BLOCK_SIZE 1
#define VIEW_SCALE 2
#define PI 3.1415926535898


//������곣��
//const double INCREMENT = 0.05;

// �������
Point center = Point(0, 0, 0);
Point lookAt = Point(5, 5, 5);
Point cameraAt = Point(5, 5, 5);
Point godAt = Point(5, 5, 5);

float last_mm_x = 0;
float last_mm_y = 0;

bool reset_god = false;
int view_person = 0;  // 0 �ϵ��ӽǣ�1 ��һ�˳ƣ�2 �����˳�, ������
int last_view = 0;
enum { GOD, FIRST_PERSON, THIRD_PERSON, BALL };
enum { HEAD, HAIR, BODY, THIGH_L, THIGH_R, ARM_L, ARMR, EYE_L, EYE_R, MOUTH };

float near_sight = 0.1f;
float far_sight = 100;
int scr_w;
int scr_h;

// ����flag
bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;

// �ӽ�ת������
float angle = 0.0, axis[3], trans[3];
float lastPos[3] = { 0.0F, 0.0F, 0.0F };
int curx, cury;
int startX, startY;


// ����
Man man;
//Crawler crawler;

//����������
bool left_forward = true;
bool right_forward = false;
bool look_from_left = true;



//��Դ̫��
GLfloat s_angle = 0.0f;
GLfloat LightPosition[] = { 0.0f,0.0f,0.0f,1.0f };		//��Դλ��

GLfloat ambient = 0.2f;
GLfloat LightAmbient[] = { ambient,ambient,ambient,1.0f };		//������
GLfloat diffuse = 0.2f;
GLfloat LightDiffuse[] = { diffuse,0,0,1.0f };		//������
GLfloat specular = 0.2f;
GLfloat LightSpecular[] = { specular,0.6f,0.6f,1.0f };	//���淴��

														//���ϲ���
GLfloat matEmissionR, matEmissionG, matEmissionB;
GLfloat	MatEmission[] = { matEmissionR, matEmissionG, matEmissionB, 1.0f };
GLfloat matAmbientR, matAmbientG, matAmbientB;
GLfloat	MatAmbient[] = { matAmbientR, matAmbientG, matAmbientB, 1.0f };
GLfloat matDiffuseR, matDiffuseG, matDiffuseB;
GLfloat MatDiffuse[] = { matDiffuseR, matDiffuseG, matDiffuseB, 1.0f };
GLfloat matSpecularR, matSpecularG, matSpecularB;
GLfloat MatSpecular[] = { matSpecularR, matSpecularG, matSpecularB, 1.0f };
GLfloat MatShininess;

//���ڴ�С
int WindowWidth = 600;
int WindowHeight = 600;
#include"Lable.h"



