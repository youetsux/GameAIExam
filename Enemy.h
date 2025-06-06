#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

namespace
{
	enum ESTATE
	{
		NORMAL, //通常営業
		CHASE,  //追いかけてくる
		ESCAPE, //逃げ
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
	int hImage_;//画像ID
	Point pos_;//位置
	DIR dir_;//移動方向
	ESTATE state_; //現在のパンダの精神状態
	float chaseTime_;
	void UpdateNormal();
	void UpdateChase();
	void UpdateEscape();
};

