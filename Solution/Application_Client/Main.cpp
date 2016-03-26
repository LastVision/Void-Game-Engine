#include "stdafx.h"
#include <DL_Debug.h>
#include <iostream>
#include <TimerManager.h>

void main()
{
	bool isRunning = true;
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();

	float deltaTime = 0;
	while (isRunning == true)
	{
		CU::TimerManager::GetInstance()->Update();
		deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
		
		std::cout << "deltaTime: " << deltaTime << "\nFPS: " << CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFPS() << std::endl;
	}


	system("pause");
}