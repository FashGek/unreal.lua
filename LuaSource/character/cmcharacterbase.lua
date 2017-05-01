local CMCharacterBase = Inherit(CppObjectBase, ACMCharacterBase)

function CMCharacterBase:Ctor()
	-- if G_GameStatics.GameMode then
	-- 	G_GameStatics.GameMode:AddPlayerController(self)
	-- end
end

function CMCharacterBase:Tick(DeltaSeconds)
	
end

function CMCharacterBase:BeginPlay()
	if self.Role == 1 then
		G_SimulateProxy[self] = true
		self.m_OnDestroyed = self.OnDestroyed
		local function OnDestroy()
			G_SimulateProxy[self] = false
		end
		self.m_OnDestroyed:Add(OnDestroy)
	end
end

return CMCharacterBase
