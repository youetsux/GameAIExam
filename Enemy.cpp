#include "Enemy.h"
#include "time.h"
#include "Player.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float ENEMY_MOVE_INTERVAL = 0.2f; //敵の移動間隔
	const float ENEMY_CHASE_INTERVAL = 0.15f; //敵の追いかける間隔

	const int THRESHOLD_DIST = 10;
	const float INIT_CHASE_TIME = 3.0; //10s追いかける

}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	//pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	//↑ステージ内のランダム位置にスポーン
	// 39x21
	//  幅で見ると　0,39が石のなか　ステージは　1～38
	//  高で見ると  0,21が石のなか　ステージは  1～20
	//GetRand(x)は　0～xまでの整数が発生　1～３８，1～２０の乱数を出すには？
	Point rndNum = { GetRand(STAGE_WIDTH - 2) + 1, GetRand(STAGE_HEIGHT - 2) + 1 };
	pos_ = {rndNum.x * ENEMY_DRAW_SIZE, rndNum.y * ENEMY_DRAW_SIZE};
	dir_ = INIT_ENEMY_DIR;
	state_ = ESTATE::NORMAL;
	chaseTime_ = INIT_CHASE_TIME;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//プレイヤーが、10,15 にいたら、escapeモード
	switch (state_)
	{
	case ESTATE::NORMAL:
		UpdateNormal();
		break;
	case ESTATE::CHASE:
		UpdateChase();
		break;
	case ESTATE::ESCAPE:
		break;
	default:
		return;
	}

}

void Enemy::UpdateNormal()
{
	//GetRand(数値)
//3秒に1回向きをランダムに変える
//static float dir_timer = 3.0f;
	static float prog_timer = ENEMY_MOVE_INTERVAL;
	float dt = Time::DeltaTime();
	//dir_timer = dir_timer - dt;
	prog_timer = prog_timer - dt;
	//if (dir_timer < 0.0f)
	//{
	//	//dir_ = (DIR)(GetRand(3));
	//	//TurnLeft();
	//	TurnBack();
	//	dir_timer = 3.0f + dir_timer;
	//}

	Point newPos = pos_;
	if (prog_timer < 0.0f)
	{
		switch (dir_)
		{
		case UP:
			newPos.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			newPos.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			newPos.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			newPos.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}
		//移動先がステージの外に出ないようにする
		if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
			|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
		{
			pos_ = newPos;

		}
		else
		{
			TurnLeft();
		}

		Player* p = FindGameObject<Player>();
		Point pPos = p->GetPlayerPos();
		//プレイヤーとのマンハッタン距離がTHRESHOLD_DISTより小さくなったら
		//CHASE
			// マンハッタン距離の計算: |x1 - x2| + |y1 - y2|
		int distance = (std::abs(pPos.x - pos_.x) +
			std::abs(pPos.y - pos_.y))/ ENEMY_DRAW_SIZE;
		if (distance <= 10) {
			state_ = ESTATE::CHASE;
			printfDx("STATE CHANGE->CHASE!\n");
		}

		prog_timer = ENEMY_MOVE_INTERVAL + prog_timer;
	}
}

void Enemy::UpdateChase()
{
	//10秒おいかけてNORMALに戻る
	float dt = Time::DeltaTime();
	if (chaseTime_ < 0)
	{
		state_ = ESTATE::NORMAL;
		chaseTime_ = INIT_CHASE_TIME;
		printfDx("STATE CHANGE->NORMAL!\n");
	}
	else {
		//マンハッタン距離を減らそう！
		Player* p = FindGameObject<Player>();
		Point pPos = p->GetPlayerPos();
		int xDist = abs(pPos.x - pos_.x);
		int yDist = abs(pPos.y - pos_.y);
		if (xDist > yDist) 
		{
			if (pPos.x > pos_.x) {
				dir_ = DIR::RIGHT;
			}
			else {
				dir_ = DIR::LEFT;
			}
		}
		else if (xDist < yDist)
		{
			if (pPos.y > pos_.y) {
				dir_ = DIR::DOWN;
			}
			else {
				dir_ = DIR::UP;
			}
		}

		static float prog_timer = ENEMY_CHASE_INTERVAL;
		Point newPos = pos_;
		if (prog_timer < 0.0f)
		{
			switch (dir_)
			{
			case UP:
				newPos.y -= ENEMY_DRAW_SIZE;
				break;
			case DOWN:
				newPos.y += ENEMY_DRAW_SIZE;
				break;
			case LEFT:
				newPos.x -= ENEMY_DRAW_SIZE;
				break;
			case RIGHT:
				newPos.x += ENEMY_DRAW_SIZE;
				break;
			default:
				break;
			}
			//移動先がステージの外に出ないようにする
			if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
				|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
			{
				pos_ = newPos;

			}
			prog_timer = ENEMY_CHASE_INTERVAL + prog_timer;
		}
		prog_timer = prog_timer - dt;
		chaseTime_ = chaseTime_ - dt;
	}
}

void Enemy::UpdateEscape()
{
}



void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	//animTimer = animTimer - Time::DeltaTime();
	//if (state_ == ESTATE::CHASE)
	//{
	//	printfDx("CHASE!\n");
	//}
}

void Enemy::TurnLeft()
{
	switch(dir_)
	{
	case   UP:
		dir_ = DIR::LEFT;
		break;
	case DOWN:
		dir_ = DIR::RIGHT;
		break;
	case LEFT:
		dir_ = DIR::DOWN;
		break;
	case RIGHT:
		dir_ = DIR::UP;
		break;
	default:
		break;
	}
}

void Enemy::TurnRight()
{
	switch (dir_)
	{
	case   UP:
		dir_ = DIR::RIGHT;
			break;
	case DOWN:
		dir_ = DIR::LEFT;
			break;
	case LEFT:
		dir_ = DIR::UP;
			break;
	case RIGHT:
		dir_ = DIR::DOWN;
			break;
	default:
		break;
	}
}

void Enemy::TurnBack()
{
	switch (dir_)
	{
	case   UP:
		dir_ = DIR::DOWN;
			break;
	case DOWN:
		dir_ = DIR::UP;
			break;
	case LEFT:
		dir_ = DIR::RIGHT;
			break;
	case RIGHT:
		dir_ = DIR::LEFT;
			break;
	default:
		break;
	}
}

