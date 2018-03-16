// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGameInstance.h"

UShooterGameInstance::UShooterGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { // this line is to call the superclass' constructor passing the parameter we want. It is done in this way only for constructors

	//instantiate the delegate class and you pass it a listener																										  
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnCreateSessionComplete);//(Bind = listener) you instantiate the delegate. session creation delegate bindings
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnStartOnlineGameComplete);

	// session finding delegate bindings
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnFindSessionsComplete);

	// session joining delegate bindings
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnJoinSessionComplete);

	// session destroying delegate bindings
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnDestroySessionComplete);
}

bool UShooterGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers) {
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Creating Session"));
	// get the OnlineSubsystem to work with
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();//You grab the subsystem you added to the files(default engine.ini and the .cs)
	if (!OnlineSubsystem) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: No OnlineSubsystem found!"));
		return false;
	}

	// get the session interface, so we can call the CreateSession function on it
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();//grab the session manager of the subsystem we use
	if (!Sessions.IsValid() || !UserId.IsValid()) { return false; }//isvalid = means its not a nullpointer

																   // fill in the session settings we want to use. there are more with SessionSettings->Set(...), for example map or gamemode/type
	SessionSettings = MakeShareable(new FOnlineSessionSettings());//it is a struct that contains the settings below
	SessionSettings->bIsLANMatch = bIsLan;
	SessionSettings->bUsesPresence = bIsPresence;
	SessionSettings->NumPublicConnections = MaxNumPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	//SessionSettings->Set(SETTING_MAPNAME, FString("ThirdPersonExampleMap"), EOnlineDataAdvertisementType::ViaOnlineService);

	// set the delegate to the handle of the sessioninterface
	//session class has the method AddOnCreateSessionCompleteDelegate_Handle and you pass it the delegate 
	OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);//need the handle so you can deregister after

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Calling session creation"));
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Calling session creation"));
	// our delegate should get called when this is complete (doesn't need to be successful)
	//create session makes the broadcast and returns a bool(if true the session was created successfully)
	//*SessionSettings : in this way you pass the object when you have the pointer(makes an object out of a pointer)
	return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
}

//deregister from previous handle and take the handle of the new one
void UShooterGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Session created. Name: %s, Successful flag: %d"), *SessionName.ToString(), bWasSuccessful);

	// Get the OnlineSubsystem so we can get the session interface
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) { return; }

	// Get the session interface to call the StartSession function
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid()) { return; }

	// clear the SessionComplete delegate handle, since we finished this call
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);// clear the handle
	if (bWasSuccessful) {
		// set the StartSession delegate handle
		OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);//will call method OnStartOnlineGameComplete

																																  // our StartSessionComplete delegate should get called after this
		UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Calling StartSession"));
		Sessions->StartSession(SessionName);
	}
}

//deregister from the delegate above: OnStartSessionCompleteDelegate
void UShooterGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Session started. Name: %s, Successful flag: %d"), *SessionName.ToString(), bWasSuccessful);

	// Get the OnlineSubsystem so we can get the session interface
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();//Get the online subsystem for a given service
	if (!OnlineSubsystem) { return; }

	// Get the session interface to clear the delegate
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid()) { return; }

	// clear the delegate since we are done with this call
	Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

	if (bWasSuccessful) {
		UGameplayStatics::OpenLevel(GetWorld(), "Level1", true, "listen");//open level and set it to listen server. Other clients can connect to the pc that is set to listen. The server is client and listen
	}
}




void UShooterGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence) {
	// get the OnlineSubsystem to work with
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();//subsystem manages the playing online
	if (!OnlineSubsystem) {
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);//did not find the OnlineSubsystem
		return;
	}

	// get the SessionInterface from the OnlineSubsystem
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();//manage the session
	if (!Sessions.IsValid() || !UserId.IsValid()) {
		// If something goes wrong, just call the Delegate Function directly with "false"
		OnFindSessionsComplete(false);//did not find the Sessions. This is the method that the delegate would call
		return;
	}

	// fill in the search settings, like if we are searching for lan games or online etc, and how many results we want
	SessionSearch = MakeShareable(new FOnlineSessionSearch());//instantiate FOnlineSessionSearch(struct)
	SessionSearch->bIsLanQuery = bIsLan;//these are like filters
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->PingBucketSize = 50;

	// we only want to set this query setting if bIsPresence is true
	if (bIsPresence) {
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

	// set the delegate to the delegate handle of the FindSession function
	OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	// finally call the SessionInterface function. the delegate gets called once this is finished
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Calling FindSessions"));
	Sessions->FindSessions(*UserId, SearchSettingsRef);//LOOK for sessions. broadcast OnFindSessionsCompleteDelegate
}

void UShooterGameInstance::OnFindSessionsComplete(bool bWasSuccessful) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful));
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: FindSessions called. Successful flag: %d"), bWasSuccessful);

	// get the OnlineSubsystem to work with
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) { return; }

	// get the SessionInterface from the OnlineSubsystem
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid()) { return; }

	// clear the delegate handle, since we finished this call
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	// Just debugging the Number of Search results. Can be displayed in UMG or something later on
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));
	UE_LOG(LogTemp, Warning, TEXT("ONLINE SUBSYSTEMS: Found %d results."), SessionSearch->SearchResults.Num());

	// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
	if (SessionSearch->SearchResults.Num() > 0) {
		// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
		// This can be customized later on with your own classes to add more information that can be set and displayed
		for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++) {
			// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
			// This is something you can't do in Blueprint for example!
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));

		}
	}
}


bool UShooterGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubystem = IOnlineSubsystem::Get();

	if (OnlineSubystem)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSubystem->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}
	return bSuccessful;
}

void UShooterGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	if (Result != EOnJoinSessionCompleteResult::Success) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Join Failed"));
		return;
	}

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubystem = IOnlineSubsystem::Get();
	if (OnlineSubystem)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSubystem->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// string for us by giving him the SessionName and an empty String. We want to do this, because
			// every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}



void UShooterGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubystems = IOnlineSubsystem::Get();
	if (OnlineSubystems)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSubystems->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (could be a MainMenu!)
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "E1M1", true);
			}
		}
	}
}


void UShooterGameInstance::StartOnlineGame()
{

	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession(Player->GetPreferredUniqueNetId(), MPSessionName, true, false, 4);
}

void UShooterGameInstance::FindOnlineGames()//called by the UI button: Find Session
{//this method is for the clients

	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId(), MPSessionName, true, false);//you find session by name
}

void UShooterGameInstance::JoinOnlineGame()
{

	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];
				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSession(Player->GetPreferredUniqueNetId(), MPSessionName, SearchResult);
				break;
			}
		}
	}
}

void UShooterGameInstance::DestroySessionAndLeaveGame()
{

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(MPSessionName);
		}
	}
}