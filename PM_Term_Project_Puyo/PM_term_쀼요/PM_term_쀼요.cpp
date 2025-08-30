#include <iostream>
#include <fstream>

#include <random>
#include <vector>
#include <algorithm>

#include "Puyo.h"
#include "cellData.h"
#include "Word.h"
#include "Letter.h"

using namespace std;

#define row 12
#define col 6

Beam beam(WIDTH, HEIGHT, WIDTH / 2);

Puyo puyo1(radius, 20, 20), puyo2(radius, 20, 20), prepuyo1(radius, 20, 20), prepuyo2(radius, 20, 20);
Puyo_pair puyo_pair;

// Booting Window
extern Letter pu1, yo1, pu2, yo2;
void draw뿌(const Letter& pu, const float& w); void draw요(const Letter& yo, const float& r, const float& w1, const float& w2);

int pu1_phase = 0, yo1_phase = 0, pu2_phase = 0, yo2_phase = 0;

// 시작 화면 버튼
Word start("Start"), score_board("Score");
Vector4f start_color, score_board_color;
Vector4f start_color_copy, score_board_color_copy;
bool start_on = false, score_board_on = false;

// Button effect
float button_starting = 0;
void draw_button(const float& starting, const float& x, const float& y) {
	float r = 1;
	vector<float> v = { starting, starting + 20, starting + 40 };

	for (auto i : v) {
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < 13; j++) {
			glVertex3f(x + r * cos(2 * PI / 60 * (i + j)), y + r * sin(2 * PI / 60 * (i + j)), 1);
			glVertex3f(x + r * 8 / 9 * cos(2 * PI / 60 * (i + j)), y + r * 8 / 9 * sin(2 * PI / 60 * (i + j)), 1);
		} glEnd();
	}
}

vector<vector<int>> example_ani_puyo_color(4, vector<int>(2));
int puyo_ani = 4;
bool ani_start = false;
enum ANI_MODE { ANI_CONTROL, ANI_FALL, ANI_REMOVE, ANI_POP };
ANI_MODE ani_mode = ANI_CONTROL;

// Score Window
Word ranking[10] = { Word("1", GLUT_BITMAP_9_BY_15), Word("2", GLUT_BITMAP_9_BY_15), Word("3", GLUT_BITMAP_9_BY_15)\
, Word("4", GLUT_BITMAP_9_BY_15) , Word("5", GLUT_BITMAP_9_BY_15) , Word("6", GLUT_BITMAP_9_BY_15) , Word("7", GLUT_BITMAP_9_BY_15)\
, Word("8", GLUT_BITMAP_9_BY_15) , Word("9", GLUT_BITMAP_9_BY_15) , Word("10", GLUT_BITMAP_9_BY_15) };

Word score_title("R a n k i n g"), MAIN("Main");
Vector4f score_title_color;
Vector4f MAIN_color;
Vector4f MAIN_color_copy;
bool MAIN_on = false, score_init = true, score_sort = false;

vector<Word> rank_set(10);

int ranking_coor[10] = { 136, 100, 64, 28, -8, -44, -80, -116, -152, -188 };

void score_recording() {
	vector<int> score_record(10, 0);

	ifstream read_score("score.txt"); // 점수 기록 스코어 파일은 점수에 해당하는 정수만 담긴다.
	for (int i = 0; i < 10; i++) {
		if (read_score.eof()) {	break; }

		read_score >> score_record[i];
	}
	read_score.close();

	int k = 0;
	for (auto w : score_record) {
		rank_set[k] = Word(to_string(w));
		rank_set[k].setFont(GLUT_BITMAP_9_BY_15);
		k++;
	}
}

// Game field
void draw_puyo_field(int lev) {
	glLineWidth(2.f);
	glBegin(GL_LINES); {
		for (int i = 0; i <= col; i++) {
			glVertex3f(-fieldX - radius + i*2*radius, fieldY_bottom - radius, -1);
			glVertex3f(-fieldX - radius + i*2*radius, fieldY_top + radius - lev*2*radius, -1);
		}
		for (int j = 0; j <= row - lev; j++) {
			glVertex3f(-fieldX - radius, fieldY_bottom - radius + j*2*radius, -1);
			glVertex3f(fieldX + radius, fieldY_bottom - radius + j*2*radius, -1);
		}
	} glEnd();
}

// Random color generator
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dis(1, 4);
vector<Vector4f> colorset_background = { {0, 0, 1, 1}, {1, 1, 1, 1}, {0, 1, 0, 1}, {1, 1, 0, 1}, {1, 0, 0, 1} };
vector<Vector4f> colorset = { {0, 0, 0, 0}, {1, 0, 0, 1}, {0, 0.3, 1, 1}, {0, 1, 0, 1}, {0.4, 0, 0.4, 1} };

Puyo colorPuyo; int color = 1, color_gauge = 0;

// Puyo function
int level = 0;
int next_level_gauge = 0;
vector<int> level_gauge = { 18, 13, 8 };
vector<int> level_score = { 10, 20, 50 };
vector<int> level_length = { 1500, 2000 };

float effect_radius = 0;
void pop_effect(const float& r, const int& x, const int& y) {
	for (auto i : { 8, 16, 24, 32, 40, 48, 56, 64 }) {
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < 3; j++) {
			glVertex3f(-fieldX + x*2*radius + r * cos(2 * PI / 64 * (i - 1 + j)), fieldY_bottom + y*2*radius + r * sin(2 * PI / 64 * (i - 1 + j)), 1);
			glVertex3f(-fieldX + x*2*radius + r * 4 / 5 * cos(2 * PI / 64 * (i - 1 + j)), fieldY_bottom + y*2*radius + r * 4 / 5 * sin(2 * PI / 64 * (i - 1 + j)), 1);
		} glEnd();
	} 
}

vector<vector<Puyo>> puyo_set(col, vector<Puyo>(15, Puyo())); // 15 X 6 puyo set. y좌표 13에 뿌요가 들어올 경우 게임 종료
vector<vector<int>> puyo_pile(col, vector<int>(15, 0)); // puyo_update를 위한 15 X 6 matrix 버그 방지를 위해 넉넉하게 잡았다.

vector<vector<cellData>> puyo_pop;
vector<cellData> pop;

vector<vector<bool>> is_checked(col, vector<bool>(row, false));

vector<vector<int>> dir = { {0, 1}, { 1, 0 }, { -1, 0 }, { 0, -1 } };

