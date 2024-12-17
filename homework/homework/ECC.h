#ifndef ECC_H
#define ECC_H

#include <vector>
#include <string>
using namespace std;

class ECC {
public:
    struct Curve {
        int a, b, p;
    };

    struct Point {
        int x, y;
        bool infinity;

        Point();
        Point(int x, int y);
    };

    ECC(const Curve& curve, const Point& basePoint, int privateKey);

    Point encrypt(const Point& message);
    Point decrypt(const Point& cipherText);

    static vector<int> normalizeAudioData(const vector<unsigned char>& audioBytes, int p);
    static Point mapToCurve(int message, const Curve& curve);

private:
    Curve curve;
    Point basePoint;
    int privateKey;
    Point publicKey;

    static Point add(const Point& p1, const Point& p2, const Curve& curve);
    static Point multiply(const Point& p, int k, const Curve& curve);
    static bool isOnCurve(const Point& point, const Curve& curve);
    static int mod(int a, int b);
    static int modInverse(int k, int p);
};

#endif
