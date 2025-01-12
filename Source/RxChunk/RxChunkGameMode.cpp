#include "RxChunkGameMode.h"

#include "Faced.h"
#include "ChunkedTest.h"
#include "ChunkedTexture.h"

#include "OSCManager.h"
#include "OSCClient.h"
#include "OSCServer.h"

ARxChunkGameMode::ARxChunkGameMode() {
}

void ARxChunkGameMode::BeginPlay() {
  Super::BeginPlay();

  GEngine->Exec(nullptr, TEXT("log LogOSC off"));

  UE_LOG(LogTemp, Warning, TEXT("osc: go"));

  FString txip = TEXT("127.0.0.1");
  int txport = 7001;
  osctx = UOSCManager::CreateOSCClient(
    txip,
    txport,
    TEXT("osctx"),
    this
  );

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

  // oscrx->OnOscMessageReceived.AddDynamic(this, &ARxChunkGameMode::TestMessage);
  // SendEcho();

  AddRoute(TEXT("/echo/*"), FName(TEXT("Echo")));
  AddRoute(TEXT("/blobtest/*"), FName(TEXT("BlobTest")));
  AddRoute(TEXT("/chunked_test/*"), FName(TEXT("ChunkedTest")));
  AddRoute(TEXT("/chunked_texture/*"), FName(TEXT("ChunkedTexture")));

  FActorSpawnParameters spawnParams;
  spawnParams.Owner = this;

  DemoActor =
    GetWorld()->SpawnActor<AFaced>(
      AFaced::StaticClass(),
      FVector(0.f),
      FRotator(0.f, -90.f, 0.f),
      spawnParams
    );
  DemoActor->SetSize(100.f, 100.f);
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

void ARxChunkGameMode::AckChunk(
  const FOSCMessage &message,
  int32_t& chunkedId,
  int32_t& chunkNum
) {
  UOSCManager::GetInt32(message, 0, chunkedId);
  UOSCManager::GetInt32(message, 1, chunkNum);

  SendChunkAck(chunkedId, chunkNum);
}

void ARxChunkGameMode::ChunkedTest(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  int32_t id, chunkNum;
  AckChunk(message, id, chunkNum);

  int32_t numChunks, chunkSize;
  UOSCManager::GetInt32(message, 2, numChunks);
  UOSCManager::GetInt32(message, 3, chunkSize);

  TArray<uint8_t> blob;
  UOSCManager::GetBlob(message, 4, blob);

  if (!ChunkedSends.Contains(id)) {
    UChunkedTest* chunked = NewObject<UChunkedTest>(this);
    chunked->Init(id, chunkSize * numChunks, numChunks);
    chunked->OnComplete.AddLambda([](uint32_t id, TArray<uint8_t>& Data) {
      UE_LOG(LogTemp, Warning, TEXT("osc: chunked %d complete, data:"), id);
      for (uint8_t& byte : Data) {
        UE_LOG(LogTemp, Warning, TEXT("osc:   %d"), byte);
      }
    });
    ChunkedSends.Add(id, chunked);
  }

  ChunkedSends[id]->AddChunk(blob, chunkNum, chunkSize);
}

void ARxChunkGameMode::ChunkedTexture(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  int32_t id, chunkNum;
  AckChunk(message, id, chunkNum);

  if (Textures.Contains(id)) {
    DemoActor->SetTexture(Textures[id]);
    return;
  }

  int32_t numChunks, chunkSize;
  UOSCManager::GetInt32(message, 2, numChunks);
  UOSCManager::GetInt32(message, 3, chunkSize);

  TArray<uint8_t> blob;
  UOSCManager::GetBlob(message, 4, blob);

  int32_t width, height;
  UOSCManager::GetInt32(message, 5, width);
  UOSCManager::GetInt32(message, 6, height);

  if (!ChunkedSends.Contains(id)) {
    UChunkedTexture* chunked = NewObject<UChunkedTexture>(this);
    chunked->Init(id, chunkSize * numChunks, numChunks);
    chunked->SetDimensions(width, height);
    chunked->OnComplete.AddLambda([&](uint32_t id, UTexture2D* Texture) {
      UE_LOG(LogTemp, Warning, TEXT("osc: texture added for chunked %d"), id);
      Textures.Add(id, Texture);
      DemoActor->SetTexture(Texture);

      ChunkedSends[id]->ConditionalBeginDestroy();
      ChunkedSends[id] = nullptr;
    });
    ChunkedSends.Add(id, chunked);
  }

  ChunkedSends[id]->AddChunk(blob, chunkNum, chunkSize);
}

void ARxChunkGameMode::TestMessage(const FOSCMessage& InMessage, const FString& InIPAddress, int32 InPort) {
  UE_LOG(LogTemp, Warning, TEXT("osc: test message path- %s"), *InMessage.GetAddress().GetFullPath());
}

void ARxChunkGameMode::SendEcho() {
  FOSCAddress address = UOSCManager::ConvertStringToOSCAddress(FString("/echo"));
  FOSCMessage message;
  UOSCManager::SetOSCMessageAddress(message, address);
  UOSCManager::AddString(message, TEXT("HI AM UNREAL EH?"));

  osctx->SendOSCMessage(message);
}

void ARxChunkGameMode::SendChunkAck(int32_t chunkedId, int32_t chunkNum) {
  // fake a faulty send sometimes
  // if (FMath::RandBool()) return;

  FOSCAddress address = UOSCManager::ConvertStringToOSCAddress(FString("/ack_chunk"));
  FOSCMessage message;
  UOSCManager::SetOSCMessageAddress(message, address);
  UOSCManager::AddInt32(message, chunkedId);
  UOSCManager::AddInt32(message, chunkNum);

  // UE_LOG(LogTemp, Warning, TEXT("osc: acking chunked %d chunknum %d"), chunkedId, chunkNum);

  osctx->SendOSCMessage(message);
}