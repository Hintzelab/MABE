import platform ## system identification
import uuid ## unique guid generator for vs project files
import collections ## defaultdict
import os

def make_visual_studio_project(units, f_filename):
    outString = ''
    SDKversion = "10.0.16299.0" ## assume win 10...
    platformToolset = "v"
    if platform.system() == 'Windows':
        try:
            hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows")
            #SDKversion = winreg.EnumValue(hkey, 0)[1]
            SDKversion = str(winreg.QueryValueEx(hkey, "CurrentVersion")[0])
            firstPeriod = SDKversion.find('.')
            secondPeriod = SDKversion.find('.',firstPeriod+1)
            SDKversion = SDKversion[0:secondPeriod]
        except:
            try:
                hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots")
                SDKversion = winreg.EnumKey(hkey, 0)
            except:
                print("Warning: Unknown windows version. You will need to retarget the generated project manually in Visual Studio.")
                print("Warning: Could not find Windows SDK version information on this machine.")
                print("Warning: Please contact the authors so they can identify your setup and add it to identification.")
        hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\debug\\x64")
        platformToolset += str(winreg.QueryValueEx(hkey,"Major")[0]) + str(winreg.QueryValueEx(hkey,"Minor")[0])[0]

    outString += """<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>15.0</VCProjectVersion>
    <ProjectGuid>{{{0}}}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <WindowsTargetPlatformVersion>{1}</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LinkIncremental>true</LinkIncremental>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LinkIncremental>true</LinkIncremental>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <WarningLevel>Level3</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <Optimization>Disabled</Optimization>
    </ClCompile>
    <Link>
      <TargetMachine>MachineX86</TargetMachine>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <WarningLevel>Level3</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
    </ClCompile>
    <Link>
      <TargetMachine>MachineX86</TargetMachine>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
    </Link>
  </ItemDefinitionGroup>
""".format(str(uuid.uuid4()),SDKversion,platformToolset)
    outString += "  <ItemGroup>\n" ## start cpp list
    for eachunit in units:
        if eachunit[f_filename].endswith('.cpp'):
            outString += '    <ClCompile Include="{0}" />\n'.format(eachunit[f_filename])
    outString += "  </ItemGroup>\n"
    outString += "  <ItemGroup>\n" ## start header list
    for eachunit in units:
        if eachunit[f_filename].endswith('.h'):
            outString += '    <ClInclude Include="{0}" />\n'.format(eachunit[f_filename])
    outString += "  </ItemGroup>"
    outString += """
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>
"""
    with open('mabe.vcxproj','w') as outfile:
        outfile.write(outString)



