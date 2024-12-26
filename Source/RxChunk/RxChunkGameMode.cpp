#include "RxChunkGameMode.h"

#include "OSCManager.h"
#include "OSCClient.h"
#include "OSCServer.h"

ARxChunkGameMode::ARxChunkGameMode() {
}

void ARxChunkGameMode::BeginPlay() {
  Super::BeginPlay();

  UE_LOG(LogTemp, Warning, TEXT("osc: go"));

  FString txip = TEXT("127.0.0.1");
  int txport = 7001;
  osctx = UOSCManager::CreateOSCClient(
    txip,
    txport,
    TEXT("osctx"),
    this
  );

  // FString rxip = TEXT("127.0.0.1");
  // FString rxip = TEXT("192.168.86.24");
  FString rxip = TEXT("0.0.0.0");
  int rxport = 7000;
  UE_LOG(LogTemp, Warning, TEXT("osc: server starting at %s:%d"), *rxip, rxport);
  oscrx = UOSCManager::CreateOSCServer(
    rxip,
    rxport,
    false, // loopback
    true, // start listening
    TEXT("oscrx"),
    this
  );
  oscrx->SetAllowlistClientsEnabled(false);

  oscrx->OnOscMessageReceived.AddDynamic(this, &ARxChunkGameMode::TestMessage);

  AddRoute(TEXT("/echo/*"), FName(TEXT("Echo")));
}

void ARxChunkGameMode::SendEcho() {
  FOSCAddress address = UOSCManager::ConvertStringToOSCAddress(FString("/echo"));
  FOSCMessage message;
  UOSCManager::SetOSCMessageAddress(message, address);
  UOSCManager::AddString(message, TEXT("HI AM UNREAL EH?"));

  osctx->SendOSCMessage(message);
}

void ARxChunkGameMode::AddRoute(const FString& AddressPattern, const FName& MethodName) {
  UE_LOG(LogTemp, Warning, TEXT("osc: adding route- %s"), *AddressPattern);
  FOSCDispatchMessageEventBP Event;
  Event.BindUFunction(this, MethodName);
  oscrx->BindEventToOnOSCAddressPatternMatchesPath(
    UOSCManager::ConvertStringToOSCAddress(AddressPattern),
    Event
  );
}

void ARxChunkGameMode::Echo(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  FString say;
  UOSCManager::GetString(message, 0, say);

  UE_LOG(LogTemp, Warning, TEXT("osc: echo- %s"), *say);
}

void ARxChunkGameMode::BlobTest(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  TArray<uint8> blob;
  UOSCManager::GetBlob(message, 0, blob);

  for (uint8 val : blob)
    UE_LOG(LogTemp, Warning, TEXT("osc: blob val- %d"), val);
}

void ARxChunkGameMode::TestMessage(const FOSCMessage& InMessage, const FString& InIPAddress, int32 InPort) {
  UE_LOG(LogTemp, Warning, TEXT("osc: test message path- %s"), *InMessage.GetAddress().GetFullPath());
}
