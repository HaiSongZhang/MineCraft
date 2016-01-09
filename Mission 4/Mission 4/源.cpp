#include "Param.h"
#include "visualBall.h"
//����
/* ����grab
* ץȡ�����е�����
* ���贰�ڿ��ΪWindowWidth���߶�ΪWindowHeight
*/
void grab(void)
{
	FILE*     pDummyFile;
	FILE*     pWritingFile;
	GLubyte* pPixelData;
	GLubyte   BMP_Header[BMP_Header_Length];
	GLint     i, j;
	GLint     PixelDataLength;

	// �����������ݵ�ʵ�ʳ���
	i = WindowWidth * 3;    // �õ�ÿһ�е��������ݳ���
	while (i % 4 != 0)       // �������ݣ�ֱ��i�ǵı���
		++i;                // �������и�����㷨��
							// �������׷��ֱ�ۣ����ٶ�û��̫��Ҫ��
	PixelDataLength = i * WindowHeight;

	// �����ڴ�ʹ��ļ�
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);

	pDummyFile = fopen("dummy.bmp", "rb");
	if (pDummyFile == 0)
		exit(0);

	pWritingFile = fopen("grab.bmp", "wb");
	if (pWritingFile == 0)
		exit(0);

	// ��ȡ����
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(0, 0, WindowWidth, WindowHeight,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	// ��dummy.bmp���ļ�ͷ����Ϊ���ļ����ļ�ͷ
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = WindowWidth;
	j = WindowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// д����������
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// �ͷ��ڴ�͹ر��ļ�
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}

/* ����power_of_two
* ���һ�������Ƿ�Ϊ2�������η�������ǣ�����1�����򷵻�0
* ʵ����ֻҪ�鿴�������λ���ж��ٸ������������1��������1�����򷵻�0
* �ڡ��鿴�������λ���ж��ٸ���ʱʹ����һ��С����
* ʹ��n &= (n-1)����ʹ��n�еļ���һ��������ԭ���ҿ����Լ�˼����
*/
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

/* ����load_texture
* ��ȡһ��BMP�ļ���Ϊ����
* ���ʧ�ܣ�����0������ɹ�������������
*/
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// ���ļ������ʧ�ܣ�����
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// �ھɰ汾��OpenGL��
	// ���ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������
	// ���ﲢû�м��OpenGL�汾�����ڶ԰汾�����ԵĿ��ǣ����ɰ汾����
	// ���⣬�����Ǿɰ汾�����°汾��
	// ��ͼ��Ŀ�Ⱥ͸߶ȳ�����ǰOpenGLʵ����֧�ֵ����ֵʱ��ҲҪ��������
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// ���µ������������������������
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�
	GLint lid = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lid);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	// ֮ǰΪpixels������ڴ����ʹ��glTexImage2D�Ժ��ͷ�
	// ��Ϊ��ʱ���������Ѿ���OpenGL���б�����һ�ݣ����ܱ����浽ר�ŵ�ͼ��Ӳ���У�
	free(pixels);
	return texture_ID;
}

void moveMouse()
{
	INPUT mouseInput;
	ZeroMemory(&mouseInput, sizeof mouseInput);
	mouseInput.type = INPUT_MOUSE;
	mouseInput.mi.dx = 32767;
	mouseInput.mi.dy = 32767;
	mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &mouseInput, sizeof(mouseInput));
}

