// Copyright (c) 2024 Fedahumada Studio. All Rights Reserved.

#include "Game/GoGameModeBase.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameState.h"
#include "Subsystem/GoSubsystem.h"


AGoGameModeBase::AGoGameModeBase() /*:
/*Bind Delegates#1#
RegisterPlayersCompleteDelegate(FOnRegisterPlayersCompleteDelegate::CreateUObject(this, &ThisClass::OnRegisteredPlayerComplete))*/
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void AGoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);	
	GoRegisterPlayer(NewPlayer);
	/*
	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	FString PlayerUsername = PlayerState->GetPlayerName();
	int32 PlayerId = PlayerState->GetPlayerId();

	if (GEngine && bDisplayScreenMessage)
	{
		GEngine->AddOnScreenDebugMessage(999, 999.f, FColor::Blue, TEXT("Players in game:"));
		GEngine->AddOnScreenDebugMessage(PlayerId, 999.f, FColor::Green, FString::Printf(TEXT("%s - Connected"), *PlayerUsername));
	}
	*/
}

void AGoGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	/*
	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	FString PlayerUsername = PlayerState->GetPlayerName();
	int32 PlayerId = PlayerState->GetPlayerId();
	if (GEngine && bDisplayScreenMessage) GEngine->AddOnScreenDebugMessage(PlayerId, 999.f, FColor::Red, FString::Printf(TEXT("%s - Disconnected"), *PlayerUsername));
	*/

}


void AGoGameModeBase::OnRegisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess)
{
	for (FUniqueNetIdRef Player : PlayerId)
	{
		APlayerState* PlayerState = GameState->GetPlayerStateFromUniqueNetId(Player);
		RegisteredPlayer = FName(PlayerState->GetPlayerName());
		GoOnRegisterPlayerComplete.Broadcast(RegisteredPlayer);
		LogMessage("Registration Successful");
	}
	
}
void AGoGameModeBase::GoRegisterPlayer(APlayerController* NewPlayer)
{
	if(NewPlayer == nullptr) return;

	//~ Bind login callback.
	SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(this, &AGoGameModeBase::OnRegisteredPlayerComplete);

	//~ REGISTER PLAYER
	FUniqueNetIdRepl UniqueNetIdRepl;
	if(NewPlayer->IsLocalController())
	{
		if(const ULocalPlayer* LocalPlayer = NewPlayer->GetLocalPlayer())
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	if(!SessionInterface->RegisterPlayer(NAME_GameSession, *UniqueNetId, false))
	{
		LogMessage("Registration Failed");
		GoOnRegisterPlayerComplete.Broadcast("Unknown");
	}
}
