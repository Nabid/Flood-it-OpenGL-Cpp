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

// window width, height
int width = 640;
int height = 480;
// total cells in a row or a columns, grid size = sz*sz
const int sz = 12;
// single cell size in pixels
const int cell_sz = 25;
int grid[sz+3][sz+3];
int cnt, moves, mx_moves = 25;
// approx centers
int sa = width/2-((sz/2*cell_sz)+(sz/2));
int sb = height/2+((sz/2*cell_sz)+(sz/2));
// buttons
const int tot_buttons = 8;
int buttonX[tot_buttons];
int buttonY[tot_buttons];
// random color to achieve as final goal
int rand_col;
// save current color command
int curr_col;

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
	rand_col = rand() % 6;
	curr_col = -1;
}
/* draw grid */
void drawBoard() {
	int i, j;
	int w = sa, h = sb;

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
		w = sa;
		// decrease height to make new row
		h -= cell_sz + 1;
	}

	// final color cell
	w = sa-100, h = sb;
	glColor3f( col[rand_col].r, col[rand_col].g, col[rand_col].b );
	glBegin(GL_POLYGON);
		glVertex2i(w, h-cell_sz);
		glVertex2i(w+cell_sz, h-cell_sz);
		glVertex2i(w+cell_sz, h);
		glVertex2i(w, h);
	glEnd();

	// TEXTURE
	string s[] = {"TARGET", "WIN", "LOOSE"};
	//glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	void * font1 = GLUT_BITMAP_HELVETICA_10;
	void * font2 = GLUT_BITMAP_HELVETICA_18;

	glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
	glRasterPos2i(w-5, h+5);
	for(string::iterator i = s[0].begin(); i != s[0].end(); ++i) {
		char cc = *i;
		glutBitmapCharacter(font1, cc);
	}

	if( moves >= mx_moves || cnt >= sz*sz ) {
		if( rand_col == curr_col ) {
			glColor3f(GREEN.r, GREEN.g, GREEN.b); //needs to be called before RasterPos
			glRasterPos2i(w-5, h-100);
			for(string::iterator i = s[1].begin(); i != s[1].end(); ++i) {
				char cc = *i;
				glutBitmapCharacter(font2, cc);
			}
		} else if( rand_col != curr_col && curr_col != -1 ) {
			glColor3f(RED.r, RED.g, RED.b); //needs to be called before RasterPos
			glRasterPos2i(w-5, h-100);
			for(string::iterator i = s[2].begin(); i != s[2].end(); ++i) {
				char cc = *i;
				glutBitmapCharacter(font2, cc);
			}
		}
	}

	glEnable(GL_TEXTURE_2D);
	glutSwapBuffers();
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
	glRasterPos2i(sa-20, sb+10);
    for(string::iterator i = s1.begin(); i != s1.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font1, c);
    }

	int ln = 120;
    // message line 2
    glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
    glRasterPos2i(sa-ln, sb+50);
    for(string::iterator i = s2start.begin(); i != s2start.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font2, c);
    }

	for( int k=0; k<6; k++ ) {
		glColor3f(col[k].r, col[k].g, col[k].b); //needs to be called before RasterPos
		glRasterPos2i(sa-ln+50+35*(k+1), sb+50);
		for(string::iterator i = s2c[k].begin(); i != s2c[k].end(); ++i) {
			char c = *i;
			glutBitmapCharacter(font2, c);
		}
	}

	glColor3f(WHITE.r, WHITE.g, WHITE.b); //needs to be called before RasterPos
    glRasterPos2i(sa-ln+20+50+35*6, sb+50);
    for(string::iterator i = s2end.begin(); i != s2end.end(); ++i) {
        char c = *i;
        glutBitmapCharacter(font2, c);
    }

    glEnable(GL_TEXTURE_2D);
    glutSwapBuffers();
    // end text -------------------------------------------------
}

/**
 * draw buttons
 */
void drawButtons() {
	int sx = width-100, sy = height-100;
	int w = sx, h = sy;
	int i, j;
	int c = 0;

	for( i=0; i<6; i++ ) {
		for( j=0; j<1; j++ ) {
			//printf( "grid[ %d ][ %d ] = %d\n", i, j, c );
			// draw polygon with cell color
			glColor3f( col[c].r, col[c].g, col[c].b );
			glBegin(GL_POLYGON);
				glVertex2i(w, h-cell_sz);
				glVertex2i(w+2*cell_sz, h-cell_sz);
				glVertex2i(w+2*cell_sz, h);
				glVertex2i(w, h);
			glEnd();
			// save middle point
			buttonX[c] = w;
			buttonY[c] = h;
			c++;
			// increase width to draw next cell
			w += cell_sz + 1;
		}
		// set width to 0 to draw from starting position
		w = sx;
		// decrease height to make new row
		h -= cell_sz + 1;
	}

	// quit button and reset button
	w = 200, h = 50;
	for(int it=0; it<2; it++) {
		glColor3f( WHITE.r, WHITE.g, WHITE.b );
		glBegin(GL_POLYGON);
			glVertex2i(w, h-cell_sz);
			glVertex2i(w+3*cell_sz, h-cell_sz);
			glVertex2i(w+3*cell_sz, h);
			glVertex2i(w, h);
		glEnd();
		// save middle point
		buttonX[c] = w;
		buttonY[c] = h;
		c++;

		// TEXTURE
		string s[] = {"RESET", "QUIT"};

		//glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, width, 0.0, height);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		void * font1 = GLUT_BITMAP_9_BY_15;

		glColor3f(RED.r, RED.g, RED.b); //needs to be called before RasterPos
		glRasterPos2i(buttonX[c-1]+20, buttonY[c-1]-20);
		for(string::iterator i = s[it].begin(); i != s[it].end(); ++i) {
			char cc = *i;
			glutBitmapCharacter(font1, cc);
		}

		glEnable(GL_TEXTURE_2D);
		glutSwapBuffers();

		w += 100;
	}
}

/* display func */
void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw board
	drawBoard();
	// draw buttons
	drawButtons();

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
	if( moves >= mx_moves || cnt >= sz*sz ) {
		if( rand_col == curr_col ) {
			printf("WIN\n");
		} else {
			printf("LOOSE\n");
		}
		return;
	}

	curr_col = col;				// save current color
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

void mouse(int button, int state, int x, int y) {
	int i;
	y = height - y;
	if(state == 0) {
		for( i=0; i<tot_buttons; i++ ) {
			int w = buttonX[i];
			int h = buttonY[i];
			int x1 = w, y1 = h-cell_sz;
			int x2 = w+3*cell_sz, y2 = h-cell_sz;
			int x3 = w+3*cell_sz, y3 = h;
			int x4 = w, y4 = h;

	//		printf( "(%d, %d)\n", x1, y1 );
	//		printf( "(%d, %d)\n", x2, y2 );
	//		printf( "(%d, %d)\n", x3, y3 );
	//		printf( "(%d, %d)\n", x4, y4 );
	//		printf( "== (%d, %d)\n", x, y );

			if( x > x1 && x < x2 && y > y2 && y < y4 ) {
				switch(i) {
					case 0: key('r', 1, 1); break;
					case 1: key('g', 1, 1); break;
					case 2: key('b', 1, 1); break;
					case 3: key('y', 1, 1); break;
					case 4: key('p', 1, 1); break;
					case 5: key('t', 1, 1); break;
					case 6: key('f', 1, 1); break;
					case 7: key('q', 1, 1); break;
				}

				break;
			}
		}
	}
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
	glutMouseFunc(mouse);
    glutReshapeFunc(reshape);

	gameReset();

	glutMainLoop();
 }