void human()
{
	// ��
	glPushMatrix();
	glTranslatef(man.x, man.y, man.z);
	glRotatef(man.vangle, 0, 1, 0);

	////////////////////////////Hair//////////////////////////
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(man.hair.x, man.hair.y, man.hair.z);
	glScalef(man.hair_scale.x, man.hair_scale.y, man.hair_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	///////////////////////////Head////////////////////////////
	// head
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.head.x, man.head.y, man.head.z);
	glScalef(man.head_scale.x, man.head_scale.y, man.head_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// left eye
	glColor3f(0.9f, 0.9f, 0.9f);
	glPushMatrix();
	glTranslatef(man.eyeL.x, man.eyeL.y, man.eyeL.z);
	glScalef(man.eye_scale.x, man.eye_scale.y, man.eye_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// right eye
	glColor3f(0.9f, 0.9f, 0.9f);
	glPushMatrix();
	glTranslatef(man.eyeR.x, man.eyeR.y, man.eyeR.z);
	glScalef(man.eye_scale.x, man.eye_scale.y, man.eye_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// mouth
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(man.mouth.x, man.mouth.y, man.mouth.z);
	glScalef(man.mouth_scale.x, man.mouth_scale.y, man.mouth_scale.z);
	glutSolidCube(1);
	glPopMatrix();


	///////////////////////////Body////////////////////////////
	glColor3f(0.0f, 206.0 / 255, 209.0 / 255);
	glPushMatrix();
	glTranslatef(man.body.x, man.body.y, man.body.z);
	glScalef(man.body_scale.x, man.body_scale.y, man.body_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	/////////////////////////Leg/////////////////////////
	//Left leg
	glColor3f(67.0 / 255, 110.0 / 255, 238.0 / 255);
	glPushMatrix();
	glTranslatef(man.thighL.x, 2 * man.thighL.y, man.thighL.z);
	glRotatef(man.ltangle, 0, 0, 1);
	glTranslatef(0, -man.thighR.y, 0);
	glScalef(man.thigh_scale.x, man.thigh_scale.y, man.thigh_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	//Right leg
	glColor3f(67.0 / 255, 110.0 / 255, 238.0 / 255);
	glPushMatrix();
	glTranslatef(man.thighR.x, 2 * man.thighR.y, man.thighR.z);
	glRotatef(man.rtangle, 0, 0, 1);
	glTranslatef(0, -man.thighR.y, 0);
	glScalef(man.thigh_scale.x, man.thigh_scale.y, man.thigh_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	///////////////////////Arm////////////////////////

	//Left Arm
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.armL.x, man.arm_scale.y / 2 + man.armL.y, man.armL.z);
	glRotatef(man.laangle, 0, 0, 1);
	glTranslatef(0, -man.arm_scale.y / 2, 0);
	glScalef(man.arm_scale.x, man.arm_scale.y, man.arm_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	//Right Arm
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.armR.x, man.arm_scale.y / 2 + man.armR.y, man.armR.z);
	glRotatef(man.raangle, 0, 0, 1);
	glTranslatef(0, -man.arm_scale.y / 2, 0);
	glScalef(man.arm_scale.x, man.arm_scale.y, man.arm_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
}

void reference()
{
	glColor3f(0.3f, 0.2f, 0.3f);
	glPushMatrix();

	glScalef(1, 4, 1);
	glutSolidCube(1.2);
	glPopMatrix();
}

void floor()
{
	glColor3f(0.3f, 1.0f, 0.3f);
	/*for (int i = 0; i < NUM_OF_LINE * 2; i++)
	{
		glBegin(GL_LINES);
		glVertex3f((NUM_OF_LINE - i) * BLOCK_SIZE, 0, NUM_OF_LINE * BLOCK_SIZE);
		glVertex3f((NUM_OF_LINE - i) * BLOCK_SIZE, 0, -NUM_OF_LINE * BLOCK_SIZE);
		glVertex3f(NUM_OF_LINE * BLOCK_SIZE, 0, (NUM_OF_LINE - i) * BLOCK_SIZE);
		glVertex3f(-NUM_OF_LINE * BLOCK_SIZE, 0, (NUM_OF_LINE - i) * BLOCK_SIZE);
		glEnd();
	}*/
	float BASIC_SIZE = 1.0;
	glBindTexture(GL_TEXTURE_2D, texGround);
	int num_cube = 20;
	float single_cube_size = 0.5f;
	float px = 1.0f;
	for (int i = num_cube;i >= (-num_cube);i--)
	{
		for (int j = num_cube;j >= (-num_cube);j--)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-single_cube_size + i*single_cube_size, 0, -single_cube_size + j*single_cube_size);
			glTexCoord2f(0.0f, px); glVertex3f(-single_cube_size + i*single_cube_size,0, single_cube_size + j*single_cube_size);
			glTexCoord2f(px, px); glVertex3f(single_cube_size + i*single_cube_size, 0, single_cube_size + j*single_cube_size);
			glTexCoord2f(px, 0.0f); glVertex3f(single_cube_size + i*single_cube_size,0, -single_cube_size + j*single_cube_size);
			glEnd();
		}

	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ��Դ̫��
void sun()
{
	glPushMatrix();
	glRotated(s_angle, 1, 0, 0);
	glTranslatef(0.0f, 5.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);		//���û�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);		//���������
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);	//�����
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);	//���ù�Դλ��

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//���ư�ɫ��̫��
	MatShininess = 0.01f;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);

	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere(0.2, 40, 40);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	sun();

	glDisable(GL_BLEND);
	floor();
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	matDiffuseR = 1.0f, matDiffuseG = 1.0f, matDiffuseB = 0.0f;
	matSpecularR = 0.5f, matSpecularG = 0.5f, matSpecularB = 0.5f;
	MatShininess = 20.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
	reference();

	matDiffuseR = 0.2f, matDiffuseG = 0.2f, matDiffuseB = 0.0f;
	matSpecularR = 1.0f, matSpecularG = 1.0f, matSpecularB = 1.0f;
	MatShininess = 130.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
	human();

	glutSwapBuffers();

	//�ͷ������ڴ�
	grab();

	if (view_person == FIRST_PERSON)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cameraAt.x = man.x;
		cameraAt.y = man.head.y;
		cameraAt.z = man.z;

		lookAt.x = man.x + 5 * cos(man.vangle / 180.0 * PI);
		lookAt.y = man.hair.y;
		lookAt.z = man.z - 5 * sin(man.vangle / 180.0 * PI);

		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == GOD)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (!reset_god)
		{
			cameraAt = godAt;
			lookAt.x = 0;
			lookAt.y = 0;
			lookAt.z = 0;
			reset_god = true;
		}

		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == THIRD_PERSON)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cameraAt.x = man.x - 10 * cos(man.vangle / 180.0 * PI);
		cameraAt.y = man.head.y + 10;
		cameraAt.z = man.z + 10 * sin(man.vangle / 180.0 * PI);

		lookAt.x = man.x;
		lookAt.y = man.hair.y;
		lookAt.z = man.z;

		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == BALL)
	{
		if (trackballMove)
		{
			glRotatef(angle, axis[0], axis[1], axis[2]);
		}
	}
}

