// Copyright (c) 2024 Fedahumada Studio. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GoGameModeBase.generated.h"

//~ GO GAME MODE DELEGATES
							//For GoOverlay class to bind callbacks to.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoOnRegisterPlayerComplete, FName, Username);

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
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditDefaultsOnly, Category="EOS-Go")
	bool bDisplayScreenMessage = true;
	
	//~ To handle Registered Players functionality.
	void GoRegisterPlayer(APlayerController* NewPlayer);
	UPROPERTY(BlueprintAssignable, Category="EOS-Go|Account")
	FGoOnRegisterPlayerComplete GoOnRegisterPlayerComplete;
	

protected:
	//~ To handle Registered Players functionality.
	void OnRegisteredPlayerComplete(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccess);
	
private:
	IOnlineSessionPtr SessionInterface;
	
	FName RegisteredPlayer{FName()};

	/*//~ Delegates to add to the Online Session Interface delegate list. Each one has its own handle.
	FOnRegisterPlayersCompleteDelegate RegisterPlayersCompleteDelegate;
	FDelegateHandle OnRegisterPlayersCompleteDelegateHandle;*/

};
