#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include "body.cpp"
#include <cstdlib>
#include <ctime>

#define C_LENGHT 7
#define G 6.67430e-11
#define DELTA 2
int steps = 250;
int factor = 800;

struct dark_bramble_cylinder{
    GLdouble x;
    GLdouble y;
    GLdouble angle_rotation;
    GLdouble height;
    Vec3 scale_factor;
};

typedef struct Camera {
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float angle;
    float distance_to_target;
} Camera;

Camera cam = {
    Vec3(0, 25000, 0),
    Vec3(0, 0, 0),
    Vec3(0, 0, 1),
    0.0,
    25000
};

float sun_step_change_timer = 0;
int sun_explosion_step = 0;
float sun_scale = 1;
float sun_mass = 4e11;
Vec3 sun_color = Vec3(1, 1, 0.4);

dark_bramble_cylinder cylinders[C_LENGHT];

GLfloat fAspect;

std::vector<Body> bodies = std::vector<Body>();

void physics_setup() {
    bodies.push_back(Body(2000, 0, sun_mass, sun_color));
    bodies.push_back(Body(169, Vec3(-250, 0, 5000), 1.6E+6, Vec3(0.85, 0.31, 0.11), Vec3(0.07307147186145904, 0, 0)));
    bodies.push_back(Body(170, Vec3(250, 0, 5000), 1.6E+6, Vec3(0.89, 0.62, 0.29), Vec3(0.07307147186145904, 0, 0)));
    bodies.push_back(Body(254, 8600, 3E+7, Vec3(0.22, 0.38, 0.27)));
    bodies.push_back(Body(80, Vec3(900, 0, 8600), 5E+5, Vec3(0.62, 0.49, 0.42), Vec3(0.0557164, 0, 0.001491565173)));
    bodies.push_back(Body(272, 11700, 3E+7, Vec3(0.31, 0.26, 0.34)));
    bodies.push_back(Body(97, Vec3(1000, 0, 11700), 9.1E+5, Vec3(1, 0.75, 0.13), Vec3(0.04776831, 0, 0.001415022)));
    bodies.push_back(Body(500, 16460, 2.2E+7, Vec3(0.11, 0.42, 0.27)));
    bodies.push_back(Body(200, 20000, 3.25E+6, Vec3(0.3, 0.13, 0.12)));
    bodies.push_back(Body(83, 19000, 5.5E+6, Vec3(0.13, 0.48, 0.62)));
};

void update(int _) {
    float effective_delta = DELTA * factor * 0.001;
    for (int _step = 0; _step < steps; _step++){
        for (int i = 0; i < bodies.size(); i++) {
            for (int j = i + 1; j < bodies.size(); j++) {
                float acc_factor = G / pow((bodies[i].position - bodies[j].position).length(), 2);
                Vec3 acc = (bodies[j].position - bodies[i].position).unitary() * acc_factor;

                Vec3 step = acc * effective_delta;

                bodies[i].velocity = bodies[i].velocity + step * bodies[j].mass;
                bodies[j].velocity = bodies[j].velocity - step * bodies[i].mass;
            }

            bodies[i].position = bodies[i].position + bodies[i].velocity * effective_delta;
        }
    }

    if (sun_explosion_step != 0) {
        float effective_delta = DELTA * 0.001;
        sun_step_change_timer += effective_delta;
        switch(sun_explosion_step) {
            case 1:
                sun_scale += 1 * effective_delta / 2;
                sun_color = sun_color + (Vec3(1, 0, 0) - sun_color) * (effective_delta / 2);
                sun_mass = sun_mass + (1e11 * effective_delta / 2);
                if (sun_step_change_timer > 2) {
                    sun_step_change_timer = 0;
                    sun_explosion_step = 2;
                }
                break;
            case 2:
                sun_scale -= (2 - 0.1) * effective_delta;
                sun_color = sun_color + (Vec3(1, 1, 1) - sun_color) * (effective_delta / 1);
                sun_mass = sun_mass + (2e11 * effective_delta / 1);
                if (sun_step_change_timer > 1) {
                    sun_step_change_timer = 0;
                    sun_explosion_step = 3;
                }
                break;
            case 3:
                sun_mass = sun_mass - 5e10 * effective_delta;
                sun_color = sun_color + (Vec3(0.29, 0.78, 0.99) - sun_color) * effective_delta;
                sun_scale += 2 * effective_delta;
                break;
        }
        bodies[0].mass = sun_mass;
        bodies[0].color = sun_color;
    }
    glutPostRedisplay();
    glutTimerFunc(DELTA, update, 0);
}

