#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include "vec3.cpp"

#define G 6.67430e-11

class Body {
    public:
        float radius;
        float distance;
        float mass;
        Vec3 position;
        Vec3 velocity;
        Vec3 color;

        Body(float radius, float distance, float mass, Vec3 color, Vec3 initial_velocity) {
            this->radius = radius;
            this->distance = distance;
            this->mass = mass;
            this->color = color;

            this->position = Vec3(0, 0, distance);
            this->velocity = initial_velocity;
        }
        
        Body(float radius, float distance, float mass, Vec3 color){
            this->radius = radius;
            this->distance = distance;
            this->mass = mass;
            this->color = color;

            this->position = Vec3(0, 0, distance);
            if (distance != 0) this->velocity = Vec3(sqrt(G * 4E+11 / distance), 0, 0);
            else this->velocity = Vec3(0, 0, 0);
        }

        void draw() {
            glColor3f(color.x, color.y, color.z);
            glPushMatrix();
            glTranslatef(distance, 0, 0);
            glutSolidSphere(radius, 20, 20);
            glPopMatrix();
        }
};
