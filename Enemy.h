#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

namespace
{
	enum ESTATE
	{
		NORMAL, //�ʏ�c��
		CHASE,  //�ǂ������Ă���
		ESCAPE, //����
		MAX_ESTATE
	};
}


class Enemy :
    public GameObject
{
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
	void TurnLeft();
	void TurnRight();
	void TurnBack();
protected:
private:
	int hImage_;//�摜ID
	Point pos_;//�ʒu
	DIR dir_;//�ړ�����
	ESTATE state_; //���݂̃p���_�̐��_���
	float chaseTime_;
	void UpdateNormal();
	void UpdateChase();
	void UpdateEscape();
};

