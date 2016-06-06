#include <BxScene.hpp>

class callitalk_group;
class callitalk_peer;

class callitalk_group
{
public:
	BxString::Parse Name;
	BxVar<callitalk_peer*> Peers;
public:
	callitalk_group() {}
	~callitalk_group() {}
};

class callitalk_peer
{
public:
	int PeerID;
	callitalk_group* MyGroup;
public:
	callitalk_peer() : PeerID(-1), MyGroup(nullptr) {}
	~callitalk_peer() {}
public:
	bool ExitFromRoom(id_server Serv)
	{
		if(!MyGroup) return false;
		BxVar<callitalk_peer*>& Peers = MyGroup->Peers;

		// 자신의 인덱스를 찾고 친구들에게 퇴장알림
		int FindIndex = -1;
		for(int i = 0, iend = Peers.Length(); i < iend; ++i)
		{
			if(Peers[i]->PeerID == PeerID)
			{
				FindIndex = i;
				continue;
			}
			static const uint Message[2] = {4, 'EXIT'};
			BxCore::Server::SendToPeer(Serv, Peers[i]->PeerID, Message, 8);
		}

		// 그룹내에서 자신을 지움
		if(FindIndex != -1)
		{
			Peers.Delete(FindIndex);
			// 멤버가 하나도 없으면 그룹도 제거
			if(Peers.Length() == 0)
				return true;
		}
		return false;
	}
};

class callitalk_server
{
public:
	id_server Serv;
	BxVarMap<callitalk_group> Group;
	BxVarMap<callitalk_peer> Peer;

	id_font InfoFont;
	int Connectors;
	int Rooms;

public:
	callitalk_server()
	{
		Serv = BxCore::Server::Create(true);
		const bool IsSuccess = BxCore::Server::Listen(Serv, 8240);
		BxASSERT("서버등록에 실패하였습니다", IsSuccess);

		if(!BxCore::Font::IsExistNick("image/cartoonstory.ttf"))
			BxCore::Font::NickOpen("image/cartoonstory.ttf", "image/cartoonstory.ttf");
		InfoFont = BxCore::Font::Open("image/cartoonstory.ttf", CT_SIZEBASE * 2 / 3);
		Connectors = 0;
		Rooms = 0;
	}
	~callitalk_server()
	{
		BxCore::Server::Release(Serv);
	}
};

FRAMEWORK_SCENE(callitalk_server, "callitalk_server")

syseventresult OnEvent(callitalk_server& This, const sysevent& Event)
{
	return syseventresult_pass;
}

