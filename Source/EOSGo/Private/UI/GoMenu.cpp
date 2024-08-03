// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GoMenu.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Subsystem/GoSubsystem.h"

bool UGoMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (Host_Button)
	{
		Host_Button->OnClicked.AddDynamic(this, &UGoMenu::HostButtonClicked);
	}
	if (Join_Button)
	{
		Join_Button->OnClicked.AddDynamic(this, &UGoMenu::JoinButtonClicked);
	}
	return true;
}

void UGoMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UGoMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LevelPath)
{
	MapToTravel = LevelPath.Append(FString(TEXT("?Listen")));
	MatchType = TypeOfMatch;
	NumberOfConnections = NumberOfPublicConnections;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		GoSubsystem = GameInstance->GetSubsystem<UGoSubsystem>();
	}

	
	if (IsValid(GoSubsystem))
	{
		//~ Bind session callbacks.
		GoSubsystem->GoOnCreateSessionComplete.AddDynamic(this, &UGoMenu::OnCreateSession);
		GoSubsystem->GoOnFindSessionsComplete.AddUObject(this, &UGoMenu::OnFindSessions);
		GoSubsystem->GoOnJoinSessionComplete.AddUObject(this, &UGoMenu::OnJoinSession);
		GoSubsystem->GoOnDestroySessionComplete.AddDynamic(this, &UGoMenu::OnDestroySession);
		GoSubsystem->GoOnStartSessionComplete.AddDynamic(this, &UGoMenu::OnStartSession);

		//~ Login
		GoSubsystem->GoEOSLogin("","","accountportal");
	}
}

void UGoMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LogMessage(FColor::Green,FString(TEXT("Session created successfully")));
		if (UWorld* World = GetWorld()) World->ServerTravel(MapToTravel);
	}
	else
	{
		LogMessage(FColor::Red,FString(TEXT("Failed creating session")));
	}
}

void UGoMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!IsValid(GoSubsystem)) return;
	
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			GoSubsystem->GoJoinSession(Result);
			return;
		}
	}
}

void UGoMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}
}

void UGoMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UGoMenu::OnStartSession(bool bWasSuccessful)
{
}

void UGoMenu::HostButtonClicked()
{
	if (GoSubsystem) GoSubsystem->GoCreateSession(NumberOfConnections, MatchType);
}

void UGoMenu::JoinButtonClicked()
{
	if (GoSubsystem) GoSubsystem->GoFindSessions(100);
}

void UGoMenu::MenuTearDown()
{
	RemoveFromParent();
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			const FInputModeGameOnly InputData;
			PlayerController->SetInputMode(InputData);
		}
	}
}
