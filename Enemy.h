#pragma once
#include ".\Library\GameObject.h"
#include "global.h"
#include <vector>


namespace
{
	enum ESTATE
	{
		PATROL, //èÑâÒ
		CHASE,  //í«ê’
		ATTACK, //çUåÇ
		SEARCH, //íTçı
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
	int hImage_;
	Point pos_;
	DIR dir_;
	ESTATE state_;
	float stateTimer_;
	float moveTimer_;
	void UpdatePatrol();
	void UpdateChase();
	void UpdateAttack();
	void UpdateSearch();
	bool MoveOneStep();
	std::vector<Point> GetViewTiles(float angle, int dist);
	std::vector<Point> viewTiles_;

};

