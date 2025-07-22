#pragma once
#include ".\Library\GameObject.h"
#include "global.h"
#include <vector>


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

class Player;

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
	bool CanSeePlayer(Player *p);
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
	void DrawFieldOfViewArc_PureDxLib(float fovAngleDeg = 90.0f,int   viewDistanceTiles = 5, int   numSegments = 25) const;
	//������͈͂̃s�b�N�A�b�v��������g���āA���E�̃^�C����h��
	std::vector<Point> GetViewTiles(float angle, int dist);

};

