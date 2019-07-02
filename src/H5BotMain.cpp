#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

#include "H5Bot.h"
#include <iostream>

#if 0
int main(int argc, char* argv[])
{
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv))
	{
		cout << "Unable to find or parse settings." << endl;
		return 1;
	}
	coordinator.SetStepSize(1);
	coordinator.SetRealtime(true);
	coordinator.SetMultithreaded(true);
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, new CH5Bot()),
		//sc2::PlayerSetup(sc2::PlayerType::Observer,Util::GetRaceFromString(enemyRaceString)),
		CreateComputer(sc2::Race::Terran, sc2::Difficulty::Easy)
	});

	// Start the game.
	coordinator.LaunchStarcraft();
	coordinator.StartGame("C:/Program Files (x86)/StarCraft II/Maps/PortAleksanderLE_Hackthon.SC2Map");

	// Step forward the game simulation.
	while (coordinator.Update())
	{
	}
}
#else
#include "LadderInterface.h"
int main(int argc, char* argv[])
{
	RunBot(argc, argv, new CH5Bot(), sc2::Race::Terran);

	return 0;
}
#endif
