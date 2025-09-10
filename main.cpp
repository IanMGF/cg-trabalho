#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include "body.cpp"

#define G 6.67430e-11
#define DELTA 2
int steps = 250;
int factor = 800;

Vec3 camera_pos = Vec3(0, 25000 , 0);
Vec3 camera_target = Vec3(0, 0, 0);
Vec3 camera_up = Vec3(0, 0, 1);
float camera_angle = 0.0;

float sun_step_change_timer = 0;
int sun_explosion_step = 0;
float sun_scale = 1;
float sun_mass = 4e11;
Vec3 sun_color = Vec3(1, 1, 0.4);

GLfloat fAspect;

Body bodies[10] = {
    // Sun
    Body(2000, 0, sun_mass, sun_color),

    // Planets

    // Ash Twin
    Body(169, Vec3(-250, 0, 5000), 1.6E+6, Vec3(0.85, 0.31, 0.11), Vec3(0.07307147186145904, 0, 0 )),
    // Ember Twin
    Body(170, Vec3(250, 0, 5000), 1.6E+6, Vec3(0.89, 0.62, 0.29), Vec3(0.07307147186145904, 0, 0)),

    // Timber Heart
    Body(254, 8600, 3E+7, Vec3(0.22, 0.38, 0.27)),
    // Attlerock
    Body(80, Vec3(900, 0, 8600), 5E+5, Vec3(0.62, 0.49, 0.42), Vec3(0.0557164, 0, 0.001491565173)),

    // Brittle Hollow
    Body(272, 11700, 3E+7, Vec3(0.31, 0.26, 0.34)),
    // Hollow's Lantern
    Body(97, Vec3(1000, 0, 11700), 9.1E+5, Vec3(1, 0.75, 0.13), Vec3(0.04776831, 0, 0.001415022)),

    // Giant's Deep
    Body(500, 16460, 2.2E+7, Vec3(0.11, 0.42, 0.27)),

    // Dark Bramble
    Body(203.3, 20000, 3.25E+6, Vec3(0.3, 0.13, 0.12)),

    // Interloper
    Body(83, 19000, 5.5E+6, Vec3(0.13, 0.48, 0.62)),

    // Quantum Moon
    // Body(100, 0, 0, Vec3(1, 1, 1)),
};

void update(int _) {
    float effective_delta = DELTA * factor * 0.001;
    for (int _step = 0; _step < steps; _step++){
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j++) {
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

	//Desenha objetos 3D (wire-frame)
	for (int i=0; i<10; i++) {
	    Body body = bodies[i];
		
	    glColor3f(body.color.x, body.color.y, body.color.z);
		glPushMatrix();
			glTranslated(body.position.x, body.position.y, body.position.z);
			
			if (i == 0 && sun_explosion_step != 0) {
			    // Sun scaling
			    glScalef(sun_scale, sun_scale, sun_scale);
			}
			
			glutSolidSphere(body.radius, 50, 50);
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

	glutSwapBuffers();
}

void setup(void)
{
    // Slightly blue tone
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

    glEnable(GL_DEPTH_TEST);
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

	// Especifica posição do observador e do alvo
	gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,  // posição da câmera
              0, 0, 0,          // posição do alvo
              camera_up.x, camera_up.y, camera_up.z);         // vetor UP da câmera
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
			camera_pos.x = 0;
            camera_pos.y = 25000;
            camera_pos.z = 0;
            camera_angle = 0.0;
			break;

		// Movimentação da câmera
		case 'w':
		    camera_pos.x += 1000;
			break;
		case 'a':
		    camera_pos.z += 1000;
			break;
		case 's':
            camera_pos.x -= 1000;
            break;
		case 'd':
		    camera_pos.z -= 1000;
			break;

		case 'q':
		    camera_angle += 0.03;
			camera_up = Vec3(sin(camera_angle), 0, cos(camera_angle));
			break;
		case 'e':
		    camera_angle -= 0.03;
			camera_up = Vec3(sin(camera_angle), 0, cos(camera_angle));
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
    setup();
	glutMainLoop();

}
