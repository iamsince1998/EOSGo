// Copyright (c) 2024 Fedahumada Studio. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameMode.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystem/GoSubsystem.h"
#include "GoGameModeBase.generated.h"

//~ GO GAME MODE DELEGATES
							//For GoOverlay class to bind callbacks to.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoOnRegisterPlayerComplete, bool, bWasSuccess);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoOnUnregisterPlayerComplete, FName, Username);

/**
 * 
 */
UCLASS()
class EOSGO_API AGoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGoGameModeBase();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingPlayer) override;
	
	//~ To handle Registered Players functionality.
	void GoRegisterPlayer(APlayerController* NewPlayer);
	UPROPERTY(BlueprintAssignable, Category="EOS-Go|Session")
	FGoOnRegisterPlayerComplete GoOnRegisterPlayerComplete;

	/*void GoUnregisterPlayer();
	UPROPERTY(BlueprintAssignable, Category="EOS-Go|Session")
	FGoOnUnregisterPlayerComplete GoOnUnregisterPlayerComplete;*/
	

protected:
	//~ To handle Registered Players functionality.
	void OnRegisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess);
	///void OnUnregisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess);
	
private:
	//The subsystem designed to handle online functionality.
	///TObjectPtr<UGoSubsystem> GoSubsystem;
	IOnlineSessionPtr SessionInterface;
	IOnlineIdentityPtr Identity;

	TObjectPtr<APlayerController> ExitingPlayerController;
	FName RegisteredPlayer{FName()};
	///FName UnregisteredPlayerUserName{FName()};
	
	//~ Delegates to add to the Online Session Interface delegate list. Each one has its own handle.
	FOnRegisterPlayersCompleteDelegate RegisterPlayersCompleteDelegate;
	FDelegateHandle RegisterPlayersCompleteDelegateHandle;
	///FOnUnregisterPlayersCompleteDelegate UnregisterPlayersCompleteDelegate;
	///FDelegateHandle UnregisterPlayersCompleteDelegateHandle;

	TSharedPtr<const FUniqueNetId> GoGetUniqueNetId(APlayerController* Player) const;

};
