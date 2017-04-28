local CMPlayerControllerBase = require "controller.cmplayercontrollerbase"
local CMPlayerController = Inherit(CMPlayerControllerBase, ACatchMePlayerController)

function CMPlayerController:Ctor( )
	self.SpawnActors = {}
	self.Count = 0
end

function CMPlayerController:HandleInput(name, ...)
	self[name](self, ...)
end

function CMPlayerController:BeginPlay( )
	-- A_(self.Role, self:GetRemoteRole())
	if self.Role ~= ENetRole.ROLE_Authority then
		self.TestUI = require "ui.test":NewCpp(self, self)
	end
end

function CMPlayerController:InputTap_Press(Pos)
end	

function CMPlayerController:ClearAllCharacter( )
	for actor in pairs(self.SpawnActors) do
		actor:Release()
	end
	self.SpawnActors = {}
end

function CMPlayerController:InputTap_Release(Pos, HoldTime)
	local HitResult = FHitResult.New()
	local isHit = self:GetHitResult(Pos[1], Pos[2], HitResult, ECollisionChannel.ECC_Pawn)
	if isHit then
		if self:IsAuth() then
			self:MoveToPos(HitResult.ImpactPoint)
		else
			self:MoveToLocation(HitResult.ImpactPoint)
		end
	end
end		

function CMPlayerController:MoveToPos(Pos)
	UNavigationSystem.SimpleMoveToLocation(self, Pos)
end

function CMPlayerController:InputTap_Hold(Pos, HoldTime)
	-- A_("Hold", Pos[1], Pos[2], HoldTime)
end		

function CMPlayerController:InputTap_Move(Pos, HoldTime)
	-- A_("Move", Pos[1], Pos[2], HoldTime)
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

return CMPlayerController