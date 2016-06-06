#include <BxScene.hpp>
#include <BxImage.hpp>
#include "adapter.hpp"
#include "command.hpp"
#include "input.hpp"
#include "export.hpp"

class callitalk_room
{
public:
	float ViewOx;
	float ViewOy;
	float UnitSize;
	BxImage ChatImage[2];
	const int ChatWidth;
	uhuge LastDrawTime;
	float LastDrawPos;
	int LastDrawAni;
	BxImage BtnImage[5];
	enum {BTN_EXIT, BTN_UNDO, BTN_DONE, BTN_OPEN, BTN_CLOSE};

	bool IsChatOpened;
	int ChatDoc;
	class TalkData
	{
	public:
		int Doc;
		int Peer;
		TalkData()
		{
			Doc = -1;
			Peer = -1;
		}
		~TalkData() {}
	};
	BxVarVector<TalkData, 1024> Talks;
	float ScrollPos;
	float ScrollTarget;

	id_socket Sock;
	int PeerID;

public:
    callitalk_room() : ChatWidth(CT_SIZEBASE * 100)
	{
		ViewOx = 0;
		ViewOy = 0;
		UnitSize = 0;
		ChatImage[0].Load("image/chat0.png", BxImage::PNG);
		ChatImage[1].Load("image/chat1.png", BxImage::PNG);
		LastDrawTime = 0;
		LastDrawPos = 0;
		LastDrawAni = 0;
		BtnImage[BTN_EXIT].Load("image/exit.png", BxImage::PNG);
		BtnImage[BTN_UNDO].Load("image/undo.png", BxImage::PNG);
		BtnImage[BTN_DONE].Load("image/done.png", BxImage::PNG);
		BtnImage[BTN_OPEN].Load("image/open.png", BxImage::PNG);
		BtnImage[BTN_CLOSE].Load("image/close.png", BxImage::PNG);

		IsChatOpened = false;
		ChatDoc = -1;
		ScrollPos = 0;
		ScrollTarget = 0;

		Sock = nullptr;
		PeerID = -1;
	}
	~callitalk_room()
	{
		Command::Canvas::Remove(ChatDoc);
		for(int i = 0; i < Talks.Length(); ++i)
			Command::Canvas::Remove(Talks[i].Doc);

		BxCore::Socket::Release(Sock);
	}
	void FlushScroll(bool reset)
	{
		Zoom DocZoom = Command::Canvas::GetZoom(ChatDoc);
		if(reset) DocZoom.offset.x = -ViewOx;
		else DocZoom.offset.x -= LastDrawPos / DocZoom.scale;
		Command::Canvas::SetZoom(ChatDoc, DocZoom);
		Command::Canvas::Update(ChatDoc);
		LastDrawAni = 0;
		LastDrawPos = 0;
	}
	void AddTalk(int peer)
	{
        static const float GapX = CT_SIZEBASE * 20 / 50;
        static const float GapY = CT_SIZEBASE * 10 / 50;
        static const float SizeH = CT_SIZEBASE * 20 / 50;
        static const float ChatH = CT_SIZEBASE * (50 + 3) / 50;
		const float Rate = UnitSize / 2 / SizeH;
		const int Count = Talks.Length();
		Talks.Insert(LAST);
		Talks[END].Doc = Command::Canvas::Create(false);
		Talks[END].Peer = peer;
		Command::Canvas::SetArea(Talks[END].Doc, Rect(0, -UnitSize / 2, UnitSize * Rate, UnitSize / 2));
		const float PosY = SizeH - ViewOy + GapY + ChatH * Count;
		if(PeerID == peer)
			Command::Canvas::SetZoom(Talks[END].Doc, Zoom(Point((ViewOx - UnitSize - GapX) * Rate, PosY * Rate), 1 / Rate));
		else Command::Canvas::SetZoom(Talks[END].Doc, Zoom(Point((GapX - ViewOx) * Rate, PosY * Rate), 1 / Rate));
		ScrollTarget = Math::MaxF(0, PosY + SizeH + GapY - ViewOy);

		if(PeerID == peer)
		{
			const int PacketSize = 4 + 4 + 8;
			const uint PacketType = 'DATA';
			BxCore::Socket::Send(Sock, &PacketSize, 4);
			BxCore::Socket::Send(Sock, &PacketType, 4);
			const int SubPacketSize = 8;
			const uint SubPacketType = 'ADDT';
			BxCore::Socket::Send(Sock, &SubPacketSize, 4);
			BxCore::Socket::Send(Sock, &SubPacketType, 4);
			BxCore::Socket::Send(Sock, &PeerID, 4);
		}
	}
	void SendTalk()
	{
		linkstep1 doclink = Adapter::GetDocumentLink(ChatDoc);
		linkstep2 layerlink = Adapter::GetLayerLink(doclink, 0);
		const int shapecount = Adapter::GetShapeCount(layerlink, false);
		linkstep3 shapelink = Adapter::GetShapeLink(layerlink, false, shapecount - 1);

		const byte* Buffer = Adapter::GetShapeBuffer(shapelink);
		const int BufferSize = Adapter::GetShapeBufferSize(shapelink);
		for(int i = Talks.Length() - 1; 0 <= i; --i)
		{
			if(Talks[i].Peer == PeerID)
			{
				Export::LoadShape(Talks[i].Doc, 0, Buffer, BufferSize);
				break;
			}
		}

		if(0 <= PeerID)
		{
			const int PacketSize = 4 + 4 + 8 + BufferSize;
			const uint PacketType = 'DATA';
			BxCore::Socket::Send(Sock, &PacketSize, 4);
			BxCore::Socket::Send(Sock, &PacketType, 4);
			const int SubPacketSize = 8 + BufferSize;
			const uint SubPacketType = 'TALK';
			BxCore::Socket::Send(Sock, &SubPacketSize, 4);
			BxCore::Socket::Send(Sock, &SubPacketType, 4);
			BxCore::Socket::Send(Sock, &PeerID, 4);
			BxCore::Socket::Send(Sock, Buffer, BufferSize);
		}
	}
};

