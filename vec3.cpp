#include <cmath>
class Vec3 {
    public:
        float x, y, z;
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
        Vec3() = default;

        Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3 operator-(const Vec3& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        Vec3 operator*(float scale) const {
            return Vec3(x * scale, y * scale, z * scale);
        }

        Vec3 operator/(float scale) const {
            return Vec3(x / scale, y / scale, z / scale);
        }

        float length() const {
            return sqrt(x * x + y * y + z * z);
        }

        Vec3 unitary() const {
            float length = this->length();
            return Vec3(x / length, y / length, z / length);
        }
};
