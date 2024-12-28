function Check-Rebuild($infile, $outfile) {
    return -not (Test-Path $outfile) -or ((Get-Item $outfile).LastWriteTime -lt (Get-Item "$infile").LastWriteTime)
}

function Clean-File($file) {
    if (Test-Path $file) {
        rm $file -r -force
    }
}

function Make-Path($name) {
    if (-not (Test-Path $name)) {
        New-Item -Path $name -ItemType Directory | Out-Null
    }

    return Resolve-Path $name
}

function Rem-Path($name) {
    if (Test-Path $name) {
        rm $name -r -force
    }
}

function Smart-Copy($dir, $out) {
    foreach ($file in Get-ChildItem -Path $dir -Recurse) {
        
        if (Check-Rebuild "$dir/$file" "$out/$file") {
            Copy-Item "$dir/$file" "$out/$file"
            Write-Host "$file smart copied"
        }        
    }
}

$currentScript = $MyInvocation.MyCommand.Path
$executable = "main.exe"

function Build-Vars {
    $global:outputDir = Make-Path "./build"
    $global:buildCache = Make-Path "$outputDir/cache"
    $global:inputDir = Make-Path "./project"
    
    $global:objDir = Make-Path "$outputDir/obj"
    $global:srcDir = Make-Path "$inputDir/src"
    $global:incDir = Make-Path "$inputDir/include"
    
    $global:inputAssetDir = Make-Path "$inputDir/assets"
    $global:inputShaderDir = Make-Path "$inputDir/shaders"
    $global:outputAssetDir = Make-Path "$outputDir/assets"
    $global:outputShaderDir = Make-Path "$outputDir/shaders"

    Copy-Item -Path $currentScript -Destination $outputDir -Force
}

function Build-Clean {
    Rem-Path $outputDir
    Build-Vars
}

Build-Vars

$CXX = "gcc"
$CXXFLAGS = "-I$incDir -Ilib -Wall -c -g"
$LDFLAGS = "-Llib -lglad -lglfw3 -lopengl32 -lgdi32 -lstdc++ -lstb"

$objectBuild = "$CXX $LDFLAGS $CXXFLAGS"

function Check-Build-Cache {
    # Rem-Path $buildCache
    $global:buildCache = Make-Path "$outputDir/cache"

    $cacheDated = $false

    ## check and update header cache
    foreach ($file in Get-ChildItem -Path $incDir -File -Filter *.hpp) {
        $cachePath = "$buildCache/$($file.BaseName).hc"
        $filePath = "$incDir/$file"

        $updated = ($cacheDated) -or (Check-Rebuild $filePath $cachePath)
        $cacheDated = $cacheDated -or $updated

        Rem-Path $cachePath
        New-Item $cachePath
    }

    ## check and update source cache
    foreach ($file in Get-ChildItem -Path $srcDir -File -Filter *.cpp) {
        $cachePath = "$buildCache/$($file.BaseName).sc"
        $filePath = "$srcDir/$file"

        $updated = ($cacheDated) -or (Check-Rebuild $filePath $cachePath)
        $cacheDated = $cacheDated -or $updated

        Rem-Path $cachePath
        New-Item $cachePath
    }

    return $cacheDated
}

function Build-Compile {
    Clean-File $executable

    ## build source files
    if (Test-Path $inputDir) {
        ## test for force rebuild (header modified)
        
        $cacheDated = Check-Build-Cache

        foreach ($file in Get-ChildItem -Path $srcDir -File -Filter *.cpp) {
            $outputFile = Join-Path $objDir "$($file.BaseName).o"
            
            ## checks to make sure the file has actually been modified
            if ($cacheDated -or (Check-Rebuild "$srcDir/$($file)" $outputFile)) {
                Invoke-Expression "$CXX $srcDir/$file $LDFLAGS $CXXFLAGS -o $outputFile"
                Write-Output "$CXX $srcDir/$file $LDFLAGS $CXXFLAGS -o $outputFile"
            }
        }
    }

    ## link executable
    $objects = (Get-ChildItem -Path $objDir -File -Filter *.o | ForEach-Object { $_.FullName }) -join ' '
    Invoke-Expression "$CXX $objects -o $outputDir/$executable $LDFLAGS"
    Write-Host "Build Completed..."
}

function Run {
    # & $executable
    Push-Location -Path $outputDir; Invoke-Expression ".\$executable"; Pop-Location
}

function Build-Project {
    Build-Clean
    Build-Compile
    Smart-Copy $inputAssetDir $outputAssetDir
    Smart-Copy $inputShaderDir $outputShaderDir
    Run
}

function Reload-Assets {
    Smart-Copy $inputAssetDir $outputAssetDir
    Smart-Copy $inputShaderDir $outputShaderDir
    Run
}