def make_x_code_project(units, f_uuid, f_filerefuuid, f_folder, f_filename):
    def newXcodeUUID():
        return ''.join(str(uuid.uuid4()).upper().split('-')[1:])
    for unit in units: ## create XUUIDs for each file
        unit[f_uuid]=newXcodeUUID()
        unit[f_filerefuuid]=newXcodeUUID()
    folders=[]
    for eachunit in units:
        folders.append(eachunit[f_folder])
    folders=sorted(list(filter(bool,list(set(folders))))) #removes dups & empties, then sorts
    folderuuids={}
    for folder in folders:
        folderuuids[folder] = newXcodeUUID()
    folderFiles=collections.defaultdict(list)
    for unit in units:
        foldername=unit[f_folder]
        folderFiles[foldername].append( (unit[f_filename], True, unit[f_filerefuuid]) ) ## true for files, false for folders
    for folderName in folders:
        if '/' in folderName:
            parentName=folderName[0:folderName.rfind('/')]
            folderFiles[parentName].append( (folderName, False, folderuuids[folderName]) )
        else:
            folderFiles[''].append( (folderName, False, folderuuids[folderName]) ) ## mabe-root is ''
    ## create UUIDs
    rootObjUUID=newXcodeUUID()
    productUUID=newXcodeUUID()
    copyPhaseUUID=newXcodeUUID()
    frameworksPhaseUUID=newXcodeUUID()
    sourcesPhaseUUID=newXcodeUUID()
    buildConfigurationListTargetUUID=newXcodeUUID()
    buildConfigurationListProjectUUID=newXcodeUUID()
    mainGroupUUID=newXcodeUUID()
    mainGroupProductUUID=newXcodeUUID()
    mainGroupProductsUUID=newXcodeUUID()
    nativeTargetUUID=newXcodeUUID()
    configurationDebugUUID=newXcodeUUID()
    configurationReleaseUUID=newXcodeUUID()
    buildConfigurationDebugUUID=newXcodeUUID()
    buildConfigurationReleaseUUID=newXcodeUUID()
    outString = '''// !$*UTF9*$!
{{
	archiveVersion = 1;
	classes = {{
	}};
	objectVersion = 48;
	objects = {{

/* Begin PBXBuildFile section */
'''.format()
    for unit in units:
        if unit[f_filename].endswith('.cpp'):
            outString += "		{0} /* {1} in Sources */ = {{isa = PBXBuildFile; fileRef = {2} /* {1} */; }};\n".format( unit[f_uuid], unit[f_filename], unit[f_filerefuuid] )
    outString += '''/* End PBXBuildFile section */

/* Begin PBXCopyFilesBuildPhase section */
		{0} /* CopyFiles */ = {{
			isa = PBXCopyFilesBuildPhase;
			buildActionMask = 2147483647;
			dstPath = /usr/share/man/man1/;
			dstSubfolderSpec = 0;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 1;
		}};
/* End PBXCopyFilesBuildPhase section */

/* Begin PBXFileReference section */
'''.format(copyPhaseUUID)
    outString += '		{0} /* MABE */ = {{isa = PBXFileReference; explicitFileType = "compiled.mach-o.executable"; includeInIndex = 0; path = mabe; sourceTree = BUILT_PRODUCTS_DIR; }};\n'.format(productUUID)
    for unit in units:
        if 'main.cpp' == unit[f_filename]: ## 'main.cpp' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
        elif unit[f_filename].endswith('.h'): ## '.h' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; lastKnownFileType = sourcecode.c.h; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
        else: ## '.cpp' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
    outString += '''/* End PBXFileReference section */

/* Begin PBXFrameworksBuildPhase section */
		{0} /* Frameworks */ = {{
			isa = PBXFrameworksBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXFrameworksBuildPhase section */

/* Begin PBXGroup section */
		{1} = {{
			isa = PBXGroup;
			children = (
				{2} /* MABE */,
				{3} /* Products */,
			);
			sourceTree = "<group>";
		}};
		{3} /* Products */ = {{
			isa = PBXGroup;
			children = (
				{4} /* MABE */,
			);
			name = Products;
			sourceTree = "<group>";
		}};
		{2} /* MABE */ = {{
			isa = PBXGroup;
			children = (
'''.format(frameworksPhaseUUID, mainGroupUUID, mainGroupProductUUID, mainGroupProductsUUID, productUUID)
    #for unit in units:
    #    outString += '				{0} /* {1} */,\n'.format(unit[f_filerefuuid], unit[f_filename])
    for item in folderFiles['']: ## loop through MABE-root items
        outString += '				{0} /* {1} */,\n'.format(item[2],item[0])
    outString += '''			);
			path = ./;
			sourceTree = "<group>";
		}};
'''.format()
    for folder,items in folderFiles.items():
        if folder:
            outString += '''		{0} /* {1} */ = {{
			isa = PBXGroup;
			children = (
'''.format(folderuuids[folder],folder[folder.rfind('/')+1:])
            for item in items:
                outString += '			{0} /* {1} */,\n'.format(item[2],item[0][item[0].rfind('/')+1:])
            outString += '''			);
			path = {0};
			sourceTree = "<group>";
		}};
'''.format(folder[folder.rfind('/')+1:])
    outString += '''
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
'''.format()
    outString += '''		{0} /* MABE */ = {{
			isa = PBXNativeTarget;
			buildConfigurationList = {1} /* Build configuration list for PBXNativeTarget "mabe" */;
			buildPhases = (
				{2} /* Sources */,
				{3} /* Frameworks */,
				{4} /* CopyFiles */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = mabe;
			productName = mabe;
			productReference = {5} /* MABE */;
			productType = "com.apple.product-type.tool";
                }};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
'''.format(nativeTargetUUID, buildConfigurationListTargetUUID, sourcesPhaseUUID, frameworksPhaseUUID, copyPhaseUUID, productUUID)
    outString += '''		{0} /* Project object */ = {{
			isa = PBXProject;
			attributes = {{
				LastUpgradeCheck = 9910;
				ORGANIZATIONNAME = MabeUser;
				TargetAttributes = {{
					{1} = {{
						CreatedOnToolsVersion = 9.1;
						ProvisioningStyle = Automatic;
					}};
				}};
			}};
'''.format(rootObjUUID, nativeTargetUUID)
    outString += '''			buildConfigurationList = {0} /* Build configuration list for PBXProject "mabe" */;
			compatibilityVersion = "Xcode 8.0";
			developmentRegion = en;
			hasScannedForEncodings = 0;
			knownRegions = (
				en,
			);
'''.format(buildConfigurationListProjectUUID)
    outString += '''			mainGroup = {0};
			productRefGroup = {1} /* Products */;
			projectDirPath = "";
			projectRoot = "";
			targets = (
				{2} /* MABE */,
			);
		}};
/* End PBXProject section */

/* Begin PBXSourcesBuildPhase section */
'''.format(mainGroupUUID, mainGroupProductsUUID, nativeTargetUUID)
    outString += '''		{0} /* Sources */ = {{
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
'''.format(sourcesPhaseUUID)
    for unit in units:
        if unit[f_filename].endswith('.cpp'):
            outString += '				{0} /* {1} in Sources */,\n'.format(unit[f_uuid], unit[f_filename])
    outString += '''			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
		{0} /* Debug */ = {{'''.format(configurationDebugUUID)
    outString += '''
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++14";
				CLANG_CXX_LIBRARY = "libc++";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				CODE_SIGN_IDENTITY = "-";
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = dwarf;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_TESTABILITY = YES;
				GCC_C_LANGUAGE_STANDARD = gnu11;
				GCC_DYNAMIC_NO_PIC = NO;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_OPTIMIZATION_LEVEL = 0;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=0",
					"$(inherited)",
				);
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 10.13;
				MTL_ENABLE_DEBUG_INFO = YES;
				ONLY_ACTIVE_ARCH = YES;
				SDKROOT = macosx;
			}};
			name = Debug;
		}};
'''.format()
    outString += '		{0} /* Release */ = {{'.format(configurationReleaseUUID)
    outString += '''
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++14";
				CLANG_CXX_LIBRARY = "libc++";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				CODE_SIGN_IDENTITY = "-";
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
				ENABLE_NS_ASSERTIONS = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				GCC_C_LANGUAGE_STANDARD = gnu11;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 10.13;
				MTL_ENABLE_DEBUG_INFO = NO;
				SDKROOT = macosx;
			}};
			name = Release;
		}};'''.format()
    outString += '''
		{0} /* Debug */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				CODE_SIGN_STYLE = Automatic;
				PRODUCT_NAME = "$(TARGET_NAME)";
			}};
			name = Debug;
		}};
		{1} /* Release */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				CODE_SIGN_STYLE = Automatic;
				PRODUCT_NAME = "$(TARGET_NAME)";
			}};
			name = Release;
		}};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */'''.format(buildConfigurationDebugUUID, buildConfigurationReleaseUUID)
    outString += '''
		{0} /* Build configuration list for PBXProject "doggie" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{1} /* Debug */,
				{2} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
		{3} /* Build configuration list for PBXNativeTarget "doggie" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{4} /* Debug */,
				{5} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
/* End XCConfigurationList section */
	}};
	rootObject = {6} /* Project object */;
}}
'''.format(buildConfigurationListProjectUUID,
        configurationDebugUUID,
        configurationReleaseUUID,
        buildConfigurationListTargetUUID,
        buildConfigurationDebugUUID,
        buildConfigurationReleaseUUID,
        rootObjUUID)
    if not os.path.isdir('mabe.xcodeproj'):
        os.mkdir('mabe.xcodeproj')
    with open('mabe.xcodeproj/project.pbxproj','w') as outfile:
        outfile.write(outString)