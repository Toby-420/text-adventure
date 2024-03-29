$currentVersion = (Get-Content -Path "assets\text\currentver.txt" -Raw).Trim()
$currentVersion = [version](([regex]'[\d.]+').Matches($currentVersion).Value)
Write-Host "Current version: $currentVersion"

$latestRelease = Invoke-WebRequest https://api.github.com/repos/Toby-420/text-adventure/releases/latest

if ($latestRelease.StatusCode -eq 200) {
    $latestReleaseJson = $latestRelease.Content | ConvertFrom-Json
    $newVersion = [version](([regex]'^v?(\d+)(\.\d+)*').Matches($latestReleaseJson.tag_name).Value -replace '^v')
    Write-Host "Latest version: $newVersion"

    if ($currentVersion -lt $newVersion) {
        $response = Read-Host "A new version is available. Do you want to download the latest version? (Y/N)"
        if ($response.ToLower() -eq "y") {
            $latestReleaseUrl = ($latestReleaseJson.assets | Select-Object -First 1).browser_download_url
            Write-Host "Downloading latest version from $latestReleaseUrl"
            Start-Process $latestReleaseUrl
        }
    }
    else {
        Write-Host "You have the latest version."
    }
}
else {
    Write-Host "Error: Unable to get latest release information (HTTP status code $($latestRelease.StatusCode))."
}
