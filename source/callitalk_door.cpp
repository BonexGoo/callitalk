#include <BxScene.hpp>

class callitalk_door
{
public:
	id_font BigFont;
	id_font SmallFont;
	BxString RoomNumber;

public:
	callitalk_door()
	{
		if(!BxCore::Font::IsExistNick("image/cartoonstory.ttf"))
			BxCore::Font::NickOpen("image/cartoonstory.ttf", "image/cartoonstory.ttf");
		BigFont = BxCore::Font::Open("image/cartoonstory.ttf", CT_SIZEBASE);
		SmallFont = BxCore::Font::Open("image/cartoonstory.ttf", CT_SIZEBASE * 2 / 3);
	}
	~callitalk_door()
	{
		BxCore::Font::Close(BigFont);
		BxCore::Font::Close(SmallFont);
	}
};

FRAMEWORK_SCENE(callitalk_door, "callitalk_door")

syseventresult OnEvent(callitalk_door& This, const sysevent& Event)
{
	if(Event.type == syseventtype_button && Event.button.type == sysbuttontype_up)
	{
		if(StrSameCount(Event.button.name, "NUM-") == 4)
			This.RoomNumber += Event.button.name + 4;
		else if(StrCmp(Event.button.name, "BACK") == same)
			This.RoomNumber.DeleteRight(1);
		else if(StrCmp(Event.button.name, "ENTER") == same)
		{
			BxScene::SetFlow(sceneflow_decel);
			BxScene::AddRequest("callitalk_room", sceneside_center,
				scenelayer_1st, This.RoomNumber);
			BxScene::SubRequest("callitalk_door", sceneside_left);
		}
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(callitalk_door& This)
{
	return sysupdateresult_do_render;
}

void OnRender(callitalk_door& This, BxDraw& Draw)
{
	// 배경색
    Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(82, 55, 56));

	// 설명글
	Draw.TextW(This.BigFont, W2("\"방번호를"),
		XY(Draw.Width() / 2, Draw.Height() / 6 - CT_SIZEBASE / 4),
		textalign_right_bottom, COLOR(255, 236, 66));
	Draw.TextW(This.BigFont, W2("입력하세요\""),
		XY(Draw.Width() / 2 - CT_SIZEBASE, Draw.Height() / 6),
		textalign_left_top, COLOR(255, 236, 66));

    const int Gap = CT_SIZEBASE * 15 / 50;

	// 에디트박스
    BxTRY(Draw, CLIP(XYWH(Gap * 2, Draw.Height() / 3 - CT_SIZEBASE / 2,
        Draw.Width() - Gap * 4, CT_SIZEBASE)))
	{
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(192, 192, 192));
		Draw.Text(This.SmallFont, This.RoomNumber, Draw.CurrentCenter(),
			textalign_center_middle, COLOR(0, 0, 0));
	}

	// 숫자패드
	static string EvtName[12] = {
		"NUM-1", "NUM-2", "NUM-3", "NUM-4",
		"NUM-5", "NUM-6", "NUM-7", "NUM-8",
		"NUM-9", "NUM-0", "BACK", "ENTER"};
	static wstring KeyName[12] = {
		W2("1"), W2("2"), W2("3"), W2("4"),
		W2("5"), W2("6"), W2("7"), W2("8"),
		W2("9"), W2("0"), W2("←"), W2("GO")};
	static color_x888 KeyColor[12] = {
		RGB32(255, 255, 255), RGB32(255, 255, 255), RGB32(255, 255, 255), RGB32(255, 255, 255),
		RGB32(255, 255, 255), RGB32(255, 255, 255), RGB32(255, 255, 255), RGB32(255, 255, 255),
		RGB32(255, 255, 255), RGB32(255, 255, 255), RGB32(255, 64, 0), RGB32(0, 128, 255)};
	bool KeyEnable[12] = {
		true, true, true, true, true, true, true, true, true, true,
		(0 < This.RoomNumber.GetLength()), (3 < This.RoomNumber.GetLength())};
    BxTRY(Draw, CLIP(XYXY(Gap, Draw.Height() / 3 + CT_SIZEBASE / 2 + Gap,
		Draw.Width() - Gap, Draw.Height() - Gap)))
	for(int y = 0, i = 0; y < 3; ++y)
	for(int x = 0; x < 4; ++x, ++i)
	{
		BxTRY(Draw, CLIP(XYXY(
			Draw.Width() * x / 4 + Gap,
			Draw.Height() * y / 3 + Gap,
			Draw.Width() * (x + 1) / 4 - Gap,
			Draw.Height() * (y + 1) / 3 - Gap)), (KeyEnable[i])? EvtName[i] : nullptr)
		{
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(0, 0, 0) >> OPACITY(64));
			Draw.TextW(This.BigFont, KeyName[i], Draw.CurrentCenter(), textalign_center_middle,
				COLOR((KeyEnable[i])? KeyColor[i] : RGB32(128, 128, 128)));
		}
	}
}
