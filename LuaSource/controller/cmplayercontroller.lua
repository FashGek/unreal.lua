local CMPlayerControllerBase = require "controller.cmplayercontrollerbase"
local CMPlayerController = Inherit(CMPlayerControllerBase, ACatchMePlayerController)

function CMPlayerController:Ctor( )
	self.SpawnActors = {}
	self.Count = 0
	self.bAttachToPawn = true
	self.m_DefaultPawnClass = APawn.FClassFinder("/Game/Git/TopDownCharacter")
end

function CMPlayerController:HandleInput(name, ...)
	self[name](self, ...)
end

function CMPlayerController:BeginPlay( )
	-- A_(self.Role, self:GetRemoteRole())
	if self:IsLocalPlayerController() then
		self.TestUI = require "ui.test":NewCpp(self, self)
	end
	if self:IsAuth() then
	-- 	local Character 
		local SpawnLocation = FVector.New(-490, -86, 292)
		local SpawnRotation = FRotator.New(0,0,0)
		local transfrom = UKismetMathLibrary.MakeTransform(SpawnLocation, SpawnRotation, FVector.New(1, 1, 1))
		local spawnActor = UGameplayStatics.BeginDeferredActorSpawnFromClass(self, self.m_DefaultPawnClass, transfrom, ESpawnActorCollisionHandlingMethod.AlwaysSpawn)
		spawnActor = UGameplayStatics.FinishSpawningActor(spawnActor, transfrom)
		self.PlayCharacter = spawnActor
		self.m_PlayCharacter = spawnActor
	end
end

function CMPlayerController:InputTap_Press(Pos)
	self.m_Pawn:StartPress(Pos)
	-- self.PlayCharacter:SetActorHiddenInGame(true)
	A_(self.PlayCharacter)
end	

function CMPlayerController:InputTap_Release(Pos, HoldTime)
	if not self.m_bHasMoveScreen then
		local Hit = FHitResult.New()
		if self:GetHitResult(Pos[1], Pos[2], Hit) then
			self:S_MoveToLocation(Hit.ImpactPoint)
			-- self:S_MoveToPos(Hit.ImpactPoint)
		end
	end
	self.m_bHasMoveScreen = false
end		

function CMPlayerController:MoveToPos(Pos)
	-- UNavigationSystem.SimpleMoveToLocation(self, Pos)
	A_(self.m_PlayCharacter)
	self.m_PlayCharacter:GetController():MoveToLocation(Pos)
	-- A_(self.m_theproxy:GetController())
end

function CMPlayerController:InputTap_Hold(Pos, HoldTime)
end		

function CMPlayerController:InputTap_Move(Pos, HoldTime, change)
	self.m_Pawn:Move(Pos)
	self.m_bHasMoveScreen = true
end

function CMPlayerController:GetAnimIns()
	local Character = self.Character
	if Character then
		local Mesh = Character.Mesh
		if Mesh then
			return Mesh:GetAnimInstance()
		end
	end
end

function CMPlayerController_GetLifetimeReplicatedProps()
	local t = {}
	table.insert(t, FReplifetimeCond.NewItem("PlayCharacter", ELifetimeCondition.COND_AutonomousOnly))
	return t
end

return CMPlayerController