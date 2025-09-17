#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include "body.cpp"


#define C_LENGHT 7
#define G 6.67430e-11
#define DELTA 2

int steps = 100;
int factor = 1000;

struct SupernovaParticle {
    Vec3 position;          // Posição atual no espaço 3D
    Vec3 color;             // Cor da partícula
    float lifetime;         // Tempo de vida restante
    float age;              // Idade atual

    // Curva Bézier cúbica
    Vec3 p0, p1, p2, p3;   // Pontos de controle
    float t;                // Parâmetro da curva (0 a 1)
    float speed;            // Velocidade ao longo da curva

    std::vector<Vec3> trail; // histórico de posições
};

const int NUM_PARTICLES = 2000;
SupernovaParticle particles[NUM_PARTICLES];

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
} Camera;

#define INITIAL_CAMERA_POSITION Vec3(25000, 5000, 0)
#define INITIAL_CAMERA_UP Vec3(0, 1, 0)

Camera cam = {
    INITIAL_CAMERA_POSITION,
    Vec3(0, 0, 0),
    INITIAL_CAMERA_UP,
};

float sun_step_change_timer = 0;
int sun_explosion_step = 0;
float sun_scale = 1;
float sun_mass = 4e11;
Vec3 sun_color = Vec3(1, 1, 0.4);

dark_bramble_cylinder cylinders[C_LENGHT];

GLfloat fAspect;

std::vector<Body> bodies = std::vector<Body>();

SupernovaParticle generate_particle(void) {
    SupernovaParticle particle;
    particle.age = 0.0f;
    particle.lifetime = 2.0f + ((float)rand()) / (RAND_MAX / 3.0f); // 2 a 5 segs
    particle.t = 0.0f;
    particle.speed = 0.1f + ((float)rand()) / (RAND_MAX / 1.5f);

    // Posição inicial no Sol
    Vec3 center = bodies[0].position;

    float distance_from_sun = bodies[0].radius * ((float) rand() / RAND_MAX) * 0.4;
    float wave_amplitude = bodies[0].radius * (1 + ((float) rand() / RAND_MAX));

    float escape_elevation = 2 * M_PI * ((float) rand() / RAND_MAX);
    float escape_azimuth = 2 * M_PI * ((float) rand() / RAND_MAX);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> standard_normal_dist(0.0, 1.0);

    Vec3 midpoint = Vec3(
        standard_normal_dist(generator),
        standard_normal_dist(generator),
        standard_normal_dist(generator)
    ).unitary() * distance_from_sun / 2;
    Vec3 surface_point = midpoint.unitary() * bodies[0].radius;
    Vec3 far_point = midpoint * 2;

    // Generate a pull point perpendicular to the midpoint
    Vec3 aux_vec = midpoint + Vec3(
        -midpoint.x,
        midpoint.y,
        0
    );
    Vec3 perpendicular = aux_vec.cross(midpoint).unitary();
    Vec3 pull_point = perpendicular * wave_amplitude * 2;

    // Define pontos de controle para curvas Bézier saindo do Sol
    Vec3 p1_delta = midpoint + pull_point;
    Vec3 p2_delta = midpoint - pull_point;
    Vec3 p3_delta = midpoint * 2;

    particle.p0 = center + surface_point;
    particle.p1 = center + surface_point+ p1_delta;
    particle.p2 = center + surface_point+ p2_delta;
    particle.p3 = center + surface_point+ p3_delta;

    // Cor inicial
    particle.color = Vec3(1, 1, 1);
    return particle;
}

void reset_supernova() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i] = generate_particle();
    }
}

