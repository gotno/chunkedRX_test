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

  // oscrx->OnOscMessageReceived.AddDynamic(this, &ARxChunkGameMode::TestMessageReceipt);
  // SendEcho();

  AddRoute(TEXT("/echo/*"), FName(TEXT("rxEcho")));
  AddRoute(TEXT("/blobtest/*"), FName(TEXT("rxBlobTest")));
  AddRoute(TEXT("/chunked_test/*"), FName(TEXT("rxChunkedTest")));
  AddRoute(TEXT("/chunked_texture/*"), FName(TEXT("rxChunkedTexture")));
  AddRoute(TEXT("/module_info/*"), FName(TEXT("rxModuleInfo")));

  FActorSpawnParameters spawnParams;
  spawnParams.Owner = this;

  DemoActor =
    GetWorld()->SpawnActor<AFaced>(
      AFaced::StaticClass(),
      FVector(0.f),
      FRotator(0.f, -90.f, 0.f),
      spawnParams
    );
  DemoActor->SetSize(5.f, 5.f);
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

void ARxChunkGameMode::rxEcho(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  FString say;
  UOSCManager::GetString(message, 0, say);

  UE_LOG(LogTemp, Warning, TEXT("osc: echo- %s"), *say);
}

void ARxChunkGameMode::rxBlobTest(
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

  txChunkAck(chunkedId, chunkNum);
}

void ARxChunkGameMode::rxChunkedTest(
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

  TArray<uint8> blob;
  UOSCManager::GetBlob(message, 4, blob);

  if (!ChunkedSends.Contains(id)) {
    UChunkedTest* chunked = NewObject<UChunkedTest>(this);
    chunked->Init(id, chunkSize * numChunks, numChunks, chunkSize);
    chunked->OnComplete.AddLambda([](uint32_t id, TArray<uint8_t>& Data) {
      UE_LOG(LogTemp, Warning, TEXT("osc: chunked %d complete, data:"), id);
      for (uint8_t& byte : Data) {
        UE_LOG(LogTemp, Warning, TEXT("osc:   %d"), byte);
      }
    });
    ChunkedSends.Add(id, chunked);
  }

  ChunkedSends[id]->AddChunk(blob, chunkNum);
}

void ARxChunkGameMode::rxChunkedTexture(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  int32_t id, chunkNum;
  AckChunk(message, id, chunkNum);

  if (Textures.Contains(id)) {
    DemoActor->SetBaseTexture(Textures[id]);
    return;
  }

  int32_t numChunks, chunkSize;
  UOSCManager::GetInt32(message, 2, numChunks);
  UOSCManager::GetInt32(message, 3, chunkSize);

  int64_t totalSize;
  UOSCManager::GetInt64(message, 4, totalSize);

  TArray<uint8_t> blob;
  UOSCManager::GetBlob(message, 5, blob);

  int32_t width, height;
  UOSCManager::GetInt32(message, 6, width);
  UOSCManager::GetInt32(message, 7, height);

  bool bCompressed;
  UOSCManager::GetBool(message, 8, bCompressed);

  bool bOverlay;
  UOSCManager::GetBool(message, 9, bOverlay);

  if (!ChunkedSends.Contains(id)) {
    UChunkedTexture* chunked = NewObject<UChunkedTexture>(this);
    chunked->Init(id, totalSize, numChunks, chunkSize);
    chunked->SetDimensions(width, height);
    chunked->SetCompressed(bCompressed);
    chunked->OnComplete.AddLambda([&](UChunkedTexture* ChunkedTexture) {
      if (bOverlay) {
        DemoActor->SetOverlayData(ChunkedTexture);
      } else {
        Textures.Add(ChunkedTexture->id, ChunkedTexture->MakeTexture2D());
        DemoActor->SetBaseTexture(Textures[ChunkedTexture->id]);
      }

      ChunkedSends[id]->ConditionalBeginDestroy();
      ChunkedSends[id] = nullptr;
      ChunkedSends.Remove(id);
    });
    ChunkedSends.Add(id, chunked);
  } else if (ChunkedSends[id]) {
    ChunkedSends[id]->AddChunk(blob, chunkNum);
  }
}

void ARxChunkGameMode::rxModuleInfo(
  const FOSCAddress& AddressPattern,
  const FOSCMessage &message,
  const FString &ipaddress,
  int32 port
) {
  int64_t moduleId;
  UOSCManager::GetInt64(message, 0, moduleId);

  float width, height;
  UOSCManager::GetFloat(message, 1, width);
  UOSCManager::GetFloat(message, 2, height);

  UE_LOG(LogTemp, Warning, TEXT("%f/%f"), width, height);
  DemoActor->SetSize(width, height);
}

void ARxChunkGameMode::TestMessageReceipt(const FOSCMessage& InMessage, const FString& InIPAddress, int32 InPort) {
  UE_LOG(LogTemp, Warning, TEXT("osc: test message path- %s"), *InMessage.GetAddress().GetFullPath());
}

void ARxChunkGameMode::txEcho() {
  FOSCAddress address = UOSCManager::ConvertStringToOSCAddress(FString("/echo"));
  FOSCMessage message;
  UOSCManager::SetOSCMessageAddress(message, address);
  UOSCManager::AddString(message, TEXT("HI AM UNREAL EH?"));

  osctx->SendOSCMessage(message);
}

void ARxChunkGameMode::txChunkAck(int32_t chunkedId, int32_t chunkNum) {
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