void dfs(int c, int r) { // 깊이 우선 탐색
	for (int z = 0; z < 4; z++) {
		int x = c + dir[z][0];
		int y = r + dir[z][1];

		if (x < 0 || x >= col || y < 0 || y >= row) continue;
		if (is_checked[x][y]) continue;
		if (puyo_pile[c][r] == puyo_pile[x][y]) {
			pop.push_back(cellData(x, y));
			is_checked[x][y] = true;

			dfs(x, y);
		}
	}
}

bool is_puyo_pop() { // 터뜨릴 뿌요가 있는지 확인
	bool p = false;
	
	for (int j = 0; j < row; j++) {
		for (int i = 0; i < col; i++) {
			if (puyo_pile[i][j] != 0 && is_checked[i][j] == false) {
				is_checked[i][j] = true;
				pop.push_back(cellData(i, j));
				dfs(i, j);

				if (pop.size() >= 4) {
					puyo_pop.push_back(pop);
					p = true;
				}
			}
			pop.clear();
		}
	}
	
	is_checked = vector<vector<bool>>(col, vector<bool>(row));
	return p;
}

enum MODE { START, CONTROL, FALL, REMOVE, POP, SCORE, END, OVER, PAUSE, QUIT }; // CONTROL은 무조건 FALL로 넘어간다. FALL은 무조건 REMOVE로 넘어간다. REMOVE는 is_puyo_pop에 따라 CONTROL, FALL이 갈린다.
MODE mode = START;

// CONTROL
bool generator = true; // remove 끝나면 generate
bool prepuyo = true; // 뿌요 미리보기

Word time1("1"), time2("2"), time3("3"); // Game start count down

// FALL
int control_gauge = 0;
int falling_gauge = 0;

// SCORE
Word point("Point"), earned_point("0");
int score = 0;

// PAUSE
Word back("Back"), quit("Quit"), terminal("Quit?");
bool back_on = false;
bool quit_on = false; MODE quit_mode_copy = QUIT;
bool resume_on = false; MODE resume_mode_copy = PAUSE;
bool pause_on = false;

// END
bool end_init = true;
Word gameover("Game Over");

void resume(const float& w) { // pause triangle
	glLineWidth(w);
	glBegin(GL_LINES); {
		glVertex3f(0, 1, 0); glVertex3f(0, -1, 0); // 왼쪽 ㅣ
		glVertex3f(1, 0, 0); glVertex3f(0, 1, 0); // 오른쪽 \ 빗변
		glVertex3f(1, 0, 0); glVertex3f(0, -1, 0); // 오른쪽 / 빗변
	} glEnd();
}

void pause(const float& w) { // pause sticks
	glLineWidth(w);
	glBegin(GL_LINES); {
		glVertex3f(0, 1, 0); glVertex3f(0, -1, 0); // 왼쪽 ㅣ
		glVertex3f(0.2, 1, 0); glVertex3f(0.2, -1, 0); // 오른쪽 ㅣ
		glVertex3f(0, 1, 0); glVertex3f(0.2, 1, 0); // 위쪽 ㅡ
		glVertex3f(0, -1, 0); glVertex3f(0.2, -1, 0); // 아래쪽 ㅡ
		
		// 2nd.
		glVertex3f(0.8, 1, 0); glVertex3f(0.8, -1, 0); // 왼쪽 ㅣ
		glVertex3f(1, 1, 0); glVertex3f(1, -1, 0); // 오른쪽 ㅣ
		glVertex3f(0.8, 1, 0); glVertex3f(1, 1, 0); // 위쪽 ㅡ
		glVertex3f(0.8, -1, 0); glVertex3f(1, -1, 0); // 아래쪽 ㅡ
	} glEnd();
}

void init() {
	
	// Control Mode
	puyo_set = vector<vector<Puyo>>(col, vector<Puyo>(row + 1, Puyo()));
	puyo_pile = vector<vector<int>>(col, vector<int>(row + 1, 0));

	puyo_pop = {};
	pop = {};

	is_checked = vector<vector<bool>>(col, vector<bool>(row, false));

	colorPuyo.setEmission(0, 0, 0, 0);

	level = 0;
	next_level_gauge = 0;

	beam.setAmbient(0.5, 0.5, 0.5, 1.0);
	beam.setDiffuse(0.7, 0.7, 0.7, 1.0);
	beam.setSpecular(1.0, 1.0, 1.0, 1.0);

	prepuyo1.setColor(dis(gen));
	prepuyo2.setColor(dis(gen));
	prepuyo1.setAmbient(colorset[prepuyo1.getColor()]);
	prepuyo2.setAmbient(colorset[prepuyo2.getColor()]);
	prepuyo1.setCenter({ 2, 12, 0 });
	prepuyo1.setCenter(2, 12, 0);
	prepuyo2.setCenter(3, 12, 0);

	point.setCoor(-45, -260);
	point.setFont(GLUT_BITMAP_9_BY_15);

	earned_point.setCoor(17, -260);
	earned_point.setFont(GLUT_BITMAP_9_BY_15);
	earned_point.setWord("0");

	// Start Mode
	pu1.setCenter({ -1.1, 10, 0 }); pu1.setVelocity({ 0, -0.3, 0 });
	yo1.setCenter({ -0.15, 11.2, 0 }); yo1.setVelocity({ 0, -0.1, 0 });
	pu2.setCenter({ 1.4, 11.5, 0 }); pu2.setVelocity({ 0, -0.1, 0 });
	yo2.setCenter({ 2.7, 12, 0 }); yo2.setVelocity({ 0, -0.1, 0 });

	pu1_phase = yo1_phase = pu2_phase = yo2_phase = 0;

	//pu1.setCenter({ -1.2, 2, 0 }); Letter Coordinate
	//yo1.setCenter({ -0.3, 3.2, 0 });
	//pu2.setCenter({ 1.2, 3.5, 0 });
	//yo2.setCenter({ 2.5, 4, 0 });
	
	start.setCoor(-97, -10);
	score_board.setCoor(50, -10);

	start_color.coor(0, 0, 0, 0); start_on = false;
	score_board_color.coor(0, 0, 0, 0); score_board_on = false;

	puyo_ani = 4;
	ani_start = false;
	ani_mode = ANI_CONTROL;

	control_gauge = 0;
	falling_gauge = 0;
	button_starting = 0;

	// Score Mode
	rank_set = vector<Word>(10);
	score_recording();

	score_title.setCoor(-60, 223);
	score_title.setFont(GLUT_BITMAP_TIMES_ROMAN_24);

	MAIN.setCoor(-20, -270);
	MAIN_color.coor(0, 0, 0, 0); MAIN_on = false;

	score_init = true; score_sort = false;
	score = 0; 
	
	// Pause Mode
	terminal.setCoor(-24, 80);
	quit.setCoor(-93, -15);
	back.setCoor(50, -15);

	// END
	end_init = true;
	gameover.setCoor(0, 20);
}

