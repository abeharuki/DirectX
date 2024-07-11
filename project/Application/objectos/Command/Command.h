#pragma once
#include <ViewProjection.h>
#include <Sprite.h>


class Command {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection& camera);

	void DrawUI();

private:
	void MoveArrow();

private:
	std::unique_ptr<Sprite> backgroundTask;
	std::unique_ptr<Sprite> attack;
	std::unique_ptr<Sprite> magic;
	std::unique_ptr<Sprite> skill;
	std::unique_ptr<Sprite> tool;
	std::unique_ptr<Sprite> operation;
	std::unique_ptr<Sprite> arrow;


	Vector3 arrowPos_;
	bool arrowFlag_;

	Vector2 pos;
	Vector2 size;
};