void init()
{
	glClearColor(0.0f, 0.25f, 0.4f, 0.0f);

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);        // Do not calculate inside of jet
	glFrontFace(GL_CCW);        // Counter clock-wise polygons face out

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_COLOR_MATERIAL);
}

void refresh(int c)
{
	//center.y = -(1 - sin(-left_thigh_angle / 180.0 * PI)) * basic_size * THIGH_SCALE_X + basic_size * FOOT_SCALE_Y / 2;
	if (man.isMoving())
	{
		if (man.ltangle < -60)
		{
			left_forward = false;
		}
		else if (man.ltangle > 60)
		{
			left_forward = true;
		}
		if (man.rtangle < -60)
		{
			right_forward = false;
		}
		else if (man.rtangle > 60)
		{
			right_forward = true;
		}
		if (left_forward)
		{
			man.ltangle--;
			man.laangle++;
		}
		else
		{
			man.ltangle++;
			man.laangle--;
		}
		if (right_forward)
		{
			man.rtangle--;
			man.raangle++;
		}
		else
		{
			man.rtangle++;
			man.raangle--;
		}

	}
	else
	{
		man.raangle = 0;
		man.laangle = 0;
		man.rtangle = 0;
		man.ltangle = 0;
	}
	glutTimerFunc(DELAY, refresh, 0);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	scr_w = w;
	scr_h = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0, (GLfloat)scr_w / scr_h, near_sight, far_sight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// s�����лص�����
void passiveMotion(int x, int y)
{
	//man.vangle = (man.vangle - (x - scr_w / 2) / 150);
	man.vangle -= 0.1*(x - last_mm_x);
	cout << man.vangle << endl;
	last_mm_x = x;
	last_mm_y = y;
	//moveMouse();
}
// ���������
void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:
			//y = scr_h - y;
			startMotion(x, y);
			break;
		case GLUT_UP:
			stopMotion(x, y);
			break;

		}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		// �ӽ�����
		if (state == GLUT_DOWN && far_sight / VIEW_SCALE <1000)
		{
			/*near_sight *= VIEW_SCALE;
			far_sight *= VIEW_SCALE;*/
			cameraAt.x *= VIEW_SCALE;
			cameraAt.y *= VIEW_SCALE;
			cameraAt.z *= VIEW_SCALE;
		}
	}
}
// ����ƶ��ص�����
void mouseMotion(int x, int y)
{

	trackball_ptov(x, y, scr_w, scr_h, curPos);
	trackMouse();
	glutPostRedisplay();
}
// ���лص�����
void idle()
{
	// �����귢���ƶ���������Զ�ת��
	if (redrawContinue) glutPostRedisplay();
}
// ��������
void control(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		man.move = true;
		man.x -= man.speed * sin(man.vangle / 180 * PI);
		man.z -= man.speed * cos(man.vangle / 180 * PI);
		break;
	case 'd':
		man.move = true;
		man.x += man.speed * sin(man.vangle / 180 * PI);
		man.z += man.speed * cos(man.vangle / 180 * PI);
		break;
	case 'w':
		man.move = true;
		man.x += man.speed * cos(man.vangle / 180 * PI);
		man.z -= man.speed * sin(man.vangle / 180 * PI);
		break;
	case 's':
		man.move = true;
		man.x -= man.speed * cos(man.vangle / 180 * PI);
		man.z += man.speed * sin(man.vangle / 180 * PI);
		break;
	case 'h':
		reset_god = false;
		if (view_person >= 3)
		{
			view_person = 0;
		}
		else
		{
			view_person++;
		}
		break;
	case 'r':
		if (view_person != 3)
		{
			last_view = view_person;
			view_person = 3;
		}
		else
		{
			view_person = last_view;
		}
		break;
	case 'o':
		s_angle++;
		glutPostRedisplay();
		break;
	case 'p':
		s_angle--;
		glutPostRedisplay();
		break;
	case '[':
		ambient += 0.1;
		glutPostRedisplay();
		break;
	case ']':
		ambient -= 0.1;
		glutPostRedisplay();
		break;
	case ';':
		diffuse += 0.1;
		glutPostRedisplay();
		break;
	case '\'':
		diffuse -= 0.1;
		glutPostRedisplay();
		break;
	case ',':
		specular += 0.1;
		glutPostRedisplay();
		break;
	case '.':
		specular -= 0.1;
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
	default:
		break;
	}

}
// �����ɿ�
void controlup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		man.move = false;
		break;
	case 's':
		man.move = false;
		break;
	case 'a':
		man.move = false;
		break;
	case 'd':
		man.move = false;
		break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Passenger and plane");
	init();

	//�����������
	//����2D����Ͳ��ʳ�ʼ��
	glEnable(GL_TEXTURE_2D);
	// ��������һЩ��ʼ��
	glEnable(GL_DEPTH_TEST);
	texGround = load_texture("ground.bmp");
	texWall = load_texture("wall.bmp");
	glTexParameterf(GL_NEAREST, GL_TEXTURE_WRAP_S, GL_REPEAT);


	refresh(0);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(passiveMotion);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardUpFunc(controlup);
	glutKeyboardFunc(control);
	glutMainLoop();
	return 0;
}

