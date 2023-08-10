#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

const int SIZE = 20; // Size of the maze
const int WALL = 0;
const int PATH = 1;
const int VISITED = 2;
const int SHORTEST_PATH = 3;
const int START =4;
const int END =5;

int maze[SIZE][SIZE];
int distances[SIZE][SIZE];
pair<int, int> previous[SIZE][SIZE];
int start_x, start_y, end_x, end_y;

bool solving = false;

struct Node {
    int x, y;
    int distance;

    Node(int _x, int _y, int _distance) : x(_x), y(_y), distance(_distance) {}

    bool operator<(const Node& other) const {
        return distance > other.distance;
    }
};

void generateMaze() {
    srand(time(0));

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (i == 0 || i == SIZE - 1 || j == 0 || j == SIZE - 1) {
                maze[i][j] = WALL;
            } else {
                maze[i][j] = PATH;
            }
        }
    }

    // Generate walls
    for (int i = 2; i < SIZE - 2; i += 2) {
        for (int j = 2; j < SIZE - 2; j += 2) {
            maze[i][j] = WALL;
        }
    }
    
    // Generate additional random walls
    for (int i = 1; i < SIZE - 1; ++i) {
        for (int j = 1; j < SIZE - 1; ++j) {
            if (maze[i][j] == PATH && rand() % 5 == 0) {
                maze[i][j] = WALL;
            }
        }
    }
}

void solveMaze() {
    solving = true;

    priority_queue<Node> pq;
    pq.push(Node(start_x, start_y, 0));

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            distances[i][j] = INT_MAX;
        }
    }

    distances[start_x][start_y] = 0;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (current.x == end_x && current.y == end_y) {
            break;
        }

        if (distances[current.x][current.y] < current.distance) {
            continue;
        }

        int dx[] = {1, 0, -1, 0};
        int dy[] = {0, 1, 0, -1};

        for (int i = 0; i < 4; ++i) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (newX >= 0 && newX < SIZE && newY >= 0 && newY < SIZE && maze[newX][newY] != WALL) {
                int newDistance = current.distance + 1;

                if (newDistance < distances[newX][newY]) {
                    distances[newX][newY] = newDistance;
                    previous[newX][newY] = make_pair(current.x, current.y);
                    pq.push(Node(newX, newY, newDistance));
                }
            }
        }
    }

    int x = end_x;
    int y = end_y;

    while (x != start_x || y != start_y) {
        maze[x][y] = SHORTEST_PATH;
        pair<int, int> prev = previous[x][y];
        x = prev.first;
        y = prev.second;
    }

    solving = false;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-1.0f, -1.0f, 0.0f);
    glScalef(2.0f / SIZE, 2.0f / SIZE, 1.0f);

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (maze[i][j] == WALL) {
                glColor3f(0.0f, 0.0f, 0.0f); // Black color for walls
            } else if (maze[i][j] == PATH) {
                glColor3f(1.0f, 1.0f, 1.0f); // White color for paths
            } else if (maze[i][j] == VISITED) {
                glColor3f(0.0f, 1.0f, 0.0f); // Green color for visited cells
            } else if (maze[i][j] == SHORTEST_PATH) {
                glColor3f(1.0f, 0.0f, 0.0f); // Red color for the shortest path
            } else if (maze[i][j] == START) {
                glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the starting point
            } else if (maze[i][j] == END) {
                glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the ending point
            }

            glBegin(GL_QUADS);
            glVertex2f(j, i);
            glVertex2f(j + 1, i);
            glVertex2f(j + 1, i + 1);
            glVertex2f(j, i + 1);
            glEnd();
        }
    }

    glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
    if (solving) {
        return;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to maze cell coordinates
        int cellX = x * SIZE / glutGet(GLUT_WINDOW_WIDTH);
        int cellY = (glutGet(GLUT_WINDOW_HEIGHT) - y) * SIZE / glutGet(GLUT_WINDOW_HEIGHT);

        if (cellX >= 0 && cellX < SIZE && cellY >= 0 && cellY < SIZE) {
            if (maze[cellY][cellX] == PATH) {
                if (start_x == -1 && start_y == -1) {
                    start_x = cellY;
                    start_y = cellX;
                    cout << "Start point set: (" << start_x << ", " << start_y << ")" << endl;
                } else if (end_x == -1 && end_y == -1) {
                    end_x = cellY;
                    end_y = cellX;
                    cout << "End point set: (" << end_x << ", " << end_y << ")" << endl;
                }
            }
        }
    }
}

void menuClick(int value) {
    switch (value) {
        case 0:
            generateMaze();
            start_x = -1;
            start_y = -1;
            end_x = -1;
            end_y = -1;
            maze[start_x][start_y] = START;
    	    maze[end_x][end_y] = END;
            break;
        case 1:
            if (start_x != -1 && start_y != -1 && end_x != -1 && end_y != -1) {
                solveMaze();
            } else {
                cout << "Please set both the start and end points first." << endl;
            }
            break;
        case 2:
            start_x = -1;
            start_y = -1;
            end_x = -1;
            end_y = -1;
            generateMaze();
            break;
        case 3:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

void createMenu() {
    glutCreateMenu(menuClick);
    glutAddMenuEntry("Set Start and End Points", 0);
    glutAddMenuEntry("Solve Maze", 1);
    glutAddMenuEntry("Reset Maze", 2);
    glutAddMenuEntry("Exit", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initialize() {
    generateMaze();
    start_x = -1;
    start_y = -1;
    end_x = -1;
    end_y = -1;
    solving = false;

    createMenu();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL Maze Solver");
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    initialize();
    glutMainLoop();

    return 0;
}

