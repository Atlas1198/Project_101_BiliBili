#include <iostream>
#include <unordered_map>
#include <vector>

#include "netcommon.h"

class GameServer : public olc::net::server_interface<GameMsg>
{
public:
	GameServer(uint16_t nPort) : olc::net::server_interface<GameMsg>(nPort)
	{
	}

	std::unordered_map<uint32_t, PlayerDescription> m_mapPlayerRoster;
	std::vector<uint32_t> m_vGarbageIDs;
	int nextInGameID = 0;
	//std::vector<int> availableInGameIDs = { 3, 2, 1, 0 };
	//uint32_t gameDesignerID = 0;

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		return true;
	}

	void OnClientValidated(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;

		msg << nextInGameID++;

		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		if (client)
		{
			if (m_mapPlayerRoster.find(client->GetID()) == m_mapPlayerRoster.end())
			{
				// client never added to roster, so just let it disappear
				std::cout << "Was not in the list";
			}
			else
			{
				auto &pd = m_mapPlayerRoster[client->GetID()];
				std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.uniqueID) + "\n";
				m_mapPlayerRoster.erase(client->GetID());
				m_vGarbageIDs.push_back(client->GetID());

				//olc::net::message<GameMsg> m;
				//m.header.id = GameMsg::Server_GetPing;
				//MessageAllClients(m);
			}
		}

	}

	void OnMessage(std::shared_ptr<olc::net::connection<GameMsg>> client, olc::net::message<GameMsg> &msg) override
	{
		if (!m_vGarbageIDs.empty())
		{
			for (auto pid : m_vGarbageIDs)
			{
				olc::net::message<GameMsg> m;
				m.header.id = GameMsg::Game_RemovePlayer;
				m << (int)m_mapPlayerRoster.size() << pid;
				std::cout << "Removing " << pid << "\n";
				std::cout << (int)m_mapPlayerRoster.size() << " Players in\n";
				MessageAllClients(m);
			}
			m_vGarbageIDs.clear();
		}



		switch (msg.header.id)
		{
		case GameMsg::Client_RegisterWithServer:
		{
			PlayerDescription desc;
			msg >> desc;
			desc.uniqueID = client->GetID();
			m_mapPlayerRoster.insert_or_assign(desc.uniqueID, desc);

			olc::net::message<GameMsg> msgSendID;
			msgSendID.header.id = GameMsg::Client_AssignID;
			msgSendID << desc.uniqueID;
			MessageClient(client, msgSendID);

			for (const auto &player : m_mapPlayerRoster)
			{
				olc::net::message<GameMsg> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = GameMsg::Game_AddPlayer;
				msgAddOtherPlayers << (int)m_mapPlayerRoster.size() << player.second;
				MessageClient(client, msgAddOtherPlayers);
			}

			olc::net::message<GameMsg> msgAddNewPlayer;
			msgAddNewPlayer.header.id = GameMsg::Game_AddPlayer;
			msgAddNewPlayer << (int)m_mapPlayerRoster.size() << desc;
			MessageAllClients(msgAddNewPlayer, client);
			
			std::cout << (int)m_mapPlayerRoster.size() << " Players in\n";

			break;
		}

		case GameMsg::Client_UnregisterWithServer:
		{
			break;
		}

		case GameMsg::Game_UpdatePlayer:
		{
			// Simply bounce update to everyone except incoming client
			MessageAllClients(msg, client);
			break;
		}

		/*
		case GameMsg::Client_RequestDesigner:
		{
			olc::net::message<GameMsg> respondMsg;
			respondMsg.header.id = GameMsg::Server_RespondDesignerRequest;

			if (gameDesignerID == 0)
			{
				gameDesignerID = client->GetID();
				respondMsg << true;
			}
			else
			{
				respondMsg << false;
			}

			MessageClient(client, respondMsg);
			break;
		}

		case GameMsg::Client_RequestChangeParameter:
			msg.header.id = GameMsg::Server_ChangeParameter;
			MessageAllClients(msg);
			break;
			*/
		}
		

	}

};



int main()
{
	GameServer server(60000);
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	return 0;
}