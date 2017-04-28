local CMGameModeBase = require "gameplay.gamemode.cmgamemodebase"
local CMGameMode = Inherit(CMGameModeBase, ACatchMeGameMode)

function CMGameMode:Ctor()
	self.PlayerControllerClass = ACatchMePlayerController.Class()
	self.DefaultPawnClass = APawn.FClassFinder("/Game/TopDownCPP/Blueprints/TopDownCharacter")
end

function CMGameMode:BeginPlay()

end

return CMGameMode