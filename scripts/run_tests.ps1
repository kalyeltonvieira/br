# run_tests.ps1 – discovers *.brt files and runs them with the BR interpreter
# Adjust the path to the interpreter if needed
$brExe = "D:\br\bin\br.exe"  # <- change if your interpreter lives elsewhere
$testRoot = "D:\br\lib"
$failed = 0

Get-ChildItem -Path $testRoot -Recurse -Filter "*_teste.brt" | ForEach-Object {
    $testFile = $_.FullName
    Write-Host "Running test: $testFile"
    & $brExe $testFile > $null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[FAIL] $testFile" -ForegroundColor Red
        $failed++
    } else {
        Write-Host "[PASS] $testFile" -ForegroundColor Green
    }
}

if ($failed -eq 0) {
    Write-Host "All tests passed!" -ForegroundColor Green
} else {
    Write-Host "$failed test(s) failed." -ForegroundColor Red
    exit 1
}
Write-Host "Test run completed."
