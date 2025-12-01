#pragma once


class EventManager
{
public:

	EventManager() {}	//コンストラクタ
	EventManager(const EventManager &) = delete;				//コピーコンストラクタ禁止
	void operator=(const EventManager &) = delete;	//代入演算子禁止

	static EventManager *GetInstance();	//シングルトンインスタンス取得

	const int bulletCountMax = 6; //最大弾数

	bool itemPickup[2] = { false, false }; //アイテム取得フラグ
	float teamHP[2] = { 1.0f, 1.0f }; //チームのHP
	int teamBulletCount[2] = { bulletCountMax, bulletCountMax }; //チームの弾数
	bool gameOver = false;

	void TakeDamage(int teamID, float damage); //ダメージ処理関数
	void AddBullets(int count); //弾数追加関数
	bool UseBullet(int teamID); //弾数使用関数
};