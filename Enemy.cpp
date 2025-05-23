#include "Enemy.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const int ENEMY_SPEED = 2; //敵の移動速度
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { DOWN };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//GetRand(数値)
	//3秒に1回向きをランダムに変える

}

void Enemy::Draw()
{
	Rect iRect[4] = {
		{ 0, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ 0, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ 0, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{ 0, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
}
