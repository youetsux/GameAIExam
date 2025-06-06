#pragma once
#include ".\Library\GameObject.h"
#include "global.h"


class Enemy :
    public GameObject
{
	int hImage_;//�摜ID
	Point pos_;//�ʒu
	DIR dir_;//�ړ�����
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
	void TurnLeft();
	void TurnRight();
	void TurnBack();
};

