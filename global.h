#pragma once

//2�����x�N�g���i�����j
struct Point
{
	int x;
	int y;
};
//2�����x�N�g���i���������_���j
struct Pointf
{
	float x;
	float y;
};

//��`��\����
struct Rect {
	int x;//����x
	int y;//����y
	int w;//��
	int h;//����
};

enum DIR
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE,
	MAX_DIR
};

const int STAGE_WIDTH = 39;
const int STAGE_HEIGHT = 21;
const int CHA_SIZE = 32; //�L�����̃T�C�Y 32*32