void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i=0; i<bodies.size(); i++) {
	    Body body = bodies[i];
	    glColor3f(body.color.x, body.color.y, body.color.z);
		glPushMatrix();
			glTranslated(body.position.x, body.position.y, body.position.z);

		    const float ONES[] = {1.0f, 1.0f, 1.0f, 1.0f};
			const float ZERO[] = {0.0f, 0.0f, 0.0f, 1.0f};
			const float SUN_COLOR[] = { sun_color.x, sun_color.y, sun_color.z, 1.0f };
		
			if (i == 0) {
				glDisable(GL_LIGHT0);
				glEnable(GL_LIGHT1);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, SUN_COLOR);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ZERO);
			    glScalef(sun_scale, sun_scale, sun_scale);
			}

			glutSolidSphere(body.radius, 100, 100);

			if (i == 0) {
				glDisable(GL_LIGHT1);
				float sun_diffuse[] = {sun_color.x, sun_color.y, sun_color.z, 1.0f};
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ZERO);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
				glEnable(GL_LIGHT0);
			}
		glPopMatrix();
	}

	Vec3 cylinder_path = bodies[2].position - bodies[1].position;
	float angle_rad = std::acos(cylinder_path.z / cylinder_path.length());
	float angle_deg = angle_rad * 180 / M_PI;

	GLUquadric* quad = gluNewQuadric();
	glColor3f(0.89, 0.62, 0.29);
	glPushMatrix();
        glTranslated(bodies[1].position.x, bodies[1].position.y, bodies[1].position.z);

        glRotated(angle_deg, cylinder_path.y, cylinder_path.x, 0);

        gluCylinder(quad, 50000 / cylinder_path.length(), 50000 / cylinder_path.length(), cylinder_path.length(), 20, 20);
    glPopMatrix();

    glPushMatrix();
        glTranslated(bodies[8].position.x, bodies[8].position.y, bodies[8].position.z);
        for(int i = 0; i < C_LENGHT; i++){
           	glColor3f(0.3, 0.13, 0.12);

            glRotated(cylinders[i].angle_rotation, cylinders[i].x, cylinders[i].y, 0);

            gluCylinder(quad, 25, 25, 300, 20, 20);

            glTranslated(0, 0, cylinders[i].height);
            glScaled(cylinders[i].scale_factor.x, cylinders[i].scale_factor.y, cylinders[i].scale_factor.z);

           	glColor3f(0.13, 0.48, 0.62);

            glutSolidTetrahedron();

            glScaled(1/cylinders[i].scale_factor.x, 1/cylinders[i].scale_factor.y, 1/cylinders[i].scale_factor.z);
            glTranslated(0, 0, -cylinders[i].height);

            glRotated(-cylinders[i].angle_rotation, cylinders[i].x, cylinders[i].y, 0);
        }

        glColor3f(0.64, 0.64, 0.64);

        // esfera 1
        glTranslated(125, 0, 0);
        glutSolidSphere(100, 50, 50);
        glTranslated(-125, 0, 0);

        // esfera 2
        glTranslated(-125, 0, 0);
        glutSolidSphere(100, 50, 50);
        glTranslated(125, 0, 0);

        // esfera 3
        glTranslated(0, 125, 0);
        glutSolidSphere(100, 50, 50);
        glTranslated(0, -125, 0);

        // esfera 4
        glTranslated(0, -125, 0);
        glutSolidSphere(100, 50, 50);
        glTranslated(0, 125, 0);

        // esfera 5
        glTranslated(0, 0, 125);
        glutSolidSphere(100, 50, 50);
        glTranslated(0, 0, -125);

        // esfera 6
        glTranslated(0, 0, -125);
        glutSolidSphere(100, 50, 50);
        glTranslated(0, 0, 125);
    glPopMatrix();

	glutSwapBuffers();
}

