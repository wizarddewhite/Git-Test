# define function to get the target value
def addvalue(f): f + "abd";
.volumeClaimTemplates |= map(if .metadata.name == "sysvol" then .spec.resources.requests.storage=addvalue(.spec.resources.requests.storage) else . end)