void physics_setup() {
    Body comet = Body(500, Vec3(0, 0, 100000), 5.5E+6, Vec3(0.13, 0.48, 0.62), Vec3(0, 0, 0));
    float semi_major_axis = 60000.0;
    float inv_a = 1 / semi_major_axis;
    float initial_speed = std::sqrt(G * (comet.mass + sun_mass) * (2 / 100000.0 - inv_a));

    comet.velocity = Vec3(initial_speed, 0, 0);

    bodies.push_back(Body(2000, 0, sun_mass, sun_color));
    bodies.push_back(Body(169, Vec3(-250, 0, 5000), 1.6E+6, Vec3(0.85, 0.31, 0.11), Vec3(0.07307147186145904, 0, 0)));
    bodies.push_back(Body(170, Vec3(250, 0, 5000), 1.6E+6, Vec3(0.89, 0.62, 0.29), Vec3(0.07307147186145904, 0, 0)));
    bodies.push_back(Body(254, 8600, 3E+7, Vec3(0.22, 0.38, 0.27)));
    bodies.push_back(Body(80, Vec3(900, 0, 8600), 5E+5, Vec3(0.62, 0.49, 0.42), Vec3(0.0557164, 0, 0.001491565173)));
    bodies.push_back(Body(272, 11700, 3E+7, Vec3(0.31, 0.26, 0.34)));
    bodies.push_back(Body(97, Vec3(1000, 0, 11700), 9.1E+5, Vec3(0.8, 0.3, 0.13), Vec3(0.04776831, 0, 0.001415022)));
    bodies.push_back(Body(500, 16460, 2.2E+7, Vec3(0.11, 0.42, 0.27)));
    bodies.push_back(Body(800, 20000, 3.25E+6, Vec3(0.3, 0.13, 0.12)));
    bodies.push_back(comet);
};

void update_particles(float delta_time) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        SupernovaParticle &p = particles[i];

        if (p.lifetime > 0) {
            // Atualiza idade e vida restante
            p.age += delta_time;
            p.lifetime -= delta_time;

            // Atualiza o parâmetro t da curva Bézier
            p.t += p.speed * delta_time;
            if (p.t > 1.0f) p.t = 1.0f; // não ultrapassa o final da curva

            // Calcula posição atual na curva de Bézier cúbica
            float u = 1.0f - p.t;
            Vec3 pos =
                p.p0 * (u*u*u) +
                p.p1 * (3*u*u*p.t) +
                p.p2 * (3*u*p.t*p.t) +
                p.p3 * (p.t*p.t*p.t);

            p.position = pos * sun_scale + bodies[0].position; // escala e centraliza no Sol

            if(p.trail.empty()) {
                p.trail.push_back(p.position);
                continue;
            } else if ((*p.trail.end() - p.position).length() > 100) {
                p.trail.push_back(p.position);
                if (p.trail.size() > 20) p.trail.erase(p.trail.begin()); // limite de pontos
            }
        } else {
            particles[i] = generate_particle();
        }
    }
}


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

        update_particles(effective_delta);
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
			const float COLOR[] = { 0.8, 0.3, 0.13, 1.0f };
			if (i == 0) {
				glDisable(GL_LIGHT0);
				glEnable(GL_LIGHT1);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, SUN_COLOR);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ZERO);
			    glScalef(sun_scale, sun_scale, sun_scale);
			} else if (i == 6) {
                glDisable(GL_LIGHT0);
                glEnable(GL_LIGHT1);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, COLOR);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ZERO);
			}

			glutSolidSphere(body.radius, 100, 100);

			if (i == 0) {
				glDisable(GL_LIGHT1);
				float sun_diffuse[] = {sun_color.x, sun_color.y, sun_color.z, 1.0f};
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ZERO);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
				glEnable(GL_LIGHT0);
			} else if (i == 6) {
                glDisable(GL_LIGHT1);
    			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, COLOR);
    			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ZERO);
    			glLightfv(GL_LIGHT0, GL_DIFFUSE, COLOR);
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
        glScalef(4.0, 4.0, 4.0);
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

        const float ZERO[] = {0.0f, 0.0f, 0.0f, 1.0f};
		const float COLOR[] = { 0.64, 0.64, 0.64, 0.6f };

        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, COLOR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ZERO);

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
        glScalef(1/4.0, 1/4.0, 1/4.0);
        glTranslated(0, 0, 125);

        glDisable(GL_LIGHT1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, COLOR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ZERO);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, COLOR);
        glEnable(GL_LIGHT0);
    glPopMatrix();

    if (sun_explosion_step != 0) {
        const float SUN_COLOR[] = { sun_color.x, sun_color.y, sun_color.z, 1.0f };
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (sun_explosion_step < 3)
            glLineWidth(2.0f);
        else
            glLineWidth(2.0 * sun_scale);
        glDepthMask(GL_FALSE);

        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, SUN_COLOR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ZERO);

        for (int i = 0; i < NUM_PARTICLES; i++) {
            if (particles[i].lifetime > 0 && !particles[i].trail.empty()) {
                glBegin(GL_LINE_STRIP);
                for (int j = 0; j < particles[i].trail.size(); j++) {
                    float alpha = (float)j / particles[i].trail.size(); // fade da ponta
                    glColor4f(sun_color.x, sun_color.y, sun_color.z, alpha);

                    glVertex3f(particles[i].trail[j].x, particles[i].trail[j].y, particles[i].trail[j].z);
                }
                glVertex3f(particles[i].position.x, particles[i].position.y, particles[i].position.z);
                glEnd();
            }
        }
       	glDisable(GL_LIGHT1);
        float sun_diffuse[] = {sun_color.x, sun_color.y, sun_color.z, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ZERO);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
        glEnable(GL_LIGHT0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

	glutSwapBuffers();
}

