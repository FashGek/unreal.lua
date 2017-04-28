local CMCharacterBase = Inherit(CppObjectBase, ACMCharacterBase)

function CMCharacterBase:Ctor()
	if G_GameStatics.GameMode then
		G_GameStatics.GameMode:AddPlayerController(self)
	end
end

function CMCharacterBase:Tick(DeltaSeconds)
	
end

function CMCharacterBase:BeginPlay( )
	
end

return CMCharacterBase
