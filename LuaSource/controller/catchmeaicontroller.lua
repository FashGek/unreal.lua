local CatchMeAIController = Inherit(CppObjectBase, ACatchMeAIController)

function CatchMeAIController:BeginPlay()
end

function CatchMeAIController:Possess(InPawn)
	InPawn:GC(self)
end

return CatchMeAIController