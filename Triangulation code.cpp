#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <deque>
#include <chrono>
#include <iomanip>

using namespace std;

float area_of_triangle(vector <pair<int, int>>& v) { /*Finds area of three vertices, when inputted in clockwise direction*/
    float area = (v[0].first - v[2].first) * (v[2].second - v[1].second) - (v[2].first - v[1].first) * (v[0].second - v[2].second);
    return area / 2;
}

bool is_vertex_inside(vector <pair<int, int>>& v) { /*This function checks whether the fourth vertex lies inside the triangle formed by the three vertices or not*/
    int a = 0;
    vector <pair<int, int>> v1;
    vector <pair<int, int>> v2;
    vector <pair<int, int>> v3;

    v1.push_back(make_pair(v[0].first, v[0].second));
    v1.push_back(make_pair(v[3].first, v[3].second));
    v1.push_back(make_pair(v[2].first, v[2].second));

    v2.push_back(make_pair(v[2].first, v[2].second));
    v2.push_back(make_pair(v[3].first, v[3].second));
    v2.push_back(make_pair(v[1].first, v[1].second));


    v3.push_back(make_pair(v[1].first, v[1].second));
    v3.push_back(make_pair(v[3].first, v[3].second));
    v3.push_back(make_pair(v[0].first, v[0].second));


    if (area_of_triangle(v1) >= 0) {
        a = a + 1;
    }
    if (area_of_triangle(v2) >= 0) {
        a = a + 1;
    }
    if (area_of_triangle(v3) >= 0) {
        a = a + 1;
    }
    if (a != 3) {
        return false; /*vertex is not inside*/
    }
    else {
        return true;
    }
}

int main()
{
    int n;
    cout << "Enter Number of Vertices: ";
    cin >> n;
    if (n == 1 || n == 2) {
        cout << "Can't triangulate as there is too less vertices." << endl;
    }
    cout << "Enter the vertices in anti-clockwise order:" << endl;;
    vector <pair<int, int>> v_in;
    vector <pair<int, int>> v;
    vector <char> comma;

    for (int i = 0;i < n;i++) {
;
        int a, b;
        char s;
        cin >> a>> s >> b;
        v_in.push_back(make_pair(a, b));
        comma.push_back(s);

    }
    for (int i = 0;i < n;i++) { /*For reversing the order of the vertices as the code is written for clockwise vertices but input is anticlockwise*/
        v.push_back(make_pair(v_in[n - i - 1].first, v_in[n - i - 1].second));
    }

    deque <int> path; /* Shows the path of which the algorithm should follow*/
    for (int i = 0;i < n;i++) {
        path.push_back(i);
    }

    vector <pair<int, int>> line_pairs; /* vector to store the pairs of vertices to be joined*/

    int l = n;

    auto start = chrono::high_resolution_clock::now();

    while (l > 3) {

        for (int i = 0;i < l - 2;i++) {
            vector <pair<int, int>> temp;
            temp.push_back(make_pair(v[path[i]].first, v[path[i]].second));
            temp.push_back(make_pair(v[path[i + 1]].first, v[path[i + 1]].second));
            temp.push_back(make_pair(v[path[i + 2]].first, v[path[i + 2]].second));

            int clip_count = 0;

            for (auto it : path) {
                temp.push_back(make_pair(v[it].first, v[it].second));
                /*This part finds out if there is any vertex which is inside the triangle*/
                if (is_vertex_inside(temp)) {
                    clip_count = clip_count + 1;
                    temp.pop_back();
                }
                else {
                    temp.pop_back();
                }
            }
            /* Clip_count is supposed to be 3 for clipping as we add one to the value of clip_count is there is something in the triangle, but the three vertices of the 
            triangle always get added,so it always a minimum of 3*/

            if (clip_count == 3 && area_of_triangle(temp) > 0) { /* Checking whether we can clip or not */
                line_pairs.push_back(make_pair(path[i], path[i + 2]));
                path.erase(path.begin() + (i + 1));
                break;
            }
        }
        l = l - 1;
        /*subtract 1 as one elment is removed from path*/
    }

    auto end = chrono::high_resolution_clock::now();
    double time_taken =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    cout << "Time taken by program is : " << fixed
        << time_taken << setprecision(9);
    cout << " sec" << endl;
    cout << "Blue represents the polygon, while red represents the diagonals drawn for triangulation"<<endl;


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
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        /* This piece of code draws the polgon*/
        for (int i = 0;i < n-1;i++) {
            glBegin(GL_LINES);
            glColor3f(0.0, 0.0, 1.0);
            glVertex2f((v[i].first)*2, (v[i].second)*2);
            glVertex2f((v[i+1].first)*2,(v[i+1].second)*2);

        }
        glVertex2f((v[n-1].first)*2, (v[n-1].second)*2);
        glVertex2f((v[0].first)*2, (v[0].second)*2);
        glEnd();

         for (int i = 0;i < line_pairs.size();i++) {
            
            glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0);
            glVertex2f((v[line_pairs[i].first].first)*2, (v[line_pairs[i].first].second)*2);
            glVertex2f((v[line_pairs[i].second].first)*2, (v[line_pairs[i].second].second)*2);
        }
        glEnd();
        


        /* Swap front and back buffers */
        glfwSwapBuffers(window);


        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}