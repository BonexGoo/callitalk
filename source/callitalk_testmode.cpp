#include <BxScene.hpp>

FRAMEWORK_SCENE(unknown, "callitalk_testmode")

syseventresult OnEvent(unknown& This, const sysevent& Event)
{
	if(Event.type == syseventtype_init)
	{
		BxScene::AddRequest("callitalk_door");
		BxScene::AddRequest("callitalk_server");
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(unknown& This)
{
	if(BxScene::IsActivity("callitalk_server"))
	{
		BxScene::SetFlow(sceneflow_accel);
		BxScene::SetRequest("callitalk_server", sceneside_up);
		BxScene::SubRequest("callitalk_testmode");
	}
	return sysupdateresult_do_render;
}

void OnRender(unknown& This, BxDraw& Draw)
{
}
