// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PewPew.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PEWPEW_API UShooterGameInstance : public UGameInstance
{
	
		GENERATED_BODY()

	public:
		// Constructor
		UShooterGameInstance(const FObjectInitializer& ObjectInitializer);

		/**
		*	Function to host a game!
		*
		*	@Param		UserID			User that started the request
		*	@Param		SessionName		Name of the Session
		*	@Param		bIsLAN			Is this is LAN Game?
		*	@Param		bIsPresence		"Is the Session to create a presence Session"
		*	@Param		MaxNumPlayers	Number of Maximum allowed players on this "Session" (Server)
		*/
		bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);

		/**
		*	Function fired when a session create request has completed
		*
		*	@param SessionName the name of the session this callback is for
		*	@param bWasSuccessful true if the async action completed without error, false if there was an error
		*/
		virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

		/**
		*	Function fired when a session start request has completed
		*
		*	@param SessionName the name of the session this callback is for
		*	@param bWasSuccessful true if the async action completed without error, false if there was an error
		*/
		void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

		// SESSION CREATION VARIABLES
	public:
		// Delegate called when session is created
		FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
		
		// Delegate called when session is started
		FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

		// Handles to registered delegates for creating/starting a session
		FDelegateHandle OnCreateSessionCompleteDelegateHandle;
		FDelegateHandle OnStartSessionCompleteDelegateHandle;

		// this will contain the session settings
		TSharedPtr<class FOnlineSessionSettings> SessionSettings;

		// SESSION FINDING FUNCTIONS
	public:
		/**
		*	Find an online session
		*
		*	@param UserId user that initiated the request
		*	@param SessionName name of session this search will generate
		*	@param bIsLAN are we searching LAN matches
		*	@param bIsPresence are we searching presence sessions
		*/
		void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence);

		/**
		*	Delegate fired when a session search query has completed
		*
		*	@param bWasSuccessful true if the async action completed without error, false if there was an error
		*/
		void OnFindSessionsComplete(bool bWasSuccessful);

		// SESSION FINDING VARIABLES
	public:
		// delegate for searching a session
		FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

		// handle to registered delegate for searching a session
		FDelegateHandle OnFindSessionsCompleteDelegateHandle;

		// this will contain the search session settings, as well as the search results once the search is complete
		TSharedPtr<class FOnlineSessionSearch> SessionSearch;

		// SESSION JOINING FUNCTIONS
	public:
		/**
		*	Joins a session via a search result
		*
		*	@param SessionName name of session
		*	@param SearchResult Session to join
		*
		*	@return bool true if successful, false otherwise
		*/
		bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

		/**
		*	Delegate fired when a session join request has completed
		*
		*	@param SessionName the name of the session this callback is for
		*	@param bWasSuccessful true if the async action completed without error, false if there was an error
		*/
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

		// SESSION JOINING VARIABLES
	public:
		// Delegate for joining a session
		FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	//	// Handle to registered delegate for joining a session
		FDelegateHandle OnJoinSessionCompleteDelegateHandle;

		// SESSION DESTROYING FUNCTIONS
	public:
		/**
		*	Delegate fired when a destroying an online session has completed
		*
		*	@param SessionName the name of the session this callback is for
		*	@param bWasSuccessful true if the async action completed without error, false if there was an error
		*/
		virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

		// SESSION DESTROYING VARIABLES
	public:
		// Delegate for destroying a session
		FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

		// Handle to registered delegate for destroying a session
		FDelegateHandle OnDestroySessionCompleteDelegateHandle;

		// TESTING
	public:
		UFUNCTION(BlueprintCallable, Category = "Network|Test")
			void StartOnlineGame();//create session, starts up the server

		UFUNCTION(BlueprintCallable, Category = "Network|Test")
			void FindOnlineGames();//looks for session based on some parameters

		UFUNCTION(BlueprintCallable, Category = "Network|Test")
			void JoinOnlineGame();//join the found session or a connection IP

		UFUNCTION(BlueprintCallable, Category = "Network|Test")
			void DestroySessionAndLeaveGame();

		FName MPSessionName = FName("TestMPSession!");//name of the session/server
	};

