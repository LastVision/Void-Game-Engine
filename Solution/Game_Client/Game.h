#pragma once
class Game
{
public:
	Game();
	~Game();

	bool Initialize();
	bool Destroy();
	bool Update(float aDeltaTime);

	void Pause();
	void Unpause();
	void OnResize(int aWidth, int aHeight);

private:

};

