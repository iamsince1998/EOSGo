// Copyright (c) 2024 Fedahumada Studio. All Rights Reserved.

#include "Game/GoGameModeBase.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/LocalPlayer.h"
#include "Game/GoGameStateBase.h"
#include "Subsystem/GoSubsystem.h"


AGoGameModeBase::AGoGameModeBase() :
/*Bind Delegates*/
RegisterPlayersCompleteDelegate(FOnRegisterPlayersCompleteDelegate::CreateUObject(this, &ThisClass::OnRegisteredPlayerComplete))
//UnregisterPlayersCompleteDelegate(FOnUnregisterPlayersCompleteDelegate::CreateUObject(this, &ThisClass::OnUnregisteredPlayerComplete))
{
	GameStateClass = AGoGameStateBase::StaticClass();
	
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		Identity = Subsystem->GetIdentityInterface();
	}
	
}

void AGoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);	
	GoRegisterPlayer(NewPlayer);
}
void AGoGameModeBase::Logout(AController* ExitingPlayer)
{
	ExitingPlayerController = Cast<APlayerController>(ExitingPlayer);
	//GoUnregisterPlayer();
	Super::Logout(ExitingPlayer);
}


void AGoGameModeBase::OnRegisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess)
{
	//~ If registration was successful, clear delegate of the delegate list.
	if (SessionInterface) SessionInterface->ClearOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayersCompleteDelegateHandle);

	//~ Broadcast Go GameModeBase Delegate - Registration successful.
	GoOnRegisterPlayerComplete.Broadcast(bWasSuccess);
	LogMessage("Registration Successful");
	
}
void AGoGameModeBase::GoRegisterPlayer(APlayerController* NewPlayer)
{
	if(NewPlayer == nullptr) return;
	if (!GoGetUniqueNetId(NewPlayer).IsValid()) return;
	
	//~ Store the delegate in a FDelegateHandle, so we can later remove it from the delegate list.
	RegisterPlayersCompleteDelegateHandle = SessionInterface->AddOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayersCompleteDelegate);
	
	//~ REGISTER PLAYER
	if(!SessionInterface->RegisterPlayer(NAME_GameSession, *GoGetUniqueNetId(NewPlayer), false))
	{
		//~ If it doesn't register the player, clear delegate of the delegate list.
		SessionInterface->ClearOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayersCompleteDelegateHandle);
		//~ Broadcast Go GameModeBase Delegate - Registration not successful.
		LogMessage("Registration Failed");
		GoOnRegisterPlayerComplete.Broadcast(false);
	}
}


/*void AGoGameModeBase::OnUnregisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess)
{
	//~ If unregistration was successful, clear delegate of the delegate list.
	if (SessionInterface) SessionInterface->ClearOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayersCompleteDelegateHandle);

	//~ Broadcast Go GameModeBase Delegate - Unregistration successful.
	UnregisteredPlayerUserName = FName(ExitingPlayerController->PlayerState->GetPlayerName());
	GoOnUnregisterPlayerComplete.Broadcast(UnregisteredPlayerUserName);
	LogMessage("Unregistration Successful");
}
void AGoGameModeBase::GoUnregisterPlayer()
{
	if(ExitingPlayerController == nullptr) return;
	
	//~ Store the delegate in a FDelegateHandle, so we can later remove it from the delegate list.
	UnregisterPlayersCompleteDelegateHandle = SessionInterface->AddOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayersCompleteDelegate);
	
	//~ UNREGISTER PLAYER
	if(!SessionInterface->UnregisterPlayer(NAME_GameSession, *ExitingPlayerController->PlayerState->GetUniqueId()))
	{
		//~ If it doesn't unregister the player, clear delegate of the delegate list.
		SessionInterface->ClearOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayersCompleteDelegateHandle);
		//~ Broadcast Go GameModeBase Delegate - Unregistration not successful.
		LogMessage("Unregistration Failed");
		GoOnUnregisterPlayerComplete.Broadcast("Unknown");
	}
}*/


TSharedPtr<const FUniqueNetId> AGoGameModeBase::GoGetUniqueNetId(APlayerController* InPlayer) const
{
	if (!InPlayer) return TSharedPtr<const FUniqueNetId>();
	
	FUniqueNetIdRepl UniqueNetIdRepl;
	const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayer->Player);

	if (InPlayer->IsLocalController())
	{
		if (const ULocalPlayer* LocalPlayer = InPlayer->GetLocalPlayer())
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
			LogMessage("Local Player ID obtained.");
		}
		else if (IsValid(RemoteNetConnection))
		{
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
			LogMessage("Local Player RemoteNetConnection ID obtained.");
		}
	}
	else if (IsValid(RemoteNetConnection))
	{
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		LogMessage("Remote Player ID obtained.");
	}

	const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	if (!UniqueNetId.IsValid())
	{
		LogMessage("UniqueNetId is invalid or null.");
		return TSharedPtr<const FUniqueNetId>();
	}

	return UniqueNetId;
}
