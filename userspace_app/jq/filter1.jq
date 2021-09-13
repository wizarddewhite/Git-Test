# replace class name to nvme if name is sysvol
.volumeClaimTemplates |= map(if .metadata.name == "sysvol" then .spec.storageClassName="nvme" else . end)

