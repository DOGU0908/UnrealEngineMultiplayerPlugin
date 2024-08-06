// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenu.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"

void UMultiplayerMenu::MenuSetup(const int32 NumberOfPublicConnections, const FString TypeOfMatch, const FString PathToLobby)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	LobbyPath = FString::Printf(TEXT("%s?listen"), *PathToLobby);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if (PlayerController)
		{
			FInputModeUIOnly InputModeUIOnly;
			InputModeUIOnly.SetWidgetToFocus(TakeWidget());
			InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeUIOnly);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerSessionsSubsystemOnCreateSessionCompleteDelegate.AddDynamic(this, &UMultiplayerMenu::OnCreateSession);
			MultiplayerSessionsSubsystem->MultiplayerSessionsSubsystemOnFindSessionsCompleteDelegate.AddUObject(this, &UMultiplayerMenu::OnFindSessions);
			MultiplayerSessionsSubsystem->MultiplayerSessionsSubsystemOnJoinSessionCompleteDelegate.AddUObject(this, &UMultiplayerMenu::OnJoinSession);
		}
	}
}

bool UMultiplayerMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMultiplayerMenu::OnHostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMultiplayerMenu::OnJoinButtonClicked);
	}
	
	return true;
}

void UMultiplayerMenu::NativeDestruct()
{
	RemoveFromParent();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if (PlayerController)
		{
			FInputModeGameOnly InputModeGameOnly;
			PlayerController->SetInputMode(InputModeGameOnly);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	
	Super::NativeDestruct();
}

void UMultiplayerMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			World->ServerTravel(LobbyPath);
		}
	}
	else
	{
		HostButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionSearchResults,
	bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}
	
	for (auto Result : SessionSearchResults)
	{
		FString MatchTypeSettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchTypeSettingsValue);

		if (MatchTypeSettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			
			return;
		}
	}

	if (!bWasSuccessful || SessionSearchResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();

			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnHostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMultiplayerMenu::OnJoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(9999);
	}
}