void idle() {
	if (mode == START) {

		// 뿌1 motion
		if (!(pu1_phase == 5 && pu1[1] < 2.1)) {
			pu1.move();

			pu1.setVelocity(pu1.getVelocity()[0], pu1.getVelocity()[1] - 0.01, pu1.getVelocity()[2]);

			if (pu1_phase == 0 && pu1[1] < 2) {
				pu1.setVelocity(pu1.getVelocity()[0], 0.25, pu1.getVelocity()[2]);
				pu1_phase++;
			}
			else if (pu1_phase == 2 && pu1[1] < 2) {
				pu1.setVelocity(pu1.getVelocity()[0], 0.15, pu1.getVelocity()[2]);
				pu1_phase++;
			}
			else if ((pu1_phase == 1 && pu1[1] > 3) || (pu1_phase == 3 && pu1.getVelocity()[1] < 0) || (pu1_phase == 4 && pu1.getVelocity()[1] < 0)) {
				pu1_phase++;
			}
		}

		// 요1 motion
		if (!(yo1_phase == 5 && yo1[1] < 3.3)) {
			yo1.move();

			yo1.setVelocity(yo1.getVelocity()[0], yo1.getVelocity()[1] - 0.01, yo1.getVelocity()[2]);

			if (yo1_phase == 0 && yo1[1] < 3.2) {
				yo1.setVelocity(yo1.getVelocity()[0], 0.27, yo1.getVelocity()[2]);
				yo1_phase++;
			}
			else if (yo1_phase == 2 && yo1[1] < 3.2) {
				yo1.setVelocity(yo1.getVelocity()[0], 0.15, yo1.getVelocity()[2]);
				yo1_phase++;
			}
			else if ((yo1_phase == 1 && yo1[1] > 4.2) || (yo1_phase == 3 && yo1.getVelocity()[1] < 0) || (yo1_phase == 4 && yo1.getVelocity()[1] < 0)) {
				yo1_phase++;
			}
		}

		// 뿌2 motion
		if (!(pu2_phase == 5 && pu2[1] < 3.6)) {
			pu2.move();

			pu2.setVelocity(pu2.getVelocity()[0], pu2.getVelocity()[1] - 0.01, pu2.getVelocity()[2]);

			if (pu2_phase == 0 && pu2[1] < 3.5) {
				pu2.setVelocity(pu2.getVelocity()[0], 0.25, pu2.getVelocity()[2]);
				pu2_phase++;
			}
			else if (pu2_phase == 2 && pu2[1] < 3.5) {
				pu2.setVelocity(pu2.getVelocity()[0], 0.15, pu2.getVelocity()[2]);
				pu2_phase++;
			}
			else if ((pu2_phase == 1 && pu2[1] > 4.5) || (pu2_phase == 3 && pu2.getVelocity()[1] < 0) || (pu2_phase == 4 && pu2.getVelocity()[1] < 0)) {
				pu2_phase++;
			}
		}

		// 요2 motion
		if (!(yo2_phase == 5 && yo2[1] < 4.1)) {
			yo2.move();

			yo2.setVelocity(yo2.getVelocity()[0], yo2.getVelocity()[1] - 0.01, yo2.getVelocity()[2]);

			if (yo2_phase == 0 && yo2[1] < 4) {
				yo2.setVelocity(yo2.getVelocity()[0], 0.3, yo2.getVelocity()[2]);
				yo2_phase++;
			}
			else if (yo2_phase == 2 && yo2[1] < 4) {
				yo2.setVelocity(yo2.getVelocity()[0], 0.15, yo2.getVelocity()[2]);
				yo2_phase++;
			}
			else if ((yo2_phase == 1 && yo2[1] > 5) || (yo2_phase == 3 && yo2.getVelocity()[1] < 0) || (yo2_phase == 4 && yo2.getVelocity()[1] < 0)) {
				yo2_phase++;
			}
		}
		
		// Button color
		if (pu1_phase == 5 && pu2_phase == 5 && yo1_phase == 5 && yo2_phase == 5 && \
			pu1[1] < 2.1 && yo1[1] < 3.3 && pu2[1] < 3.6 && yo2[1] < 4.1 && \
			(start_color[3] < 1 || score_board_color[3] < 1)) {

			if (!start_on) { start_color = start_color + Vector4f(0.007, 0.007, 0.007, 0.007); }
			if (!score_board_on) { score_board_color = score_board_color + Vector4f(0.007, 0.007, 0.007, 0.007); }
		}

		// Button animation
		if (start_on || score_board_on) {
			button_starting += 0.3;
		}

		// Example animation
		if ((start_color[0] >= 1 && start_color[1] >= 1 && start_color[2] >= 1 && start_color[3] >= 1) || \
			(score_board_color[0] >= 1 && score_board_color[1] >= 1 && score_board_color[2] >= 1 && score_board_color[3] >= 1)) { ani_start = true; }
		if (ani_start) { 

			if (puyo_ani == 4) {
				int f_color = dis(gen); // 랜덤 두 색상 생성
				int s_color = dis(gen);
				
				while (f_color == s_color) {
					s_color = dis(gen);
				}
				example_ani_puyo_color[0] = { f_color, f_color }; // 총 4개의 뿌요로 애니메이션 구성
				example_ani_puyo_color[1] = { s_color, f_color };
				example_ani_puyo_color[2] = { s_color, s_color };
				example_ani_puyo_color[3] = { f_color, s_color };

				puyo_ani = 0;
			}

			if (ani_mode == ANI_CONTROL) {
				control_gauge++;
				if (control_gauge == 15) {
					control_gauge = 0;
					puyo_pair.down();
				}

				if (generator) {
					puyo1.setColor(example_ani_puyo_color[puyo_ani][0]);
					puyo2.setColor(example_ani_puyo_color[puyo_ani][1]);

					puyo1.setAmbient(colorset[example_ani_puyo_color[puyo_ani][0]]);
					puyo2.setAmbient(colorset[example_ani_puyo_color[puyo_ani][1]]);

					switch (puyo_ani) {
					case 0: 
						puyo1.setCenter(2, 12, 0);
						puyo2.setCenter(3, 12, 0);
						break;
					case 1:
						puyo1.setCenter(3, 12, 0);
						puyo2.setCenter(4, 12, 0);
						break;
					case 2:
						puyo1.setCenter(0, 12, 0);
						puyo2.setCenter(1, 12, 0);
						break;
					case 3:
						puyo1.setCenter(2, 12, 0);
						puyo2.setCenter(3, 12, 0);
					}

					puyo_pair.setPuyoPair(puyo1, puyo2);
					puyo_pair.initRot();

					for (int i = 0; i < puyo_ani; i++) { puyo_pair.rotate(); }

					generator = false;
					puyo_ani++;
				}

				if (puyo_pair.getFirstPuyo().getCenter()[1] == 0 || puyo_pair.getSecondPuyo().getCenter()[1] == 0) { ani_mode = ANI_FALL; } // CONTROL에서 FALL로 넘어가는 조건
				else if (puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1] - 1] != 0 \
					|| puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1] - 1] != 0) {
					ani_mode = ANI_FALL;
				}

				if (ani_mode == ANI_FALL) { // FALL로 바뀌었다면 pair의 뿌요를 모두 puyo_pile에 넣고 바로 idle을 탈출한다.
					puyo_set[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1]] = puyo_pair.getFirstPuyo();
					puyo_set[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1]] = puyo_pair.getSecondPuyo();
					puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1]] = puyo_pair.getFirstPuyo().getColor();
					puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1]] = puyo_pair.getSecondPuyo().getColor();

					puyo_pair.setPuyoPair(Puyo(), Puyo());
				}
			}
			
			else if (ani_mode == ANI_FALL) {
				falling_gauge++;

				bool hole = false;
				if (falling_gauge == 5) {
					for (int i = 0; i < col; i++) { // 내리는 게 중요하기 때문에 바닥에서 위로 탐색을 시작한다.
						for (int j = 1; j < row; j++) { // 바닥이면 패스하므로 j = 0을 제외해버렸다.
							if (puyo_pile[i][j] == 0) continue; // 빈 공간이면 패스

							if (puyo_pile[i][j - 1] == 0) { // 아래에 뿌요가 없을 때 실행
								puyo_set[i][j - 1] = puyo_set[i][j];
								puyo_set[i][j - 1].setCenter(i, j - 1, 0);
								puyo_pile[i][j - 1] = puyo_pile[i][j];

								puyo_pile[i][j] = 0;
								puyo_set[i][j] = Puyo(); // 업데이트 함수에서는 끝까지 내렸지만 시각 효과를 위해 center를 한 칸씩 내린다.

								hole = true; // 한 번이라도 실행된다면 idle을 돌아서 한 번 더 검사한다. 이때 실행되지 않으면 false가 전달되면서 REMOVE로 넘어간다.
							}
						}
					}
					falling_gauge = 0;

					if (!hole) { ani_mode = ANI_REMOVE; } // hole이 없다면 REMOVE로 넘어간다.
				}
			}

			else if (ani_mode == ANI_REMOVE) {
				if (is_puyo_pop()) {
					for (auto cell : puyo_pop) {
						for (auto xy : cell) {
							puyo_pile[xy[0]][xy[1]] = 0;
							puyo_set[xy[0]][xy[1]] = Puyo();
						}
					}

					ani_mode = ANI_POP; // 없어질 뿌요가 있다면 제거 후 FALL로 가서 내려올 뿌요들을 검사한다. + pop_effect를 보여준다.
				}
				else { // FALL이 끝났는데 REMOVE 사항이 없다면 맵의 업데이트가 끝났다는 의미이다.
					ani_mode = ANI_CONTROL;
					generator = true;
					control_gauge = 0;
				}
			}

			else if (ani_mode == ANI_POP) {
				effect_radius += 1.6;

				if (effect_radius > radius * 3 / 2) {
					effect_radius = 0;
					puyo_pop.clear();
					ani_mode = ANI_FALL; // POP은 REMOVE에서 FALL로 가는 중간 단계
				}
			}
		}
	}
	else if (mode == CONTROL) {

		if (level != 2) {
			next_level_gauge++;
			if (next_level_gauge == level_length[level]) {
				control_gauge = 0;
				next_level_gauge = 0;
				level++;

				prepuyo1.setCenter(2, 12 - level, 0);
				prepuyo2.setCenter(3, 12 - level, 0);
			}
		}

		control_gauge++;
		if (control_gauge == level_gauge[level]) {
			control_gauge = 0;
			puyo_pair.down();
		}

		if (prepuyo) { // prepuyo는 generator 생성 이후에 만들어진다. 그러므로 generator가 돌았을 때 true가 되며, 다시 generator가 true가 될 때까지 false를 유지한다.
			prepuyo1.setColor(dis(gen));
			prepuyo2.setColor(dis(gen));
			prepuyo1.setAmbient(colorset[prepuyo1.getColor()]);
			prepuyo2.setAmbient(colorset[prepuyo2.getColor()]);

			prepuyo1.setCenter(2, 12 - level, 0);
			prepuyo2.setCenter(3, 12 - level, 0);

			prepuyo = false;
		}

		if (generator) { // generate는 REMOVE에서 CONTROL로 돌아온 상황에서 실행된다. 초기 generator는 false이며 REMOVE에서 generator의 값을 바꿔줄 것이다.
			// cout << prepuyo1.getColor() << ", " << prepuyo2.getColor() << '\n'; // 연쇄 점검 코드
			puyo1 = prepuyo1;
			puyo2 = prepuyo2;

			puyo_pair.setPuyoPair(puyo1, puyo2);
			puyo_pair.initRot();

			generator = false; prepuyo = true;

			// 연쇄 점검 코드
			/*
			for (int i = row - 1; i >= 0; i--) {
				for (int j = 0; j < col; j++) {
					cout << puyo_pile[j][i];
				}
				cout << '\n';
			}
			for (int i = row - 1; i >= 0; i--) {
				for (int j = 0; j < col; j++) {
					if (puyo_set[j][i].getColor()) {
						cout << "(" << puyo_set[j][i].getColor() << ")";
					}
					else cout << " X ";
				}
				cout << "\n";
			} */
		}

		if (puyo_pair.getFirstPuyo().getCenter()[1] == 0 || puyo_pair.getSecondPuyo().getCenter()[1] == 0) { mode = FALL; } // CONTROL에서 FALL로 넘어가는 조건
		else if (puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1] - 1] != 0 \
			|| puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1] - 1] != 0) { mode = FALL; }

		if (mode == FALL) { // FALL로 바뀌었다면 pair의 뿌요를 모두 puyo_pile에 넣고 바로 idle을 탈출한다.
			puyo_set[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1]] = puyo_pair.getFirstPuyo();
			puyo_set[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1]] = puyo_pair.getSecondPuyo();
			puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1]] = puyo_pair.getFirstPuyo().getColor();
			puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1]] = puyo_pair.getSecondPuyo().getColor();

			puyo_pair.setPuyoPair(Puyo(), Puyo());
		}

		// Button animation
		if (pause_on) {
			button_starting += 0.3;
		}
	}

	else if (mode == FALL) { // update 함수의 내용을 실행한다. gauge를 이용해 가시적으로 보여준다. update는 fall과 remove를 동시에 수행하는 함수임을 볼 수 있다.
		falling_gauge ++;

		bool hole = false;
		if (falling_gauge == 5) {
			for (int i = 0; i < col; i++) { // 내리는 게 중요하기 때문에 바닥에서 위로 탐색을 시작한다.
				for (int j = 1; j < row; j++) { // 바닥이면 패스하므로 j = 0을 제외해버렸다.
					if (puyo_pile[i][j] == 0) continue; // 빈 공간이면 패스

					if (puyo_pile[i][j - 1] == 0) { // 아래에 뿌요가 없을 때 실행
						puyo_set[i][j - 1] = puyo_set[i][j];
						puyo_set[i][j - 1].setCenter(i, j - 1, 0);
						puyo_pile[i][j - 1] = puyo_pile[i][j];

						puyo_pile[i][j] = 0;
						puyo_set[i][j] = Puyo(); // 업데이트 함수에서는 끝까지 내렸지만 시각 효과를 위해 center를 한 칸씩 내린다.

						hole = true; // 한 번이라도 실행된다면 idle을 돌아서 한 번 더 검사한다. 이때 실행되지 않으면 false가 전달되면서 REMOVE로 넘어간다.
					}
				}
			}
			falling_gauge = 0;

			if (!hole) { mode = REMOVE; } // hole이 없다면 REMOVE로 넘어간다.
		}

		// Button animation
		if (pause_on) {
			button_starting += 0.3;
		}
	}

	else if (mode == REMOVE) {
		if (is_puyo_pop()) {
			for (auto cell : puyo_pop) {
				for (auto xy : cell) {
					puyo_pile[xy[0]][xy[1]] = 0;
					puyo_set[xy[0]][xy[1]] = Puyo();
					score += level_score[level];
				}
			}

			mode = POP; // 없어질 뿌요가 있다면 제거 후 FALL로 가서 내려올 뿌요들을 검사한다. + pop_effect를 보여준다.
		}
		else { // FALL이 끝났는데 REMOVE 사항이 없다면 맵의 업데이트가 끝났다는 의미이다.
			mode = CONTROL;
			generator = true;
			control_gauge = 0;
		}

		// Button animation
		if (pause_on) {
			button_starting += 0.3;
		}
	}

	else if (mode == POP) {
		effect_radius += 1.6;
		earned_point.setWord(to_string(score));

		if (effect_radius > radius * 3 / 2) {
			effect_radius = 0;
			puyo_pop.clear();
			mode = FALL; // POP은 REMOVE에서 FALL로 가는 중간 단계
		}

		// Button animation
		if (pause_on) {
			button_starting += 0.3;
		}
	}
	
	else if (mode == SCORE) {

		if (score_init) {
			for (int i = 0; i < 10; i++) {
				if (i == 9) { ranking[i].setCoor(-79, -404 - 297 * i); } // -79, 135 - 34*i 최종 좌표
				else ranking[i].setCoor(-75, -404 - 297 * i); // -75, 135 - 34*i 최종 좌표

				rank_set[i].setCoor(44, -404 - 297 * i); // 44, 135 - 34*i 최종 좌표
			}
			
			score_init = false;
		}

		float rank_speed = 9;
		for (int i = 0; i < 10; i++) {
			if (ranking[i][1] < ranking_coor[i]) {
				if (i == 9) { ranking[i].setCoor(-79, ranking[i][1] + rank_speed); }
				else { ranking[i].setCoor(-75, ranking[i][1] + rank_speed); }
			}
			if (rank_set[i][1] < ranking_coor[i]) {rank_set[i].setCoor(44, rank_set[i][1] + rank_speed); }
		}
		if (rank_set[9][1] >= -188) { score_sort = true; }

		if (score_sort && MAIN_color[3] < 1) {
			MAIN_color = MAIN_color + Vector4f(0.007, 0.007, 0.007, 0.007);
		}

		// Button animation
		if (MAIN_on) {
			button_starting += 0.3;
		}
}

	else if (mode == PAUSE) {
	// Button animation
		if (resume_on) {
			button_starting += 0.3;
		}
		if (quit_on) {
			button_starting += 0.3;
		}
		if (MAIN_on) {
			button_starting += 0.3;
		}
	}

	else if (mode == QUIT) {
		// Button animation
		if (quit_on) {
			button_starting += 0.3;
		}
		if (back_on) {
			button_starting += 0.3;
		}
	}

	// quit condition
	for (int i = 0; i < col; i++) {
		if (puyo_pile[i][row - 1 - level] != 0) {
			mode = END; break;
		}
	}

	if (mode == END) {
		float light_fade = 0.01;
		beam.setAmbient(beam.getAmbient()[0] - light_fade, beam.getAmbient()[1] - light_fade, beam.getAmbient()[2] - light_fade, beam.getAmbient()[3] - light_fade);
		beam.setDiffuse(beam.getDiffuse()[0] - light_fade, beam.getDiffuse()[1] - light_fade, beam.getDiffuse()[2] - light_fade, beam.getDiffuse()[3] - light_fade);
		beam.setSpecular(beam.getSpecular()[0] - light_fade, beam.getSpecular()[1] - light_fade, beam.getSpecular()[2] - light_fade, beam.getSpecular()[3] - light_fade);

		if (beam.getAmbient()[3] <= 0) {
			mode = OVER;
			
			int score_copy = score;
			init();
			earned_point.setWord(to_string(score_copy));
			
			MAIN.setCoor(-20, -100);
			point.setCoor(-45, -200);
			earned_point.setCoor(17, -200);
		}

		if (end_init) {
			// 스코어
			int score_record[10] = { 0 };

			ifstream read_score("score.txt"); // 점수 기록 스코어 파일은 점수에 해당하는 정수만 담긴다.
			for (int i = 0; i < 10; i++) {
				if (read_score.eof()) break;

				read_score >> score_record[i];
			}
			read_score.close();

			ofstream write_score("score.txt");
			bool renewal = false;
			for (int i = 0; i < 10; i++) {
				if (score > score_record[i] && !renewal) {
					write_score << score;
					renewal = true;
					write_score << "\n";
					continue;
				}

				if (renewal) write_score << score_record[i - 1];
				else write_score << score_record[i];
				write_score << "\n";
			}
			write_score.close();

			end_init = false;
		}
	}

	else if (mode == OVER) {
		// Button animation
		if (MAIN_on) {
			button_starting += 0.3;
		}
	}
	
	glutPostRedisplay();
}

