param ( [Parameter(Position=0)] [string]$action = "--help" )

# force gcc (by default it tries to use msvc)
$env:CC = "gcc"
$env:CXX = "g++"

$PROJ_DIR = ".\"
$BUILD_DIR = ".\build"

# temp
# winget install Kitware.CMake

function ShowHelp {
    Write-Host "usage: .\build.ps1 [--configure|--build|--clean|--build-clean|--run]"
    Write-Host "--configure: configure project"
    Write-Host "--build: build elterclick"
    Write-Host "--build-clean: clean and build"
    Write-Host "--clean: clean build directory"
    Write-Host "--run: run the application"
    exit 0
}

function DownloadCPM {
    if (-not (Test-Path -Path ".\CPM\")) {
        New-Item -Path ".\CPM" -ItemType "Directory" | Out-Null
    }
    Invoke-WebRequest -Uri "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake" -OutFile "./CPM/CPM.cmake"
}

function Configure {
    git submodule update --init --recursive

    # download cpm and shit
    if (-not (Test-Path -Path ".\CPM\CPM.cmake")) {
        DownloadCPM
    }

    # create build dir
    if (-not (Test-Path $BUILD_DIR)) {
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }

    cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=OFF -S $PROJ_DIR -B $BUILD_DIR      
}

function Build {
    if (-not (Test-Path $BUILD_DIR)) {
        Configure
    }
    Push-Location $BUILD_DIR
    cmake --build . --config Release -j 4
    Pop-Location
}

function Clean {
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
    }
}

function Run {
    if (-not (Test-Path $BUILD_DIR)) {
        Write-Host "build directory does not exist. please run --configure or --build first."
        exit 1
    }
    Push-Location $BUILD_DIR
    if (Test-Path ".\elterclick.exe") { & ".\elterclick.exe" } 
    else { Write-Host "executable not found" }
    Pop-Location
}

switch ($action) {
    "--help" { ShowHelp }
    "--run" { Run }
    "--configure" { Configure }
    "--build" { Build }
    "--clean-build" {
        mingw32-make.exe -C build clean -j 4
        Build
    }
    "--clean" { Clean }
    default { Write-Host "Invalid parameter: $action" ShowHelp }
}