sysupdateresult OnUpdate(callitalk_server& This)
{
	struct PacketStruct
	{
		uint Type;
		union
		{
			struct
			{
				char Group[1];
			} Join;
			struct
			{
				byte Buffer[1];
			} Data;
		};
	};

	sysupdateresult Result = sysupdateresult_no_render_fixed_frame;
	while(BxCore::Server::TryNextPacket(This.Serv))
	{
		const int PeerID = BxCore::Server::GetPacketPeerID(This.Serv);
		switch(BxCore::Server::GetPacketKind(This.Serv))
		{
		case peerpacketkind_entrance:
			if(!This.Peer.Access(PeerID))
			{
				This.Connectors++;
				Result = sysupdateresult_do_render_fixed_frame;
				This.Peer[PeerID].PeerID = PeerID;
                const uint Message[3] = {8, 'NTRY', (uint) PeerID};
				BxCore::Server::SendToPeer(This.Serv, PeerID, Message, 12);
			}
			break;
		case peerpacketkind_message:
			if(This.Peer.Access(PeerID))
			{
				huge PacketSize = 0;
				const PacketStruct* Packet = (PacketStruct*)
					BxCore::Server::GetPacketBuffer(This.Serv, &PacketSize);
				switch(Packet->Type)
				{
				case 'JOIN':
					{
						callitalk_peer* CurPeer = &This.Peer[PeerID];
						// 일단 기존 소속그룹에서 퇴장
						if(CurPeer->ExitFromRoom(This.Serv))
						{
							This.Rooms--;
							Result = sysupdateresult_do_render_fixed_frame;
							BxString::Parse GroupName = CurPeer->MyGroup->Name;
							This.Group.Remove(GroupName);
							CurPeer->MyGroup = nullptr;
						}
						// 가입하고자 하는 새 그룹을 찾고
						callitalk_group* CurGroup = This.Group.Access(Packet->Join.Group);
						// 만약 그룹이 존재하지 않는다면
						if(!CurGroup)
						{
							// 즉시 그룹을 생성
							This.Rooms++;
							Result = sysupdateresult_do_render_fixed_frame;
							CurGroup = &This.Group(Packet->Join.Group);
							CurGroup->Name = Packet->Join.Group;
						}
						// 이미 활동중인 새 그룹내 친구들에게 가입알림
						for(int i = 0, iend = CurGroup->Peers.Length(); i < iend; ++i)
						{
							static const uint Message[2] = {4, 'JOIN'};
							BxCore::Server::SendToPeer(This.Serv, CurGroup->Peers[i]->PeerID, Message, 8);
						}
						// 상호 정보기록
						CurGroup->Peers[LAST] = CurPeer;
						CurPeer->MyGroup = CurGroup;
					}
					break;
				case 'DATA':
					{
						callitalk_peer* CurPeer = &This.Peer[PeerID];
						// 소속그룹이 있다면
						if(CurPeer->MyGroup)
						{
							// 친구들에게 데이터를 전달
							BxVar<callitalk_peer*>& Peers = CurPeer->MyGroup->Peers;
							for(int i = 0, iend = Peers.Length(); i < iend; ++i)
							{
								if(Peers[i]->PeerID == PeerID) continue;
								BxCore::Server::SendToPeer(This.Serv, Peers[i]->PeerID,
									Packet->Data.Buffer, PacketSize - sizeof(uint));
							}
						}
					}
					break;
				}
			}
			break;
		case peerpacketkind_leaved:
		case peerpacketkind_kicked:
			if(This.Peer.Access(PeerID))
			{
				This.Connectors--;
				Result = sysupdateresult_do_render_fixed_frame;
				callitalk_peer* CurPeer = &This.Peer[PeerID];
				// 소속그룹에서 퇴장
				if(CurPeer->ExitFromRoom(This.Serv))
				{
					This.Rooms--;
					Result = sysupdateresult_do_render_fixed_frame;
					BxString::Parse GroupName = CurPeer->MyGroup->Name;
					This.Group.Remove(GroupName);
					CurPeer->MyGroup = nullptr;
				}
				// 자신을 제거
				This.Peer.Remove(PeerID);
			}
			break;
		}
	}
	return Result;
}

void OnRender(callitalk_server& This, BxDraw& Draw)
{
	Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(128, 128, 128));

	// Title
	BxTRY(Draw, CLIP(XYXY(10, 10, Draw.Width() - 10, 60)))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(160, 160, 160));
		Draw.TextByRect(This.InfoFont, BxString("Callitalk Server"),
			Draw.CurrentRect(), textalign_center_middle, COLOR(96, 0, 128));
	}

	// Connectors
	BxTRY(Draw, CLIP(XYXY(10, 70, Draw.Width() - 10, 120)))
	{
		Draw.TextByRect(This.InfoFont, BxString("Connectors"),
			Draw.CurrentRect(), textalign_left_middle, COLOR(0, 0, 0));
		BxTRY(Draw, CLIP(XYXY(Draw.Width() / 2, 0, Draw.Width(), Draw.Height())))
		{
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(96, 96, 96));
			Draw.TextByRect(This.InfoFont, BxString("<>:<A>", BxARG(This.Connectors)),
				Draw.CurrentRect(), textalign_center_middle, COLOR(255, 255, 255));
		}
	}

	// Rooms
	BxTRY(Draw, CLIP(XYXY(10, 130, Draw.Width() - 10, 180)))
	{
		Draw.TextByRect(This.InfoFont, BxString("Rooms"),
			Draw.CurrentRect(), textalign_left_middle, COLOR(0, 0, 0));
		BxTRY(Draw, CLIP(XYXY(Draw.Width() / 2, 0, Draw.Width(), Draw.Height())))
		{
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(96, 96, 96));
			Draw.TextByRect(This.InfoFont, BxString("<>:<A>", BxARG(This.Rooms)),
				Draw.CurrentRect(), textalign_center_middle, COLOR(255, 255, 255));
		}
	}

	// Logs
	BxTRY(Draw, CLIP(XYXY(10, 190, Draw.Width() - 10, Draw.Height() - 10)))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(96, 96, 96));
	}
}
