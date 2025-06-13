#include "Enemy.h"
#include "time.h"
#include "Player.h"

namespace
{
	const int ENEMY_SIZE = 48; //�G�̃T�C�Y 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //�G�̏����ʒu
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //�G�̕`��T�C�Y
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float ENEMY_MOVE_INTERVAL = 0.2f; //�G�̈ړ��Ԋu
	const float ENEMY_CHASE_INTERVAL = 0.15f; //�G�̒ǂ�������Ԋu

	const int THRESHOLD_DIST = 10;
	const float INIT_CHASE_TIME = 3.0; //10s�ǂ�������

}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	//pos_ = ENEMY_START_POS; //32�̓u���b�N�̈ʒupos_
	//���X�e�[�W���̃����_���ʒu�ɃX�|�[��
	// 39x21
	//  ���Ō���Ɓ@0,39���΂̂Ȃ��@�X�e�[�W�́@1�`38
	//  ���Ō����  0,21���΂̂Ȃ��@�X�e�[�W��  1�`20
	//GetRand(x)�́@0�`x�܂ł̐����������@1�`�R�W�C1�`�Q�O�̗������o���ɂ́H
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
	//�v���C���[���A10,15 �ɂ�����Aescape���[�h
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
	//GetRand(���l)
//3�b��1������������_���ɕς���
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
		//�ړ��悪�X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
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
		//�v���C���[�Ƃ̃}���n�b�^��������THRESHOLD_DIST��菬�����Ȃ�����
		//CHASE
			// �}���n�b�^�������̌v�Z: |x1 - x2| + |y1 - y2|
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
	//10�b����������NORMAL�ɖ߂�
	float dt = Time::DeltaTime();
	if (chaseTime_ < 0)
	{
		state_ = ESTATE::NORMAL;
		chaseTime_ = INIT_CHASE_TIME;
		printfDx("STATE CHANGE->NORMAL!\n");
	}
	else {
		//�}���n�b�^�����������炻���I
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
			//�ړ��悪�X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
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