void reshape(int w, int h) { // window size control
	glViewport(0, 0, w, h);

	float wRatio = (float)w / WIDTH;
	float hRatio = (float)h / HEIGHT;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-viewX * wRatio, viewX * wRatio, -viewY * hRatio, viewY * hRatio, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void show() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw 3D
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
	beam.draw();

	if (mode == START) {
		color_gauge++;

		if (color_gauge == 20) {
			color++; color_gauge = 0;
		}
		if (color == 5) { color = 1; }

		colorPuyo.setAmbient(colorset_background[color % 5]); colorPuyo.draw(); glColor3f(1, 1, 1);
		glPushMatrix(); glScalef(70, 70, 70); draw뿌(pu1, 5); glPopMatrix();

		colorPuyo.setAmbient(colorset_background[(color + 1) % 5]); colorPuyo.draw();
		glPushMatrix(); glScalef(50, 50, 50); draw요(yo1, 0.2, 4.f / 7, 4); glPopMatrix();

		colorPuyo.setAmbient(colorset_background[(color + 2) % 5]); colorPuyo.draw();
		glPushMatrix(); glScalef(40, 40, 40); draw뿌(pu2, 3.5); glPopMatrix();

		colorPuyo.setAmbient(colorset_background[(color + 3) % 5]); colorPuyo.draw();
		glPushMatrix(); glScalef(40, 40, 40); draw요(yo2, 0.2, 4.5 / 7, 2.8); glPopMatrix();

		if (pu1_phase == 5 && pu2_phase == 5 && yo1_phase == 5 && yo2_phase == 5) {
			colorPuyo.setAmbient(start_color); colorPuyo.draw();
			start.display();

			colorPuyo.setAmbient(score_board_color); colorPuyo.draw();
			score_board.display();

			if (start_on) {
				colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
				glPushMatrix(); glScalef(40, 40, 40);
				draw_button(button_starting, -1.9, -0.1); glPopMatrix();
			}
			if (score_board_on) {
				colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
				glPushMatrix(); glScalef(40, 40, 40);
				draw_button(button_starting, 1.865, -0.1); glPopMatrix();
			}
		}

		if (ani_start) {
			puyo_pair.draw();

			for (int i = 0; i < col; i++) {
				for (int j = 0; j < row; j++) {
					//if (puyo_set[i][j].getColor())
					puyo_set[i][j].draw();
				}
			}

			if (ani_mode == ANI_POP) {
				for (auto cell : puyo_pop) {
					for (auto xy : cell) {
						colorPuyo.setAmbient(1, 1, 0.7, 1); colorPuyo.draw();
						pop_effect(effect_radius, xy[0], xy[1]);
					}
				}
			}
		}
	}

	else if (mode == SCORE) {
		colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
		score_title.display();
		
		// Display score
		for (int i = 0; i < 10; i++) {
			if (i == 0) { colorPuyo.setAmbient(1, 0, 0, 1); colorPuyo.draw(); }
			else if (i == 1) { colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw(); }
			else if (i == 2) { colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw(); }
			else { colorPuyo.setAmbient(1, 1, 1, 1); colorPuyo.draw(); }
		
			ranking[i].display();

			if (rank_set[i].getInitial() == '0') { continue; }
			rank_set[i].display();
		}

		if (score_sort) {
			colorPuyo.setAmbient(MAIN_color); colorPuyo.draw();
			MAIN.display();

			if (MAIN_on) {
				colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
				glPushMatrix(); glScalef(40, 40, 40);
				draw_button(button_starting, 0, -6.6); glPopMatrix();
			}
		}
	}

	else if (mode == PAUSE) {
		colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
		MAIN.setCoor(53, -15);

		if (resume_on) {
			colorPuyo.setAmbient(0, 1, 1, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, -0.07, 2.245); glPopMatrix();

			glPushMatrix(); glScalef(20, 20, 20); glTranslatef(-0.5, 4.5, 0);
			resume(2.f); glPopMatrix();

			colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
		}
		else {
			glPushMatrix(); glScalef(20, 20, 20); glTranslatef(-0.5, 4.5, 0);
			resume(2.f); glPopMatrix();
		}

		if (quit_on) {
			colorPuyo.setAmbient(1, 0, 0, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, -1.87, -0.23); glPopMatrix();
			quit.display();
			
			colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
		}
		else { quit.display(); }

		if (MAIN_on) {
			colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, 1.82, -0.23); glPopMatrix();
			MAIN.display();
		}
		else { MAIN.display(); }
	}

	else if (mode == QUIT) {
		colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
		terminal.display();

		if (quit_on) {
			colorPuyo.setAmbient(1, 0, 0, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, -1.87, -0.23); glPopMatrix();
			quit.display();

			colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
		}
		else { quit.display(); }
		if (back_on) {
			colorPuyo.setAmbient(1, 1, 0, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, 1.75, -0.23); glPopMatrix();
			back.display();
		}
		else { back.display(); }
	}

	else if (mode == OVER) {
		colorPuyo.setAmbient(1, 0, 0, 1); colorPuyo.draw();
		gameover.display();

		if (MAIN_on) {
			colorPuyo.setAmbient(0, 0, 1, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(40, 40, 40);
			draw_button(button_starting, 0, -2.34); glPopMatrix();
			MAIN.display();
		}
		else {
			colorPuyo.setAmbient(0, 1, 0, 1); colorPuyo.draw();
			MAIN.display();
		}

		colorPuyo.setAmbient(0, 1, 1, 1); colorPuyo.draw();
		point.display();
		earned_point.display();
	}

	else {
		switch (level) {
			case 0:
				colorPuyo.setAmbient(0, 0.5, 0.5, 1); colorPuyo.draw();
				draw_puyo_field(level); break;
			case 1:
				colorPuyo.setAmbient(0.5, 0.4, 0.05, 1); colorPuyo.draw();
				draw_puyo_field(level); break;
			case 2:
				colorPuyo.setAmbient(1, 0, 0, 1); colorPuyo.draw();
				draw_puyo_field(level);
		}
		
		puyo_pair.draw();
		prepuyo1.draw(); prepuyo2.draw();

		for (int i = 0; i < col; i++) {
			for (int j = 0; j < row; j++) {
					puyo_set[i][j].draw();
			}
		}

		if (mode == POP) {
			for (auto cell : puyo_pop) {
				for (auto xy : cell) {
					colorPuyo.setAmbient(1, 1, 0.7, 1); colorPuyo.draw();
					pop_effect(effect_radius, xy[0], xy[1]);
				}
			}
		}
		else { colorPuyo.setAmbient(0, 1, 1, 1); colorPuyo.draw(); }

		point.display();
		earned_point.display();

		if (pause_on) {
			colorPuyo.setAmbient(1, 0, 1, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(10, 10, 10); glTranslatef(10.1, -25.3, 0);
			pause(0.8); glPopMatrix();

			glPushMatrix(); glScalef(28, 28, 28);
			draw_button(button_starting, 3.79, -9.03); glPopMatrix();
		}
		else {
			colorPuyo.setAmbient(0.5, 1, 0, 1); colorPuyo.draw();
			glPushMatrix(); glScalef(10, 10, 10); glTranslatef(10.1, -25.3, 0);
			pause(0.8); glPopMatrix();
		}

		if (mode == END) {
			
		}
	}

	glDisable(GL_LIGHT0); glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	if (key == 27) {
		if (mode == START || mode == SCORE) {
			quit_mode_copy = mode;
			mode = QUIT;
		}
		else if (mode == QUIT || mode == PAUSE) {
			exit(0);
		}
		else { 
			if (mode != OVER && mode != END) {
				resume_mode_copy = mode;
				mode = PAUSE;
			}
		}
	}

	if (mode == CONTROL) {
		if (key == 32) {
			control_gauge = 0;
			
			if (puyo_pair.getRotate() == 0 || puyo_pair.getRotate() == 2) { // 수평
				while (1) {
					if (puyo_pair.getFirstPuyo().getCenter()[1] == 0 || puyo_pair.getSecondPuyo().getCenter()[1] == 0) { break; }
					else if (puyo_pile.at(puyo_pair.getFirstPuyo().getCenter()[0]).at(puyo_pair.getFirstPuyo().getCenter()[1] - 1) != 0 \
						|| puyo_pile.at(puyo_pair.getSecondPuyo().getCenter()[0]).at(puyo_pair.getSecondPuyo().getCenter()[1] - 1) != 0) { break; }

					puyo_pair.down();
				}
			}
			else if (puyo_pair.getRotate() == 1) { // 2번 뿌요가 아래에 있는 상태
				while (1) {
					if (puyo_pair.getSecondPuyo().getCenter()[1] == 0) { break; }
					else if (puyo_pile.at(puyo_pair.getSecondPuyo().getCenter()[0]).at(puyo_pair.getSecondPuyo().getCenter()[1] - 1) != 0) { break; }

					puyo_pair.down();
				}
			}
			else if (puyo_pair.getRotate() == 3) { // 1번 뿌요가 아래에 있는 상태
				while (1) {
					if (puyo_pair.getFirstPuyo().getCenter()[1] == 0) { break; }
					else if (puyo_pile.at(puyo_pair.getFirstPuyo().getCenter()[0]).at(puyo_pair.getFirstPuyo().getCenter()[1] - 1) != 0) { break; }

					puyo_pair.down();
				}
			}
		}
	}
}

void specialKeyDown(int key, int x, int y) {
	if (mode == CONTROL) {
		switch (key) {
		case GLUT_KEY_LEFT:
			if (puyo_pair.getFirstPuyo().getCenter()[0] > 0 && puyo_pair.getSecondPuyo().getCenter()[0] > 0) {
				if ((puyo_pair.getRotate() == 0 && puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0] - 1][puyo_pair.getFirstPuyo().getCenter()[1]] == 0) \
					|| (puyo_pair.getRotate() == 2 && puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] - 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0) \
					|| ((puyo_pair.getRotate() == 1 || puyo_pair.getRotate() == 3) && puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0] - 1][puyo_pair.getFirstPuyo().getCenter()[1]] == 0 && \
						puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] - 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0))
					puyo_pair.left();
			}
			break;

		case GLUT_KEY_RIGHT:
			if (puyo_pair.getFirstPuyo().getCenter()[0] < col - 1 && puyo_pair.getSecondPuyo().getCenter()[0] < col - 1) {
				if ((puyo_pair.getRotate() == 0 && puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] + 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0) \
					|| (puyo_pair.getRotate() == 2 && puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0] + 1][puyo_pair.getFirstPuyo().getCenter()[1]] == 0) \
					|| ((puyo_pair.getRotate() == 1 || puyo_pair.getRotate() == 3) && puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0] + 1][puyo_pair.getFirstPuyo().getCenter()[1]] == 0 && \
						puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] + 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0))
					puyo_pair.right();
			}
			break;

		case GLUT_KEY_DOWN:
			if (puyo_pair.getFirstPuyo().getCenter()[1] > 1 && puyo_pair.getSecondPuyo().getCenter()[1] > 1) {
				if (puyo_pile[puyo_pair.getFirstPuyo().getCenter()[0]][puyo_pair.getFirstPuyo().getCenter()[1] - 2] == 0 && \
					puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1] - 2] == 0)
					puyo_pair.down();
			}
			break;

		case GLUT_KEY_UP:
			if (puyo_pair.getRotate() == 0 && puyo_pair.getFirstPuyo().getCenter()[1] < row - 1 && puyo_pair.getSecondPuyo().getCenter()[1] < row - 1 - level) // 왼쪽 -> 위쪽
				puyo_pair.rotate();

			else if (puyo_pair.getRotate() == 1 && puyo_pair.getFirstPuyo().getCenter()[0] < col - 1 && puyo_pair.getSecondPuyo().getCenter()[0] < col - 1) { // 위쪽 -> 오른쪽
				if (puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] + 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0)
					puyo_pair.rotate();
			}

			else if (puyo_pair.getRotate() == 2 && puyo_pair.getFirstPuyo().getCenter()[1] > 0 && puyo_pair.getSecondPuyo().getCenter()[1] > 0) { // 오른쪽 -> 아래쪽
				if (puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0]][puyo_pair.getSecondPuyo().getCenter()[1] - 1] == 0)
					puyo_pair.rotate();
			}

			else if (puyo_pair.getRotate() == 3 && puyo_pair.getFirstPuyo().getCenter()[0] > 0 && puyo_pair.getSecondPuyo().getCenter()[0] > 0) { // 아래쪽 -> 왼쪽
				if (puyo_pile[puyo_pair.getSecondPuyo().getCenter()[0] - 1][puyo_pair.getSecondPuyo().getCenter()[1]] == 0)
					puyo_pair.rotate();
			}
		}
	}
}

