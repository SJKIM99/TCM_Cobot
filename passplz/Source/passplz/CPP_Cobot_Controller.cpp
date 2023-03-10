// Fill out your copyright notice in the Description page of Project Settings.

#include "../../../server/server/protocol.h"

#include "CPP_Cobot_Controller.h"
#include "CPP_Cobot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ACPP_Cobot_Controller::ACPP_Cobot_Controller()
    : x(0.f)
    , y(0.f)
    , z(0.f)
{
    UE_LOG(LogTemp, Warning, TEXT("Start connect server!"));
    //int retval;

    // 윈속 초기화

    WSADATA wsaData;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);   // Winsock 초기화
    //if (nRet != 0) return false;

    //// 소켓 생성
    sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    //if (Socket == INVALID_SOCKET) return false;

    // IP, Port 정보 입력
    SOCKADDR_IN stServerAddr;
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port = htons(7000);
    stServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 접속
    nRet = connect(sock, (sockaddr*)&stServerAddr, sizeof(sockaddr));
    //if (nRet == SOCKET_ERROR) return false;

    UE_LOG(LogTemp, Warning, TEXT("Success server connect"));
}

void ACPP_Cobot_Controller::RecvPacket()
{
    char buff[BUF_SIZE];

    int ret = recv(sock, reinterpret_cast<char*>(&buff), BUF_SIZE, 0);
    if (ret <= 0)
    {
        GetLastError();
        std::cout << "recv() fail!" << std::endl;
    }

    ring_buff.enqueue(buff, ret);

    int read_point = 0;
    while (buff[read_point] <= ring_buff.diff() && !ring_buff.empty())
    {
        ProcessPacket(buff);
        ring_buff.move_read_pos(buff[0]);
        read_point = buff[read_point];
    }
}

void ACPP_Cobot_Controller::ProcessPacket(char* packet)
{
    switch (packet[1])
    {
    case static_cast<int>(type::sc_move):
    {
        sc_move_packet* pack = reinterpret_cast<sc_move_packet*>(packet);

        UE_LOG(LogTemp, Warning, TEXT("recv packet success!"));
    } break;
    }
}

void ACPP_Cobot_Controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("코봇생성"));

    //서버! 상대 플레이어를 생성한다. 
    //서버에서 상대 플레이어의 초기 위치를 받아주세요
    Player_2 = GetWorld()->SpawnActor<ACPP_Cobot>(ACPP_Cobot::StaticClass(), FVector(0.0f, 0.0f, 150.0f/*여기에 초기 위치 넣어주면 됨*/), FRotator(0.0f, 0.0f, 0.0f));
   
}

void ACPP_Cobot_Controller::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void ACPP_Cobot_Controller::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    UE_LOG(LogTemp, Warning, TEXT("OnPossess"));
}

void ACPP_Cobot_Controller::SetupInputComponent()
{
    Super::SetupInputComponent();

    // W, S 키 눌리면 Move_Forward() 함수를 실행
    InputComponent->BindAxis(TEXT("MOVE_FORWARD"), this, &ACPP_Cobot_Controller::Move_Forward);
    InputComponent->BindAxis(TEXT("ROTATE"), this, &ACPP_Cobot_Controller::Rotate);
}

void ACPP_Cobot_Controller::Move_Forward(float NewAxisValue)
{

    APawn* player = GetPawn();

    if (!player)
        return;

    FRotator rotator_controller = GetControlRotation();
    FRotator rotator_forward = UKismetMathLibrary::MakeRotator(0.0f, 0.0f, rotator_controller.Yaw);
    FVector forward_vector = UKismetMathLibrary::GetForwardVector(rotator_forward);

    player->AddMovementInput(forward_vector, NewAxisValue);

    // 서버로 움직임 보냄
 // 매프레임 여기를 지나감
 // 따라서 움직임이 바뀌었을 때만 보내도록 함
 /*
    여기는 일단 내가 움직이는 거 위치 받아오려고 해둔 곳
    추후에 어디가 움직이는 곳인지 보고 고쳐야 함
 */
    if (x != player->GetActorLocation().X || y != player->GetActorLocation().Y || z != player->GetActorLocation().Z)
    {
        x = player->GetActorLocation().X;
        y = player->GetActorLocation().Y;
        z = player->GetActorLocation().Z;

        cs_move_packet pack;
        pack.size = sizeof(pack);
        pack.type = static_cast<char>(type::cs_move);
        pack.x = x;
        pack.y = y;
        pack.z = z;

        int ret = send(sock, reinterpret_cast<char*>(&pack), sizeof(pack), 0);

        // 이게 패킷 받는거
        // 패킷 받을 곳에 복붙해서 놓으면 됨
        RecvPacket();
    }

}

void ACPP_Cobot_Controller::Rotate(float NewAxisValue)
{
    float delta_time = GetWorld()->GetDeltaSeconds();
    AddYawInput(delta_time * NewAxisValue * 20.0f);
}

bool ACPP_Cobot_Controller::Is_Set_IDPW(FString I, FString p)
{
    /*서버! 여기에 서버에 send해줘야함 false라면 바로 리턴 true면 아래 작업 후 리턴*/
    ID = I;
    Passward = p; 
    return true; 
}
