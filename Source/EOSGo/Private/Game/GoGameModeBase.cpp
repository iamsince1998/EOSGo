// © 2024 fedahumada. Almost all rights reserved. Violators will be forced to debug code written in Comic Sans.


#include "Game/GoGameModeBase.h"

void AGoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGoGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
