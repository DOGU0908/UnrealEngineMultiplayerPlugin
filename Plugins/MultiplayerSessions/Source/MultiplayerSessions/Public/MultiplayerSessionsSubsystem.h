// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/*
 * Declaring delegates for the MultiplayerMenu class to bind callbacks to
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerSessionsSubsystemOnCreateSessionCompleteDelegate, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerSessionsSubsystemOnFindSessionsCompleteDelegate, const TArray<FOnlineSessionSearchResult>& SessionSearchResult, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerSessionsSubsystemOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	// GameInstanceSubsystem is instantiated once per game instance and lasts as long as the game instance exists
	// unreal engine automatically handles the creation and lifecycle of the subsystem by inheriting from UGameInstanceSubsystem class
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/*
	 * handle session functionality
	 */
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionSearchResult);
	void DestroySession();

	/*
	 * Delegates for the MultiplayerMenu class to bind callbacks to
	 */
	FMultiplayerSessionsSubsystemOnCreateSessionCompleteDelegate MultiplayerSessionsSubsystemOnCreateSessionCompleteDelegate;
	FMultiplayerSessionsSubsystemOnFindSessionsCompleteDelegate MultiplayerSessionsSubsystemOnFindSessionsCompleteDelegate;
	FMultiplayerSessionsSubsystemOnJoinSessionCompleteDelegate MultiplayerSessionsSubsystemOnJoinSessionCompleteDelegate;
	
protected:
	/*
	 * callbacks for the delegates
	 */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearchSettings;

	bool bCreateSessionOnDestroy{false};
	int32 LastNumPublicConnections{4};
	FString LastMatchType{TEXT("FreeForAll")};
	
	/*
	 * delegates
	 */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
};