void mouse(int button, int state, int x, int y) {
	//printf("(%d, %d, %d, %d)\n", button, state, x, y); // 커서 위치 탐색 코드
	if (mode == START) {
		// 게임 시작
		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 68 && x < 113 && y > 322 && y < 343) {
			mode = CONTROL;

			puyo_set = vector<vector<Puyo>>(col, vector<Puyo>(row + 1, Puyo()));
			puyo_pile = vector<vector<int>>(col, vector<int>(row + 1, 0));

			puyo_pop.clear();
			pop.clear();

			is_checked = vector<vector<bool>>(col, vector<bool>(row, false));

			vector<vector<bool>> is_checked(col, vector<bool>(row, false));

			generator = false;
			puyo_pair.setPuyoPair(Puyo(), Puyo());
		}

		// 점수 보기
		else if (button == GLUT_LEFT_BUTTON && state == 0 && x > 216 && x < 280 && y > 322 && y < 343) {
			mode = SCORE;
		}
	}

	else if (mode == SCORE) {
		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 147 && x < 186 && y > 581 && y < 603) {
			mode = START;

			init();
			generator = false;
			puyo_pair.setPuyoPair(Puyo(), Puyo());
		}
	}

	else if (mode == OVER) {
		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 147 && x < 186 && y > 414 && y < 430) {
			mode = START;

			init();
		}
	}

	else if (mode == QUIT) {

		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 74 && x < 110 && y > 329 && y < 347) {
			exit(0);
		}

		else if (button == GLUT_LEFT_BUTTON && state == 0 && x > 217 && x < 260 && y > 329 && y < 347) {
			if (quit_mode_copy == START) { 
				mode = START;

				init();
				generator = false;
				puyo_pair.setPuyoPair(Puyo(), Puyo());
			}
			else if (quit_mode_copy == SCORE) { mode = SCORE; }
			back_on = false;
		}
	}

	else if (mode == PAUSE) {

		
		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 155 && x < 179 && y > 217 && y < 263) {
			mode = resume_mode_copy;
			resume_on = false;
		}

		else if (button == GLUT_LEFT_BUTTON && state == 0 && x > 74 && x < 110 && y > 329 && y < 347) {
			exit(0);
		}

		else if (button == GLUT_LEFT_BUTTON && state == 0 && x > 220 && x < 260 && y > 329 && y < 346) {
			mode = START;

			init();
			generator = false;
			puyo_pair.setPuyoPair(Puyo(), Puyo());
			
			MAIN_on = false;
		}
	}

	else {
		if (button == GLUT_LEFT_BUTTON && state == 0 && x > 266 && x < 280 && y > 572 && y < 595) {
			resume_mode_copy = mode;
			mode = PAUSE;
			pause_on = false;
		}
	}
}

