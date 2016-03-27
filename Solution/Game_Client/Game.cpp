#include "stdafx.h"
#include "Game.h"

Game::Game()
{}

Game::~Game()
{}

bool Game::Initialize()
{
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update(float aDeltaTime)
{
	aDeltaTime;
	return true;
}

void Game::Pause()
{}

void Game::Unpause()
{}

void Game::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}