#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <deque>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
using namespace std;

const int scale = 2;
// all vertices given in anti-clock

struct Point{
    int x{}, y{};
    Point *next=nullptr, *prev=nullptr;
    Point() = default;
    Point(int in1, int in2) : x(in1), y(in2) {}
};

struct Triangle {
    Point p1{}, p2{}, p3{};
    Triangle(const Point &A,const Point &B, const Point &C) : p1(A),p2(B),p3(C) {}
    Triangle(Point* A, Point* B, Point* C) : p1(*A), p2(*B), p3(*C) {}
    double area() {
        double ans = (p1.x - p2.x) * (p3.y - p2.y) - (p3.x - p2.x)*(p1.y - p2.y);
        return ans;
    }
    double area(const Point& A, const Point& B, const Point& C) {
        double ans = (A.x - B.x) * (C.y - B.y) - (C.x - B.x) * (A.y - B.y);
        return ans;
    }
    bool is_point_inside(const Point* p) {
        double a1{}, a2{}, a3{}, a4{};
        a1 = abs(area(p1,p2,*p));
        a2 = abs(area(p3, p2,*p));
        a3 = abs(area(p3, p1,*p));
        a4 = abs(area(p1, p2, p3));
        if (a4 == a1 + a2 + a3) return true;
        return false;
    }
    void draw() {
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);  
        glVertex2f(p1.x * scale, p1.y * scale);
        glVertex2f(p2.x * scale, p2.y * scale);
        glVertex2f(p3.x * scale, p3.y * scale);
        glEnd();
    }
};

struct Line {
    Point p1{}, p2{};
    Line(Point* A,Point* B) : p1(*A),p2(*B) {}
    static void draw(vector<Line> &lines);
};

void Line::draw(vector<Line>& lines) {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    for (auto& i : lines) {
        glVertex2f(i.p1.x * scale, i.p1.y * scale);
        glVertex2f(i.p2.x * scale, i.p2.y * scale);
    }
    glEnd();
}

struct Polygon {
    int n{};
    Point* head = nullptr;
    vector<Point*> original_points;
    void add_point(Point* A) {
        if (n == 0) {
            head = A;
        }
        else if (n == 1) {
            head->next = A;
            A->prev = head;
        }
        else if (n == 2) {
            A->next = head;
            A->prev = head->next;
            head->prev = A;
            head->next->next = A;
        }
        else {
            A->prev = head->prev;
            A->next = head;
            A->prev->next = A;
            head->prev = A;
        }
        original_points.push_back(A);
        n++;
    }
    void remove_point(Point* A) {
        A->prev->next = A->next;
        A->next->prev = A->prev;
        if (A == head) {
            head = A->next;
        }
        n--;
    }

    void draw() {
        if (original_points.size() < 2) return;
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        for (int i = 0; i < original_points.size(); i++) {
            Point* p1 = original_points[i];
            Point* p2 = original_points[(i + 1) % original_points.size()];
            glVertex2f(p1->x * scale, p1->y * scale);
            glVertex2f(p2->x * scale, p2->y * scale);
        }
        this_thread::sleep_for(chrono::milliseconds(75));
        glEnd();
    }
};

int main()
{
    ifstream file("data.txt");
    if (!file) {
        cerr << "Failed to open file.\n";
        return 1;
    }
    int t;
    file >> t;
    file.ignore();
    vector<Polygon*> polygons(t);
    vector<double> run_times(t);
    vector<vector<Line>> lines(t);
    for(int k=0;k<t;k++){
        int n;
        file >> n;
        file.ignore();
        Polygon *P = new Polygon();
        for (int i = 0;i < n;i++) {
            string line;
            std::getline(file, line); 
            stringstream ss(line);
            string x_str, y_str;

            if (getline(ss, x_str, ',') && std::getline(ss, y_str)) {
                Point *A=new Point(stoi(x_str), stoi(y_str));
                (*P).add_point(A);
            }
            else {
                cerr << "Invalid format in line: " << line << "\n";
            }

        }
        polygons[k] = P;
    }
    cout << "Got file data" << '\n';
    for (int _ = 0;_ < t;_++) {
        Polygon P = (*polygons[_]);
        Point* curr = P.head;
        auto start = chrono::high_resolution_clock::now();
        while (P.n > 3) {
            Triangle tri(curr, curr->next, curr->next->next);
            if (tri.area() >= 0) {
                curr = curr->next;
                continue;
            }
            Point* temp = curr->next->next->next;
            bool flag = false;
            while (temp != curr) {
                if (tri.is_point_inside(temp)) {
                    curr = curr->next;
                    flag = true;
                    break;
                }
                temp = temp->next;
            }
            if (!flag) {
                lines[_].emplace_back(curr,curr->next->next);
                P.remove_point(curr->next);
            }
            curr = curr->next;
        }
        auto end = chrono::high_resolution_clock::now();
        double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        run_times[_] = time_taken;
    }

    cout << "Triangulation Done!!\n";
    for (int i = 0;i < t;i++) {
        cout << "Number of points: " << polygons[i]->original_points.size() << ' ' << "Time Taken: " << run_times[i]*(1e-9) << '\n';
    }
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 1000, 1000, "TRIANGULATION", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f,1000 , 1000); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(-500, 1000,1000, -500, 0.0f, 1.0f); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment

    /* Loop until the user closes the window */


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        for (int i = 0;i < t;i++) {
            glClear(GL_COLOR_BUFFER_BIT);
            polygons[i]->draw();
            Line::draw(lines[i]);
            /* Swap front and back buffers */
            glfwSwapBuffers(window);
            /* Poll for and process events */
            glfwPollEvents();
            this_thread::sleep_for(chrono::milliseconds(700));
        }
        glfwTerminate();

    }
    return 0;
}
