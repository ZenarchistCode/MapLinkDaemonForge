class ActionMapLinkOpenOnTerminal extends ActionInteractBase
{
	string mapName = "";
	
	void ActionMapLinkOpenOnTerminal()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
	}
	
	override string GetText()
	{
		return "#STR_MapLink_OpenTerminal";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (GetMapLinkConfig().GetDepaturePoint(player.GetPosition()))
		{
			return true;
		}

		return false;
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		
		PlayerBase player = PlayerBase.Cast(action_data.m_Player);
		
		if (player)
		{
			player.MapLinkUpdateClientSettingsToServer();

			if (!m_DeparturePointMenu)
			{
				m_DeparturePointMenu = DeparturePointMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(MAPLINK_DEPARTUREPOINTMENU, NULL));
			}

			m_DeparturePointMenu.SetDeparturePoint(GetMapLinkConfig().GetDepaturePoint(player.GetPosition()));
		}
	}

	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);

		action_data.m_Player.SyncLastMapTransferTimestamp();

		//Something should go here for better validation server side? Right now it just checks for the nearest depaturePoint to the player within 50 meters
	}
}