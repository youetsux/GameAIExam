#include "Stage.h"



Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
	// ステージの描画
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
			DrawBox(x * CHA_SIZE, y * CHA_SIZE, (x + 1) * CHA_SIZE, (y + 1) * CHA_SIZE, GetColor(255, 0,0), FALSE, 1);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			if (x == 0 || x == STAGE_WIDTH - 1 || y == 0 || y == STAGE_HEIGHT - 1)
			{
				DrawBox(x * CHA_SIZE, y * CHA_SIZE, (x + 1) * CHA_SIZE, (y + 1) * CHA_SIZE, GetColor(182,82,51), TRUE);
			}
		}
	}
	DrawBox(18 * CHA_SIZE, 10 * CHA_SIZE, (18 + 1) * CHA_SIZE, (10 + 1) * CHA_SIZE, GetColor(255, 255, 0), TRUE);
	DrawBox(18 * CHA_SIZE, 10 * CHA_SIZE, (18 + 1) * CHA_SIZE, (10 + 1) * CHA_SIZE, GetColor(255, 0, 0), FALSE, 2);
	DrawFormatString(18 * CHA_SIZE+13, 10 * CHA_SIZE+10, GetColor(255, 0, 0), "P");
}
