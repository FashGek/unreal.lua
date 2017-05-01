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
	if self.m_controller and self.m_controller.TestUI then
		self.m_controller.TestUI:Txt1(self.WalkSpeed)		
	else
		self.m_controller = self.m_Pawn:GetController()
	end
end



return AnimInstance