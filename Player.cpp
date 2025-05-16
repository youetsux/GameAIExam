#include "Player.h"

Player::Player()
	: GameObject(), pos_({ 2*32, 2*32 })//32はブロックの位置
{
}

Player::~Player()
{
}

void Player::Update()
{
}

void Player::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_SIZE, pos_.y + CHA_SIZE,
		 GetColor(0, 255, 0), TRUE);
}
