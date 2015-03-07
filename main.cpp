/**
Objective: FloodIt game implementation using OpenGL
Algorithm: Breadth First Search (BFS) (FLOOD FILL)
Complexity: O(V+E) for each move, V is number of adjacent same color cells
			starting from 0, 0
*/

#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <queue>
#include <cstring>
#include <string>

using namespace std;

#define RUNNING true
#define STOPPED false

int width = 640;
int height = 480;
const int sz = 12;
const int cell_sz = 15;
int grid[sz+3][sz+3];
int cnt, moves, mx_moves = 25;

// color structure
struct Color {
	double r, g, b;
	Color() {
		r = g = b = 0.0;
	}
} RED, GREEN, BLUE, YELLOW, PURPLE, BLACK, TAN, WHITE, col[6];

// creates color
void createColor() {
	RED.r = 1.0;
	GREEN.g = 1.0;
	BLUE.b = 1.0;
	YELLOW.r = YELLOW.g = 1.0;
	WHITE.r = WHITE.g = WHITE.b = 1.0;
	PURPLE.r = 1.0, PURPLE.g = 0.0, PURPLE.b = 1.0;
	TAN.r = 0.858824, TAN.g = 0.576471, TAN.b = 0.439216;
	col[0] = RED;
	col[1] = GREEN;
	col[2] = BLUE;
	col[3] = YELLOW;
	col[4] = PURPLE;
	col[5] = TAN;
}

void init() {
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);
	//glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D( 0, width, 0, height );
	//glOrtho(0, 1, 0, 1, -1, 1);
}

void reshape( int w, int h ) {
	width = w; height = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    init();
    glutPostRedisplay();
}

/* initializes grid with random color variants (0 to 5)*/
void initGrid() {
	int i, j;
	for( i=0; i<sz; i++ ) {
		for( j=0; j<sz; j++ ) {
			grid[i][j] = rand() % 6;
		}
	}
}

// resets game contents
void gameReset() {
	initGrid();
	cnt = 0;
	moves = 0;
}
/* draw grid */
void drawBoard() {
	int i, j;
	int w = 0, h = height;

	for( i=0; i<sz; i++ ) {
		for( j=0; j<sz; j++ ) {
			int c = grid[i][j];
			//printf( "grid[ %d ][ %d ] = %d\n", i, j, c );
			// draw polygon with cell color
			glColor3f( col[c].r, col[c].g, col[c].b );
			glBegin(GL_POLYGON);
				glVertex2i(w, h-cell_sz);
				glVertex2i(w+cell_sz, h-cell_sz);
				glVertex2i(w+cell_sz, h);
				glVertex2i(w, h);
			glEnd();
			// increase width to draw next cell
			w += cell_sz + 1;
		}
		// set width to 0 to draw from starting position
		w = 0;
		// decrease height to make new row
		h -= cell_sz + 1;
	}
}

/* converts int values to string */
string toString( int n ) {
	//printf( "n is = %d\n", n );
	char str[10] = {'\0'};
	int res = 1, rem = 0;
	int i = 0;
	while( res ) {
		res = n / 10;
		rem = n % 10;
		n = res;
		str[i++] = rem + '0';
	}
	int j;
	// string reverse
	for( j=0; j<i/2; j++ ) swap(str[j],str[i-j-1]);
	// save into tmp string type variable
	string tmp = str;
	//printf( "%d moves to string = %s\n", n, str );
	return tmp;
}