void setup(void)
{
    const float ZEROES[4] = {0.0, 0.0, 0.0, 0.0};
    const float ONES[4] = {1.0, 1.0, 1.0, 1.0};

    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ONES);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ZEROES);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ZEROES);
    glLightfv(GL_LIGHT0, GL_SPECULAR, ZEROES);

    glLightfv(GL_LIGHT1, GL_EMISSION, ONES);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    for(int i=0; i < C_LENGHT; i++){
        cylinders[i].scale_factor.x = ((((float)rand() / RAND_MAX) * 2) + 1) * 70;
        cylinders[i].scale_factor.y = ((((float)rand() / RAND_MAX) * 2) + 1) * 70;
        cylinders[i].scale_factor.z = ((((float)rand() / RAND_MAX) * 2) + 1) * 70;

        cylinders[i].height = (((float)rand() / RAND_MAX * 0.6) + 1) * 250;

        cylinders[i].angle_rotation = (float)rand() / (float)RAND_MAX * 180;

        GLdouble angle_rad = ((float)rand() / (float)RAND_MAX - 0.5) * 2 * M_PI;
        cylinders[i].x = std::sin(angle_rad);
        cylinders[i].y = std::cos(angle_rad);
    }
}

// Função usada para especificar o volume de visualização
void view_setup(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();
	// Especifica a projeção perspectiva
	gluPerspective(90, fAspect, 1, 1E+9);

	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	const float sun_position[4] = {0, 0, 0, 1};

	// Especifica posição do observador e do alvo
	gluLookAt(
		cam.position.x, cam.position.y, cam.position.z,
        cam.target.x, cam.target.y, cam.target.z,
        cam.up.x, cam.up.y, cam.up.z
	);

	glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
}

// Função callback chamada quando o tamanho da janela é alterado
void window_change_callback(GLint new_width, GLint new_height)
{
	if (new_height == 0) new_height = 1;

	glViewport(0, 0, new_width, new_height);
	fAspect = (GLfloat) new_width / (GLfloat) new_height;

	view_setup();
}

// Função callback chamada para gerenciar teclas especiais
void special_keys_callback(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		// ycamera += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		// ycamera -= 10;
	}
	/*
	if (key == GLUT_KEY_RIGHT) {

	}
	if (key == GLUT_KEY_LEFT) {

	}
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    view_setup();
	// glutPostRedisplay();
}
// Função callback chamada para gerenciar teclado
void keyboard_callback(unsigned char key_code, int x, int y) {
	switch (key_code) {
		case 'r':
			cam.position.x = 0;
            cam.position.y = 25000;
            cam.position.z = 0;
            cam.angle = 0.0;
			break;

		// Movimentação da câmera
		case 'w':
		    cam.position.x += 1000;
			break;
		case 'a':
		    cam.position.z += 1000;
			break;
		case 's':
            cam.position.x -= 1000;
            break;
		case 'd':
		    cam.position.z -= 1000;
			break;

		case 'q':
		    cam.angle += 0.03;
			cam.up = Vec3(sin(cam.angle), 0, cos(cam.angle));
			break;
		case 'e':
		    cam.angle -= 0.03;
			cam.up = Vec3(sin(cam.angle), 0, cos(cam.angle));
			break;

		case 'x':
            sun_explosion_step = 1;
            break;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    view_setup();
	glutPostRedisplay();
}


// Função callback chamada para gerenciar eventos do mouse
void mouse_callback(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON){

	}
	if (button == GLUT_RIGHT_BUTTON){

	}
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view_setup();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
    srand(time(0));

    glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);  //GLUT_DOUBLE trabalha com dois buffers: um para renderização e outro para exibição

    glutInitWindowPosition(700,100);
	glutInitWindowSize(1280, 720);

	fAspect = (GLfloat) 1280 / (GLfloat) 720;
    glutCreateWindow("Sistema Solar");

    glutTimerFunc(DELTA, update, 0);
	glutDisplayFunc(draw);
	glutReshapeFunc(window_change_callback); // Função para ajustar o tamanho da tela
    //glutMouseFunc(GerenciaMouse);
    glutKeyboardFunc(keyboard_callback); // Define qual funcao gerencia o comportamento do teclado
    glutSpecialFunc(special_keys_callback); // Define qual funcao gerencia as teclas especiais

    // Start
    physics_setup();
    setup();
	glutMainLoop();

}
