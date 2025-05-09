param (
    [Parameter(Position=0)]
    [string]$action = "--help"
)

$PROJ_DIR = ".\"
$BUILD_DIR = ".\build"

function ShowHelp {
    Write-Host "usage: .\build.ps1 [--configure|--build|--clean|--build-clean|--run]"
    Write-Host "--configure: configure project"
    Write-Host "--build: build elterclick"
    Write-Host "--build-clean: clean and build"
    Write-Host "--clean: clean build directory"
    Write-Host "--run: run the application"
    exit 0
}

function Configure {
    if (-not (Test-Path $BUILD_DIR)) {
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }
    
    cmake `
        -G "MinGW Makefiles" `
        -DCMAKE_C_COMPILER="C:\MinGW\bin\gcc.exe" `
        -DCMAKE_CXX_COMPILER="C:\MinGW\bin\g++.exe" `
        -DBUILD_SHARED_LIBS=OFF -S $PROJ_DIR -B $BUILD_DIR 
}

function Build {
    param (
        [bool]$clean = $false
    )

    if (-not (Test-Path $BUILD_DIR)) {
        Write-Host "build directory does not exist. please run --configure first."
        exit 1
    }
    
    Push-Location $BUILD_DIR
    
    if ($clean) {
        cmake --build . --target clean
    }
    
    cmake --build . --config Release --parallel 4 -j 4
    
    Pop-Location
}

function Clean {
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
    }
}

function Run {
    if (-not (Test-Path $BUILD_DIR)) {
        Write-Host "build directory does not exist. please run --configure first."
        exit 1
    }
    
    Push-Location $BUILD_DIR
    
    if (Test-Path ".\Release\elterclick.exe") {
        & ".\Release\elterclick.exe"
    } elseif (Test-Path ".\elterclick.exe") {
        & ".\elterclick.exe"
    } else {
        Write-Host "executable not found"
    }
    
    Pop-Location
}

switch ($action) {
    "--help" { ShowHelp }
    "" { ShowHelp }
    "--configure" { Configure }
    "--build" { Build $false }
    "--build-clean" { Build $true }
    "--clean" { Clean }
    "--run" { Run }
    default { 
        Write-Host "Invalid parameter: $action"
        ShowHelp
    }
}