void setup(void)
{
    const float ZEROES[4] = {0.0, 0.0, 0.0, 1.0};
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, fAspect, 1, 1E+9);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		cam.position.x, cam.position.y, cam.position.z,
        cam.target.x, cam.target.y, cam.target.z,
        cam.up.x, cam.up.y, cam.up.z
	);

	const float sun_position[4] = {0, 0, 0, 1};
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //aplica o zBuffer
    view_setup();
}
// Função callback chamada para gerenciar teclado
void keyboard_callback(unsigned char key_code, int x, int y) {
    float distance = cam.position.length();
	switch (key_code) {
		case 'r':
			cam.position = INITIAL_CAMERA_POSITION;
			cam.up = INITIAL_CAMERA_UP;
			break;

		// Movimentação da câmera
		case 'w':
		    cam.position = cam.position + cam.up * (distance / 100);
            cam.position = cam.position.unitary() * distance;
			break;
		case 'a':
		    cam.position = cam.position + cam.up.cross(cam.target - cam.position).unitary() * (distance / 100);
			cam.position = cam.position.unitary() * distance;
			break;
		case 's':
            cam.position = cam.position - cam.up * (distance / 100);
            cam.position = cam.position.unitary() * distance;
            break;
		case 'd':
		    cam.position = cam.position - cam.up.cross(cam.target - cam.position).unitary() * (distance / 100);
            cam.position = cam.position.unitary() * distance;
			break;

		case 'q':
		    distance = distance / 1.01;
            cam.position = cam.position.unitary() * distance;
			break;
		case 'e':
		    distance = distance * 1.01;
            cam.position = cam.position.unitary() * distance;
			break;

		case 'x':
		    if(sun_explosion_step == 0){
                sun_explosion_step = 1;
                reset_supernova();
			}
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
	    factor = (int) (factor * 1.1);
	}
	if (button == GLUT_RIGHT_BUTTON){
	    factor = (int) (factor / 1.1);
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

	#define INITIAL_WIDTH 1280
	#define INITIAL_HEIGHT 720

	const int initial_x = (glutGet(GLUT_SCREEN_WIDTH) - INITIAL_WIDTH) / 2;
	const int initial_y = (glutGet(GLUT_SCREEN_HEIGHT) - INITIAL_HEIGHT) / 2;

    glutInitWindowPosition(700,100);
	glutInitWindowSize(1280, 720);

	fAspect = (GLfloat) 1280 / (GLfloat) 720;
    glutCreateWindow("Sistema Solar");

    glutTimerFunc(DELTA, update, 0);
	glutDisplayFunc(draw);
	glutReshapeFunc(window_change_callback);
    glutMouseFunc(mouse_callback);
    glutKeyboardFunc(keyboard_callback);
    glutSpecialFunc(special_keys_callback);

    // Start
    physics_setup();
    setup();
	glutMainLoop();

}
