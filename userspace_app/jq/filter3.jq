.volumeClaimTemplates |= map(
	if .spec.storageClassName == "local-ssd" then
		.spec.resources.requests.storage=(
			.spec.resources.requests.storage | rtrimstr("Gi") | tonumber | . + 50 | tostring | . + "Gi"
		)
	else
		.
	end
)