void passive_mouse(int x, int y) {

	if (mode == START) {

		// Button Color
		if (x > 69 && x < 112 && y > 323 && y < 342) {
			if (!start_on) { start_color_copy = start_color; }

			start_on = true;
			start_color = { 1, 1, 0, 1 };
		}
		else {
			if (start_on) {
				start_on = false;
				start_color = start_color_copy;
			}
		}

		if (x > 217 && x < 269 && y > 323 && y < 342) {
			if (!score_board_on) { score_board_color_copy = score_board_color; }

			score_board_on = true;
			score_board_color = { 1, 1, 0, 1 };
		}
		else {
			if (score_board_on) {
				score_board_on = false;
				score_board_color = score_board_color_copy;
			}
		}
	}

	else if (mode == SCORE) {

		// Button Color
		if (x > 147 && x < 186 && y > 581 && y < 603) {
			if (!MAIN_on) { MAIN_color_copy = MAIN_color; }

			MAIN_on = true;
			MAIN_color = { 1, 1, 0, 1 };
		}
		else {
			if (MAIN_on) {
				MAIN_on = false;
				MAIN_color = MAIN_color_copy;
			}
		}
	}

	else if (mode == OVER) {
		if (x > 147 && x < 186 && y > 414 && y < 430) { MAIN_on = true; }
		else { MAIN_on = false; }
	}

	else if (mode == QUIT) {

		// Button Color
		if (x > 74 && x < 110 && y > 329 && y < 347) { quit_on = true; }
		else { quit_on = false; }

		if (x > 217 && x < 260 && y > 329 && y < 347) { back_on = true; }
		else { back_on = false; }
	}

	else if (mode == PAUSE) {

		// Button Color
		if (x > 155 && x < 179 && y > 217 && y < 263) { resume_on = true; }
		else { resume_on = false; }

		if (x > 74 && x < 110 && y > 329 && y < 347) { quit_on = true; }
		else { quit_on = false; }

		if (x > 220 && x < 260 && y > 329 && y < 346) { MAIN_on = true; }
		else { MAIN_on = false; }
	}

	else {
		
		// Button Color
		if (x > 266 && x < 280 && y > 572 && y < 595) { pause_on = true; }
		else { pause_on = false; }
	}
}

int main(int argc, char** argv) {
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(650, 250);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("뿌요뿌요");
	init();

	// register callbacks // call back을 여러 개 만들어 상황에 따라 다른 key, mouse 반응, 화면 재생을 만들자
	glutDisplayFunc(show);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(specialKeyDown);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passive_mouse);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}