/* prints game messages and game status */
void gameInfo() {
	// set message line 1, current status
	string s1 = "Moves = ";
    s1.append( toString(moves) );
    s1.append( "/" );
    s1.append( toString(mx_moves) );
    s1.append( ". Flodded = " );
    s1.append( toString(cnt) );
    s1.append( ". Remain = " );
    s1.append( toString(sz*sz-cnt) );
    s1.append( "." );

    // set message line 2, instructions
    string s2start = "Color ? ( ";
    string s2end = " ) Q to quit. F to fresh board.";
    string s2c[] = {" R"," G"," B"," Y"," P"," T"};

	//glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    void * font1 = GLUT_BITMAP_9_BY_15;
    void * font2 = GLUT_BITMAP_9_BY_15;
    void * font3 = GLUT_BITMAP_9_BY_15;
    void * font4 = GLUT_BITMAP_9_BY_15;

    // message line 1
	glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
	glRasterPos2i(0, height-(sz*cell_sz+5*sz));
    for(string::iterator i = s1.begin(); i != s1.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font1, c);
    }

    // message line 2
    glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
    glRasterPos2i(0, height-(sz*cell_sz+7*sz));
    for(string::iterator i = s2start.begin(); i != s2start.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font2, c);
    }

	for( int k=0; k<6; k++ ) {
		glColor3f(col[k].r, col[k].g, col[k].b); //needs to be called before RasterPos
		glRasterPos2i(50+35*(k+1), height-(sz*cell_sz+7*sz));
		for(string::iterator i = s2c[k].begin(); i != s2c[k].end(); ++i) {
			char c = *i;
			glutBitmapCharacter(font2, c);
		}
	}

	glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
    glRasterPos2i(20+50+35*6, height-(sz*cell_sz+7*sz));
    for(string::iterator i = s2end.begin(); i != s2end.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font2, c);
    }

    glEnable(GL_TEXTURE_2D);
    glutSwapBuffers();
    // end text -------------------------------------------------
}

/* display func */
void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw board
	drawBoard();

	gameInfo();

    glFlush();
}

/* measure number of cell of same colors and moves availibility
   using breadth first search
   param: color code (code according to key pressed) */
void bfs( int col ) {
	// base case
	// if moves is equal to or greater than maximum moves return, no need to run
	// if number of cell color is same as total cells return, no need to run
	if( moves >= mx_moves || cnt >= sz*sz ) return;

	int prev = grid[0][0]; 		// save previous color
	queue<pair<int, int> >cell; // cell queue
	int vis[sz][sz] = {0};		// visited array, initially all are non visited
	int dx[] = {0, 0, -1, 1};	// direction array along row
	int dy[] = {1, -1, 0, 0};	// direction array along column

	cnt = 0;
	moves++;

	// push (0,0) cell into queue
	cell.push( make_pair(0,0) );
	vis[0][0] = 1;		// make (0,0) cell as work in progress
	grid[0][0] = col;	// color (0,0) cell

	while( !cell.empty() ) {
		pair<int, int> uu = cell.front(); cell.pop();
		cnt++;
		for( int k=0; k<4; k++ ) {
			int tx = uu.first + dx[k];
			int ty = uu.second + dy[k];
			if( tx>=0 && tx<sz && ty>=0 && ty<sz && vis[tx][ty]==0 && (grid[tx][ty]==prev\
						|| grid[tx][ty]==col) ) {
				cell.push( make_pair(tx,ty) );
				grid[tx][ty] = col;
				vis[tx][ty] = 1;
			}
		}
		vis[uu.first][uu.second] = 2; // make visited
	}
}

void key( unsigned char key, int x, int y ) {
	//printf( "Key: %c, x: %d, y: %d\n", key, x, y );
	int inc = 5;
	switch( key ) {
		case 27:
			exit(EXIT_SUCCESS);
			break;
		case 'q':
			exit(EXIT_SUCCESS);
			break;
		// resets game
		case 'f':
			gameReset();
			break;
		case 'r':
			bfs( 0 );
			break;
		case 'g':
			bfs( 1 );
			break;
		case 'b':
			bfs( 2 );
			break;
		case 'y':
			bfs( 3 );
			break;
		case 'p':
			bfs( 4 );
			break;
		case 't':
			bfs( 5 );
			break;
	}
    glutPostRedisplay();
}

void idle() {
	//glutPostRedisplay();
}

int main( int argc, char** argv ) {
	createColor();

	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
	//glutEnterGameMode();
	glutCreateWindow("FLOOD IT");
	//gluOrtho2D( 0, width, 0, height );
	init();
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
    glutReshapeFunc(reshape);

	gameReset();

	glutMainLoop();
 }
