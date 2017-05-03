local AnimInstance = Inherit(CppObjectBase, ULuaAnimInstance)

function AnimInstance:NativeInitializeAnimation()
	local Pawn = self:TryGetPawnOwner()
	if Pawn then
		Pawn:GC(self)
		self.m_Pawn = Pawn
	end
end

function AnimInstance:NativeUpdateAnimation(delta)
	self.WalkSpeed = #self.m_Pawn:GetVelocity()
	local controller = UGameplayStatics.GetPlayerController(self, 0)
	if controller and controller.PlayCharacter == self.m_Pawn and controller.TestUI then
		controller.TestUI:Txt1(self.WalkSpeed)		
	end
end



return AnimInstance