FRAMEWORK_SCENE(callitalk_room, "callitalk_room")

syseventresult OnEvent(callitalk_room& This, const sysevent& Event)
{
	if(Event.type == syseventtype_init || Event.type == syseventtype_resize)
	{
		This.ViewOx = Event.init.w / 2.0f;
        This.ViewOy = (Event.init.h - CT_SIZEBASE * 2) / 2.0f;
		Command::SetViewRadius(Math::Sqrt(This.ViewOx * This.ViewOx + This.ViewOy * This.ViewOy));

		if(Event.type == syseventtype_init)
		{
			This.UnitSize = Math::MinF(This.ViewOx, This.ViewOy) * 4 / 3;
            Command::SetThick(CT_SIZEBASE * 15 / 50);
            Input::SetDeviceSpeedyForceOn(DeviceByTouch, CT_SIZEBASE * 10);
			Command::SetStrokeBy("aqua");
			Command::SetColor(Color(0, 0, 0, 180, 160));

			This.ChatDoc = Command::Canvas::Create(true);
			Command::Canvas::SetArea(This.ChatDoc, Rect(0, -This.UnitSize / 2, This.ChatWidth, This.UnitSize / 2));
			Command::Canvas::SetZoom(This.ChatDoc, Zoom(Point(-This.ViewOx, This.UnitSize / 2 - This.ViewOy), 1));
			Command::Canvas::SetFocus(This.ChatDoc);

			if(Event.init.message)
			{
				This.Sock = BxCore::Socket::Create();
				string Domain = BxCore::System::GetConfigString("CalliTalk.Domain", "localhost");
				if(BxCore::Socket::Connect(This.Sock, Domain, 8240, 3000) == connectstate_connected)
				{
					const int StringSize = StrLen(Event.init.message) + 1;
					const int PacketSize = 4 + StringSize;
					const uint PacketType = 'JOIN';
					BxCore::Socket::Send(This.Sock, &PacketSize, 4);
					BxCore::Socket::Send(This.Sock, &PacketType, 4);
					BxCore::Socket::Send(This.Sock, Event.init.message, StringSize);
				}
			}
		}
	}
	else if(Event.type == syseventtype_button)
	{
		if(Event.button.type == sysbuttontype_up)
		{
			if(StrCmp(Event.button.name, "BTN_EXIT") == same)
			{
				BxScene::SetFlow(sceneflow_decel);
				BxScene::AddRequest("callitalk_door", sceneside_left);
				BxScene::SubRequest("callitalk_room", sceneside_right);
			}
			else if(StrCmp(Event.button.name, "BTN_UNDO") == same)
			{
			}
			else if(StrCmp(Event.button.name, "BTN_DONE") == same)
			{
				Command::ClearShapes(This.ChatDoc);
				This.FlushScroll(true);
			}
			else if(StrCmp(Event.button.name, "BTN_OPEN") == same)
			{
				This.IsChatOpened = true;
			}
			else if(StrCmp(Event.button.name, "BTN_CLOSE") == same)
			{
				This.IsChatOpened = false;
			}
		}
	}
	else if(Event.type == syseventtype_touch && Event.touch.id == 0 && This.IsChatOpened && 0 <= This.PeerID)
	{
		const float X = Event.touch.fx - This.ViewOx;
        const float Y = Event.touch.fy - This.ViewOy - CT_SIZEBASE * 2;
		Command::SetCurrentTime(BxCore::System::GetTimeMilliSecond());
		switch(Event.touch.type)
		{
		case systouchtype_down:
			if(Command::GetShapeCount(This.ChatDoc) == 0)
				This.AddTalk(This.PeerID);
			if(0 < This.LastDrawAni)
				This.FlushScroll(false);
			Input::ScreenDown(Event.touch.id, X, Y, Event.touch.force, DeviceByTouch);
			break;
		case systouchtype_move:
			Input::ScreenDrag(Event.touch.id, X, Y, Event.touch.force, DeviceByTouch);
			break;
		case systouchtype_up: default:
			Input::ScreenUp(Event.touch.id, DeviceByTouch);
			This.SendTalk();
			if(0 < X)
			{
				This.LastDrawTime = BxCore::System::GetTimeMilliSecond();
				This.LastDrawPos = 0;
				This.LastDrawAni = 1024;
			}
			break;
		}
		return syseventresult_done;
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(callitalk_room& This)
{
	if(0 < This.LastDrawAni && This.LastDrawTime + 500 < BxCore::System::GetTimeMilliSecond())
	{
		This.LastDrawAni = This.LastDrawAni * 9 / 10;
		This.LastDrawPos = This.ViewOx * (1024 - This.LastDrawAni) / 1024;
		if(This.LastDrawAni == 0)
			This.FlushScroll(false);
	}

	if(0.1f < Math::AbsF(This.ScrollPos - This.ScrollTarget))
		This.ScrollPos = This.ScrollPos * 0.9f + This.ScrollTarget * 0.1f;

	if(4 <= BxCore::Socket::GetRecvLength(This.Sock))
	{
		int PacketSize = 0;
		BxCore::Socket::Recv(This.Sock, &PacketSize, 4);

		uint PacketType = 0;
		BxCore::Socket::Recv(This.Sock, &PacketType, 4);

		if(PacketType == 'NTRY')
			BxCore::Socket::RecvFully(This.Sock, (byte*) &This.PeerID, 4, 3000, false);
		else if(PacketType == 'ADDT')
		{
			int SenderPeerID;
			BxCore::Socket::RecvFully(This.Sock, (byte*) &SenderPeerID, 4, 3000, false);
			This.AddTalk(SenderPeerID);
		}
		else if(PacketType == 'TALK')
		{
			int SenderPeerID;
			BxCore::Socket::RecvFully(This.Sock, (byte*) &SenderPeerID, 4, 3000, false);

			const int NewBufferSize = PacketSize - 8;
			byte* NewBuffer = new byte[NewBufferSize];
			BxCore::Socket::RecvFully(This.Sock, NewBuffer, NewBufferSize, 3000, false);

			for(int i = This.Talks.Length() - 1; 0 <= i; --i)
			{
				if(This.Talks[i].Peer == SenderPeerID)
				{
					Export::LoadShape(This.Talks[i].Doc, 0, NewBuffer, NewBufferSize);
					break;
				}
			}
			delete[] NewBuffer;
		}
	}
	return sysupdateresult_do_render;
}

void OnRender(callitalk_room& This, BxDraw& Draw)
{
    BxTRY(Draw, CLIP(XYXY(0, 0, Draw.Width(), CT_SIZEBASE * 2)))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(82, 55, 56));
		static string EvtName[5] = {"BTN_EXIT", "BTN_UNDO", "BTN_DONE", "BTN_OPEN", "BTN_CLOSE"};
		for(int i = 0; i < 3; ++i)
		{
			int ID = i;
			if(i == 2)
			{
				if(!This.IsChatOpened) ID = 3;
				else if(Command::GetShapeCount(This.ChatDoc) == 0)
					ID = 4;
			}
			BxTRY(Draw, CLIP(XYXY(Draw.Width() * i / 3, 0, Draw.Width() * (i + 1) / 3, Draw.Height()))
				>> COLOR(115, 94, 93), EvtName[ID])
			{
				rect Rect = XYXY(
                    Draw.CurrentCenter().x - CT_SIZEBASE * 4 / 5,
                    Draw.CurrentCenter().y - CT_SIZEBASE * 4 / 5,
                    Draw.CurrentCenter().x + CT_SIZEBASE * 4 / 5,
                    Draw.CurrentCenter().y + CT_SIZEBASE * 4 / 5);
				Draw.Rectangle(FILL, Rect, FORM(&This.BtnImage[ID]));
			}
		}
	}

    BxTRY(Draw, CLIP(XYXY(0, CT_SIZEBASE * 2, Draw.Width(), Draw.Height())))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(155, 186, 216));
		BxTRY(Draw, HOTSPOT(This.ViewOx, This.ViewOy))
		{
			for(int i = 0; i <= This.Talks.Length(); ++i)
			{
				const bool IsTalk = (i < This.Talks.Length());
				const float PosY = (IsTalk)? -This.ScrollPos : 0;
				const int Doc = (IsTalk)? This.Talks[i].Doc : This.ChatDoc;
				const Zoom DocZoom = Command::Canvas::GetZoom(Doc);
				const Rect DocArea = Command::Canvas::GetArea(Doc);
				const Rect ViewRect = DocArea * DocZoom;

				// 채팅배경
				if(IsTalk)
				{
					if(This.Talks[i].Peer == This.PeerID)
					{
                        static const float BorderX1 = CT_SIZEBASE * 10 / 50;
                        static const float BorderX2 = CT_SIZEBASE * 15 / 50;
                        static const float BorderY = CT_SIZEBASE * 5 / 50;
						Draw.Rectangle9P(&This.ChatImage[0], XYXY(
							ViewRect.l - BorderX1, ViewRect.t - BorderY + PosY,
							ViewRect.r + BorderX2, ViewRect.b + BorderY + PosY));
					}
					else
					{
                        static const float BorderX1 = CT_SIZEBASE * 15 / 50;
                        static const float BorderX2 = CT_SIZEBASE * 10 / 50;
                        static const float BorderY = CT_SIZEBASE * 5 / 50;
						Draw.Rectangle9P(&This.ChatImage[1], XYXY(
							ViewRect.l - BorderX1, ViewRect.t - BorderY + PosY,
							ViewRect.r + BorderX2, ViewRect.b + BorderY + PosY));
					}
				}
				else
				{
					if(!This.IsChatOpened) continue;
					Draw.Rectangle(FILL, XYXY(ViewRect.l, ViewRect.t, ViewRect.r, ViewRect.b),
						COLOR(0, 0, 0) >> OPACITY(32));
				}

				// 도형들
				point ScenePos = BxScene::GetPosition("callitalk_room");
				ScenePos.x /= DocZoom.scale;
				ScenePos.y /= DocZoom.scale;
				linkstep1 doclink = Adapter::GetDocumentLink(Doc);
				const float ScrollPosX = (IsTalk)? ScenePos.x : ScenePos.x - This.LastDrawPos / DocZoom.scale;
                const float ScrollPosY = ScenePos.y + (CT_SIZEBASE + PosY) / DocZoom.scale;
				BxTRY(Draw, CLIP(XYXY(ViewRect.l, ViewRect.t + PosY, ViewRect.r, ViewRect.b + PosY)))
				for(int l = 0, lend = Adapter::GetLayerCount(doclink); l < lend; ++l)
				{
					linkstep2 layerlink = Adapter::GetLayerLink(doclink, l);
					if(!layerlink || !Adapter::IsLayerVisibled(layerlink))
						continue;
					for(int dyn = 0; dyn < 2; ++dyn)
					for(int s = 0, send = Adapter::GetShapeCount(layerlink, dyn != 0); s < send; ++s)
					{
						linkstep3 shapelink = Adapter::GetShapeLink(layerlink, dyn != 0, s);
						if(!shapelink) continue;
						if(CONST_STRING("PlatyVGElement::MeshAqua") == Adapter::GetShapeMeshType(shapelink))
						{
							const int CurMeshCount = Adapter::GetShapeMeshCount(shapelink);
							if(2 <= CurMeshCount)
							{
								const MeshAqua* CurMesh = (const MeshAqua*) Adapter::GetShapeMeshArray(shapelink);
								color_x888 ShapeColor = RGB32(
									Adapter::GetShapeColorRed(shapelink),
									Adapter::GetShapeColorGreen(shapelink),
									Adapter::GetShapeColorBlue(shapelink));
								BxCore::OpenGL2D::RenderStripDirectly(
									CurMeshCount, CurMesh,
									Adapter::GetShapeColorAlpha(shapelink),
									Adapter::GetShapeColorAqua(shapelink), ShapeColor,
									DocZoom.offset.x + ScrollPosX, DocZoom.offset.y + ScrollPosY, DocZoom.scale,
									Adapter::GetShapeMatrixM11(shapelink),
									Adapter::GetShapeMatrixM12(shapelink),
									Adapter::GetShapeMatrixM21(shapelink),
									Adapter::GetShapeMatrixM22(shapelink),
									Adapter::GetShapeMatrixDx(shapelink),
									Adapter::GetShapeMatrixDy(shapelink));
							}
						}
					}
				}
			}
		}
	}
}
